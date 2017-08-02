/****************************************************************************
*
*    The MIT License (MIT)
*
*    Copyright (c) 2014 - 2017 Vivante Corporation
*
*    Permission is hereby granted, free of charge, to any person obtaining a
*    copy of this software and associated documentation files (the "Software"),
*    to deal in the Software without restriction, including without limitation
*    the rights to use, copy, modify, merge, publish, distribute, sublicense,
*    and/or sell copies of the Software, and to permit persons to whom the
*    Software is furnished to do so, subject to the following conditions:
*
*    The above copyright notice and this permission notice shall be included in
*    all copies or substantial portions of the Software.
*
*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
*    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
*    DEALINGS IN THE SOFTWARE.
*
*****************************************************************************
*
*    The GPL License (GPL)
*
*    Copyright (C) 2014 - 2017 Vivante Corporation
*
*    This program is free software; you can redistribute it and/or
*    modify it under the terms of the GNU General Public License
*    as published by the Free Software Foundation; either version 2
*    of the License, or (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*****************************************************************************
*
*    Note: This software is released under dual MIT and GPL licenses. A
*    recipient may use this file under the terms of either the MIT license or
*    GPL License. If you wish to use only one license not the other, you can
*    indicate your decision by deleting one of the above license notices in your
*    version of this file.
*
*****************************************************************************/


#include "gc_hal_kernel_linux.h"
#include "gc_hal_kernel_allocator.h"

#include <linux/pagemap.h>
#include <linux/seq_file.h>
#include <linux/mman.h>
#include <asm/atomic.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>

#define _GC_OBJ_ZONE    gcvZONE_OS

typedef struct _gcsCMA_PRIV * gcsCMA_PRIV_PTR;
typedef struct _gcsCMA_PRIV {
    atomic_t cmasize;
    gctBOOL cmaLimitRequest;
}
gcsCMA_PRIV;

struct mdl_cma_priv {
    gctPOINTER kvaddr;
    dma_addr_t physical;
};

static int gc_cma_usage_show(struct seq_file* m, void* data)
{
    gcsINFO_NODE *node = m->private;
    gckALLOCATOR Allocator = node->device;
    gcsCMA_PRIV_PTR priv = Allocator->privateData;
    long long size = (long long)atomic_read(&priv->cmasize);

    seq_printf(m, "type        n pages        bytes\n");
    seq_printf(m, "cma      %10llu %12llu\n", size, size * PAGE_SIZE);

    return 0;
}

static gcsINFO InfoList[] =
{
    {"cmausage", gc_cma_usage_show},
};

static void
_CMAAllocatorDebugfsInit(
    IN gckALLOCATOR Allocator,
    IN gckDEBUGFS_DIR Root
    )
{
    gcmkVERIFY_OK(
        gckDEBUGFS_DIR_Init(&Allocator->debugfsDir, Root->root, "cma"));

    gcmkVERIFY_OK(gckDEBUGFS_DIR_CreateFiles(
        &Allocator->debugfsDir,
        InfoList,
        gcmCOUNTOF(InfoList),
        Allocator
        ));
}

static void
_CMAAllocatorDebugfsCleanup(
    IN gckALLOCATOR Allocator
    )
{
    gcmkVERIFY_OK(gckDEBUGFS_DIR_RemoveFiles(
        &Allocator->debugfsDir,
        InfoList,
        gcmCOUNTOF(InfoList)
        ));

    gckDEBUGFS_DIR_Deinit(&Allocator->debugfsDir);
}

static gceSTATUS
_CMAFSLAlloc(
    IN gckALLOCATOR Allocator,
    INOUT PLINUX_MDL Mdl,
    IN gctSIZE_T NumPages,
    IN gctUINT32 Flags
    )
{
    gceSTATUS status;
    gcsCMA_PRIV_PTR priv = (gcsCMA_PRIV_PTR)Allocator->privateData;

    struct mdl_cma_priv *mdl_priv=gcvNULL;
    gckOS os = Allocator->os;

    gcmkHEADER_ARG("Mdl=%p NumPages=0x%zx", Mdl, NumPages);

    if (os->allocatorLimitMarker)
    {
        if (Flags & gcvALLOC_FLAG_CMA_LIMIT)
        {
            priv->cmaLimitRequest = gcvTRUE;
        }
        else if (priv->cmaLimitRequest == gcvTRUE)
        {
            gcmkONERROR(gcvSTATUS_NOT_SUPPORTED);
        }
    }

    gcmkONERROR(gckOS_Allocate(os, sizeof(struct mdl_cma_priv), (gctPOINTER *)&mdl_priv));
    mdl_priv->kvaddr = gcvNULL;

    mdl_priv->kvaddr = dma_alloc_writecombine(&os->device->platform->device->dev,
            NumPages * PAGE_SIZE,
            &mdl_priv->physical,
            GFP_KERNEL | gcdNOWARN);

    if (mdl_priv->kvaddr == gcvNULL)
    {
        gcmkONERROR(gcvSTATUS_OUT_OF_MEMORY);
    }

    Mdl->priv = mdl_priv;
    Mdl->dmaHandle = mdl_priv->physical;
    atomic_add(NumPages, &priv->cmasize);

    gcmkFOOTER_NO();
    return gcvSTATUS_OK;

OnError:
    if (mdl_priv)
    {
        gckOS_Free(os, mdl_priv);
    }
    gcmkFOOTER();
    return status;
}

static void
_CMAFSLFree(
    IN gckALLOCATOR Allocator,
    IN OUT PLINUX_MDL Mdl
    )
{
    gckOS os = Allocator->os;
    struct mdl_cma_priv *mdl_priv=(struct mdl_cma_priv *)Mdl->priv;
    gcsCMA_PRIV_PTR priv = (gcsCMA_PRIV_PTR)Allocator->privateData;
    dma_free_writecombine(&os->device->platform->device->dev,
            Mdl->numPages * PAGE_SIZE,
            mdl_priv->kvaddr,
            mdl_priv->physical);
     gckOS_Free(os, mdl_priv);
    atomic_sub(Mdl->numPages, &priv->cmasize);
}

static gctINT
_CMAFSLMapUser(
    IN gckALLOCATOR Allocator,
    IN PLINUX_MDL Mdl,
    IN gctBOOL Cacheable,
    OUT gctPOINTER * UserLogical
    )
{
    PLINUX_MDL      mdl = Mdl;
    struct mdl_cma_priv *mdl_priv=(struct mdl_cma_priv *)Mdl->priv;
    gckOS           os = Allocator->os;
    struct vm_area_struct * vma;
    gctPOINTER      userLogical = gcvNULL;

    gcmkHEADER_ARG("Allocator=%p Mdl=%p Cacheable=%d", Allocator, Mdl, Cacheable);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 4, 0)
    userLogical = (gctSTRING)vm_mmap(gcvNULL,
                    0L,
                    mdl->numPages * PAGE_SIZE,
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_NORESERVE,
                    0);
#else
    down_write(&current->mm->mmap_sem);

    userLogical = (gctSTRING)do_mmap_pgoff(gcvNULL,
                    0L,
                    mdl->numPages * PAGE_SIZE,
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED,
                    0);

    up_write(&current->mm->mmap_sem);
#endif

    gcmkTRACE_ZONE(
        gcvLEVEL_INFO, gcvZONE_OS,
        "%s(%d): vmaAddr->%p for phys_addr->%p",
        __FUNCTION__, __LINE__, userLogical, mdl
        );

    if (IS_ERR(userLogical))
    {
        gcmkTRACE_ZONE(
            gcvLEVEL_INFO, gcvZONE_OS,
            "%s(%d): do_mmap_pgoff error",
            __FUNCTION__, __LINE__
            );

        gcmkFOOTER_ARG("*status=%d", gcvSTATUS_OUT_OF_MEMORY);
        return gcvSTATUS_OUT_OF_MEMORY;
    }

    down_write(&current->mm->mmap_sem);

    vma = find_vma(current->mm, (unsigned long)userLogical);

    if (vma == gcvNULL)
    {
        up_write(&current->mm->mmap_sem);

        gcmkTRACE_ZONE(
            gcvLEVEL_INFO, gcvZONE_OS,
            "%s(%d): find_vma error",
            __FUNCTION__, __LINE__
            );

        gcmkFOOTER_ARG("*status=%d", gcvSTATUS_OUT_OF_RESOURCES);
        return gcvSTATUS_OUT_OF_RESOURCES;
    }

    /* Now map all the vmalloc pages to this user address. */
    if (mdl->contiguous)
    {
        /* map kernel memory to user space.. */
        if (dma_mmap_writecombine(&os->device->platform->device->dev,
                vma,
                mdl_priv->kvaddr,
                mdl_priv->physical,
                mdl->numPages * PAGE_SIZE) < 0)
        {
            up_write(&current->mm->mmap_sem);

            gcmkTRACE_ZONE(
                gcvLEVEL_WARNING, gcvZONE_OS,
                "%s(%d): dma_mmap_attrs error",
                __FUNCTION__, __LINE__
                );

            gcmkFOOTER_ARG("*status=%d", gcvSTATUS_OUT_OF_MEMORY);
            return gcvSTATUS_OUT_OF_MEMORY;
        }
    }
    else
    {
        gckOS_Print("incorrect mdl:conti%d\n",mdl->contiguous);
    }

    up_write(&current->mm->mmap_sem);

    gcmkVERIFY_OK(gckOS_CacheFlush(
        os,
        _GetProcessID(),
        mdl,
        gcvINVALID_ADDRESS,
        (gctPOINTER)userLogical,
        mdl->numPages * PAGE_SIZE
        ));

    *UserLogical = userLogical;

    gcmkFOOTER_NO();
    return gcvSTATUS_OK;
}

static void
_CMAUnmapUser(
    IN gckALLOCATOR Allocator,
    IN PLINUX_MDL Mdl,
    IN gctPOINTER Logical,
    IN gctUINT32 Size
    )
{
    if (unlikely(current->mm == gcvNULL))
    {
        /* Do nothing if process is exiting. */
        return;
    }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)
    if (vm_munmap((unsigned long)Logical, Size) < 0)
    {
        gcmkTRACE_ZONE(
                gcvLEVEL_WARNING, gcvZONE_OS,
                "%s(%d): vm_munmap failed",
                __FUNCTION__, __LINE__
                );
    }
#else
    down_write(&current->mm->mmap_sem);
    if (do_munmap(current->mm, (unsigned long)Logical, Size) < 0)
    {
        gcmkTRACE_ZONE(
                gcvLEVEL_WARNING, gcvZONE_OS,
                "%s(%d): do_munmap failed",
                __FUNCTION__, __LINE__
                );
    }
    up_write(&current->mm->mmap_sem);
#endif
}

static gceSTATUS
_CMAMapKernel(
    IN gckALLOCATOR Allocator,
    IN PLINUX_MDL Mdl,
    OUT gctPOINTER *Logical
    )
{
    struct mdl_cma_priv *mdl_priv=(struct mdl_cma_priv *)Mdl->priv;
    *Logical =mdl_priv->kvaddr;
    return gcvSTATUS_OK;
}

static gceSTATUS
_CMAUnmapKernel(
    IN gckALLOCATOR Allocator,
    IN PLINUX_MDL Mdl,
    IN gctPOINTER Logical
    )
{
    return gcvSTATUS_OK;
}

static gceSTATUS
_CMACache(
    IN gckALLOCATOR Allocator,
    IN PLINUX_MDL Mdl,
    IN gctPOINTER Logical,
    IN gctUINT32 Physical,
    IN gctUINT32 Bytes,
    IN gceCACHEOPERATION Operation
    )
{
    return gcvSTATUS_OK;
}

static gceSTATUS
_CMAPhysical(
    IN gckALLOCATOR Allocator,
    IN PLINUX_MDL Mdl,
    IN gctUINT32 Offset,
    OUT gctPHYS_ADDR_T * Physical
    )
{
    struct mdl_cma_priv *mdl_priv=(struct mdl_cma_priv *)Mdl->priv;

    *Physical = mdl_priv->physical + Offset;

    return gcvSTATUS_OK;
}

static void
_CMAAllocatorDestructor(
    gcsALLOCATOR *Allocator
    )
{
    _CMAAllocatorDebugfsCleanup(Allocator);

    if (Allocator->privateData)
    {
        kfree(Allocator->privateData);
    }

    kfree(Allocator);
}

/* Default allocator operations. */
static gcsALLOCATOR_OPERATIONS CMAFSLAllocatorOperations =
{
    .Alloc              = _CMAFSLAlloc,
    .Free               = _CMAFSLFree,
    .MapUser            = _CMAFSLMapUser,
    .UnmapUser          = _CMAUnmapUser,
    .MapKernel          = _CMAMapKernel,
    .UnmapKernel        = _CMAUnmapKernel,
    .Cache              = _CMACache,
    .Physical           = _CMAPhysical,
};

/* Default allocator entry. */
gceSTATUS
_CMAFSLAlloctorInit(
    IN gckOS Os,
    IN gcsDEBUGFS_DIR *Parent,
    OUT gckALLOCATOR * Allocator
    )
{
    gceSTATUS status;
    gckALLOCATOR allocator = gcvNULL;
    gcsCMA_PRIV_PTR priv = gcvNULL;

    gcmkONERROR(
        gckALLOCATOR_Construct(Os, &CMAFSLAllocatorOperations, &allocator));

    priv = kzalloc(gcmSIZEOF(gcsCMA_PRIV), GFP_KERNEL | gcdNOWARN);

    if (!priv)
    {
        gcmkONERROR(gcvSTATUS_OUT_OF_MEMORY);
    }

    atomic_set(&priv->cmasize, 0);

    /* Register private data. */
    allocator->privateData = priv;
    allocator->destructor = _CMAAllocatorDestructor;

    _CMAAllocatorDebugfsInit(allocator, Parent);

    allocator->capability = gcvALLOC_FLAG_CONTIGUOUS;

#if defined(CONFIG_ARM64)
    Os->allocatorLimitMarker = (Os->device->baseAddress + totalram_pages * PAGE_SIZE) > 0x100000000;
#else
    Os->allocatorLimitMarker = gcvFALSE;
#endif
    priv->cmaLimitRequest = gcvFALSE;

    if (Os->allocatorLimitMarker)
    {
        allocator->capability |= gcvALLOC_FLAG_CMA_LIMIT;
    }

    *Allocator = allocator;

    return gcvSTATUS_OK;

OnError:
    if (allocator)
    {
        kfree(allocator);
    }
    return status;
}

