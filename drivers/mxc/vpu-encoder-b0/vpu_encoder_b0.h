/*
 * Copyright 2018 NXP
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

/*!
 * @file vpu_encoder_b0.h
 *
 * @brief VPU ENCODER B0 definition
 *
 */
#ifndef __VPU_ENCODER_B0_H__
#define __VPU_ENCODER_B0_H__

#include <linux/irqreturn.h>
#include <linux/mutex.h>
#include <linux/videodev2.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-fh.h>
#include <media/videobuf2-v4l2.h>
#include <soc/imx8/sc/svc/irq/api.h>
#include <soc/imx8/sc/ipc.h>
#include <soc/imx8/sc/sci.h>
#include <linux/mx8_mu.h>
#include <media/v4l2-event.h>
#include <linux/kfifo.h>
#include "vpu_encoder_rpc.h"
#include "vpu_encoder_config.h"

extern unsigned int vpu_dbg_level_encoder;

#define v4l2_fh_to_ctx(__fh) \
	container_of(__fh, struct vpu_ctx, fh)
#define v4l2_ctrl_to_ctx(__ctrl) \
	container_of((__ctrl)->handler, struct vpu_ctx, ctrl_handler)

#define MIN_SPACE 2048

#define VPU_MAX_FORMATS 4
#define VPU_MAX_BUFFER 32
#define M0FW_FILENAME "vpu/vpu_fw_imx8_enc.bin"
#define MMAP_BUF_TYPE_SHIFT 28
#define MMAP_BUF_TYPE_MASK 0xF0000000
#define M0_BOOT_SIZE_DEFAULT	0x1000000
#define M0_BOOT_SIZE_MIN	0x100000
#define RPC_SIZE_DEFAULT	0x100000
#define RPC_SIZE_MIN		0x20000
#define PRINT_SIZE_DEFAULT	0x200000
#define PRINT_SIZE_MIN		0x20000
#define MEM_SIZE  0x2800000
#define YUV_SIZE  0x4000000
#define STREAM_SIZE 0x300000
#define VPU_REG_BASE 0x40000000
#define ENC_REG_BASE 0x2c000000
#define MU_B0_REG_CONTROL		(0x10000 + 0x24)

#define MIN_BUFFER_COUNT		3
#define BITRATE_LOW_THRESHOLD		64
#define BITRATE_HIGH_THRESHOLD		1048576
#define BITRATE_DEFAULT_TARGET		2048
#define BITRATE_DEFAULT_PEAK		4096
#define GOP_H_THRESHOLD			300
#define GOP_L_THRESHOLD			1
#define GOP_DEFAULT			30
#define QP_MAX				51
#define QP_MIN				0
#define QP_DEFAULT			25

#define ENCODER_NODE_NUMBER 13 //use /dev/video13 as encoder node
struct vpu_v4l2_control {
	uint32_t id;
	enum v4l2_ctrl_type type;
	uint32_t minimum;
	uint32_t maximum;
	uint32_t step;
	uint32_t default_value;
	uint32_t menu_skip_mask;
	bool is_volatile;
};

typedef enum{
	INIT_DONE = 1,
	RPC_BUF_OFFSET,
	BOOT_ADDRESS,
	COMMAND,
	EVENT
} MSG_Type;

enum PLAT_TYPE {
	IMX8QXP = 0,
	IMX8QM  = 1,
};

enum QUEUE_TYPE {
	V4L2_SRC = 0,
	V4L2_DST = 1,
};

enum vpu_video_standard {
	VPU_VIDEO_UNDEFINED = 0,
	VPU_VIDEO_AVC = 1,
	VPU_VIDEO_VC1 = 2,
	VPU_VIDEO_MPEG2 = 3,
	VPU_VIDEO_AVS = 4,
	VPU_VIDEO_ASP = 5,
	VPU_VIDEO_JPEG = 6,
	VPU_VIDEO_RV8 = 7,
	VPU_VIDEO_RV9 = 8,
	VPU_VIDEO_VP6 = 9,
	VPU_VIDEO_SPK = 10,
	VPU_VIDEO_VP8 = 11,
	VPU_VIDEO_AVC_MVC = 12,
	VPU_VIDEO_HEVC = 13,
	VPU_VIDEO_VP9 = 14,
};

#define VPU_PIX_FMT_AVS         v4l2_fourcc('A', 'V', 'S', '0')
#define VPU_PIX_FMT_ASP         v4l2_fourcc('A', 'S', 'P', '0')
#define VPU_PIX_FMT_RV8         v4l2_fourcc('R', 'V', '8', '0')
#define VPU_PIX_FMT_RV9         v4l2_fourcc('R', 'V', '9', '0')
#define VPU_PIX_FMT_VP6         v4l2_fourcc('V', 'P', '6', '0')
#define VPU_PIX_FMT_SPK         v4l2_fourcc('S', 'P', 'K', '0')
#define VPU_PIX_FMT_HEVC        v4l2_fourcc('H', 'E', 'V', 'C')
#define VPU_PIX_FMT_VP9         v4l2_fourcc('V', 'P', '9', '0')
#define VPU_PIX_FMT_LOGO        v4l2_fourcc('L', 'O', 'G', 'O')

#define VPU_PIX_FMT_TILED_8     v4l2_fourcc('Z', 'T', '0', '8')
#define VPU_PIX_FMT_TILED_10    v4l2_fourcc('Z', 'T', '1', '0')

enum vpu_pixel_format {
	VPU_HAS_COLOCATED = 0x00000001,
	VPU_HAS_SPLIT_FLD = 0x00000002,
	VPU_PF_MASK       = ~(VPU_HAS_COLOCATED | VPU_HAS_SPLIT_FLD),

	VPU_IS_TILED      = 0x000000100,
	VPU_HAS_10BPP     = 0x00000200,

	VPU_IS_PLANAR     = 0x00001000,
	VPU_IS_SEMIPLANAR = 0x00002000,
	VPU_IS_PACKED     = 0x00004000,

	// Merged definitions using above flags:
	VPU_PF_UNDEFINED  = 0,
	VPU_PF_YUV420_SEMIPLANAR = 0x00010000 | VPU_IS_SEMIPLANAR,
	VPU_PF_YUV420_PLANAR = 0x00020000 | VPU_IS_PLANAR,
	VPU_PF_UYVY = 0x00040000 | VPU_IS_PACKED,
	VPU_PF_TILED_8BPP = 0x00080000 | VPU_IS_TILED | VPU_IS_SEMIPLANAR,
	VPU_PF_TILED_10BPP = 0x00100000 | VPU_IS_TILED | VPU_IS_SEMIPLANAR | VPU_HAS_10BPP,
};

struct vpu_ctx;
struct core_device;
struct vpu_dev;
struct vpu_v4l2_fmt {
	char *name;
	unsigned int fourcc;
	unsigned int num_planes;
	unsigned int venc_std;
	unsigned int is_yuv;
};

struct vb2_data_req {
	struct list_head  list;
	struct vb2_buffer *vb2_buf;
	int id;
	u_int32 buffer_flags;
};

enum ENC_RW_FLAG {
	VPU_ENC_FLAG_WRITEABLE,
	VPU_ENC_FLAG_READABLE
};

struct queue_data {
	unsigned int width;
	unsigned int height;
	unsigned int bytesperline;
	unsigned int sizeimage[3];
	unsigned int fourcc;
	unsigned int vdec_std;
	struct v4l2_rect rect;
	int buf_type; // v4l2_buf_type
	bool vb2_q_inited;
	struct vb2_queue vb2_q;    // vb2 queue
	struct list_head drv_q;    // driver queue
	struct semaphore drv_q_lock;
	struct vb2_data_req vb2_reqs[VPU_MAX_BUFFER];
	enum QUEUE_TYPE type;
	struct vpu_v4l2_fmt *supported_fmts;
	unsigned int fmt_count;
	struct vpu_v4l2_fmt *current_fmt;
	unsigned long rw_flag;
	struct list_head frame_q;
	struct vpu_ctx *ctx;
};

struct vpu_strip_info {
	unsigned long count;
	unsigned long max;
	unsigned long total;
};

struct vpu_fps_sts {
	unsigned int thd;
	unsigned int times;
	unsigned long frame_number;
	struct timespec ts;
	unsigned long fps;
};

struct vpu_statistic {
	unsigned long cmd[GTB_ENC_CMD_RESERVED + 1];
	unsigned long event[VID_API_ENC_EVENT_RESERVED + 1];
	unsigned long current_cmd;
	unsigned long current_event;
	struct timespec ts_cmd;
	struct timespec ts_event;
	unsigned long yuv_count;
	unsigned long encoded_count;
	unsigned long h264_count;
	struct {
		struct vpu_strip_info begin;
		struct vpu_strip_info end;
		struct vpu_strip_info eos;
	} strip_sts;
	bool fps_sts_enable;
	struct vpu_fps_sts fps[VPU_FPS_STS_CNT];
};

struct vpu_attr {
	struct device_attribute dev_attr;
	char name[64];
	u32 index;
	struct core_device *core;

	pid_t pid;
	pid_t tgid;

	struct vpu_statistic statistic;
	MEDIAIP_ENC_PARAM param;

	unsigned long ts_start[2];
	unsigned long msg_count;
	atomic64_t total_dma_size;

	bool created;
};

struct core_device {
	void *m0_p_fw_space_vir;
	u_int32 m0_p_fw_space_phy;
	u32 fw_buf_size;
	u32 fw_actual_size;
	void *m0_rpc_virt;
	u_int32 m0_rpc_phy;
	u32 rpc_buf_size;
	u32 print_buf_size;
	u32 rpc_actual_size;

	struct mutex cmd_mutex;
	bool fw_is_ready;
	bool firmware_started;
	struct completion start_cmp;
	struct completion snap_done_cmp;
	struct workqueue_struct *workqueue;
	struct work_struct msg_work;
	void __iomem *mu_base_virtaddr;
	unsigned int vpu_mu_id;
	int vpu_mu_init;

	u32 supported_instance_count;
	struct vpu_ctx *ctx[VID_API_NUM_STREAMS];
	struct vpu_attr attr[VID_API_NUM_STREAMS];
	struct shared_addr shared_mem;
	u32 id;
	off_t reg_fw_base;
	struct device *generic_dev;
	struct vpu_dev *vdev;
	bool snapshot;
	bool suspend;
	bool hang;
	struct device_attribute core_attr;
	char name[64];
	unsigned long reset_times;
};

struct vpu_dev {
	struct device *generic_dev;
	struct v4l2_device v4l2_dev;
	struct video_device *pvpu_encoder_dev;
	struct platform_device *plat_dev;
	struct clk *clk_m0;
	void __iomem *regs_base;
	void __iomem *regs_enc;
	struct mutex dev_mutex;
	struct core_device core_dev[2];
	u_int32 plat_type;
	u_int32 core_num;
	bool hw_enable;

	struct delayed_work watchdog;
	u8 heartbeat;
};

struct buffer_addr {
	void *virt_addr;
	dma_addr_t phy_addr;
	u_int32 size;
};

enum {
	VPU_ENC_STATUS_INITIALIZED,
	VPU_ENC_STATUS_SNAPSHOT = 20,
	VPU_ENC_STATUS_FORCE_RELEASE = 21,
	VPU_ENC_STATUS_EOS_SEND = 22,
	VPU_ENC_STATUS_START_SEND = 23,
	VPU_ENC_STATUS_START_DONE = 24,
	VPU_ENC_STATUS_STOP_REQ = 25,
	VPU_ENC_STATUS_STOP_SEND = 26,
	VPU_ENC_STATUS_STOP_DONE = 27,
	VPU_ENC_STATUS_CLOSED = 28,
	VPU_ENC_STATUS_CONFIGURED = 29,
	VPU_ENC_STATUS_HANG = 30,
	VPU_ENC_STATUS_KEY_FRAME = 31
};

struct vpu_ctx {
	struct vpu_dev *dev;
	struct v4l2_fh fh;

	struct v4l2_ctrl_handler ctrl_handler;
	bool ctrl_inited;

	int str_index;
	unsigned long status;
	struct queue_data q_data[2];
	struct mutex instance_mutex;
	struct work_struct instance_work;
	struct workqueue_struct *instance_wq;
	bool ctx_released;
	struct buffer_addr encoder_stream;
	struct buffer_addr encFrame[MEDIAIP_MAX_NUM_WINDSOR_SRC_FRAMES];
	struct buffer_addr refFrame[MEDIAIP_MAX_NUM_WINDSOR_REF_FRAMES];
	struct buffer_addr actFrame;
	struct buffer_addr enc_buffer;
	struct core_device *core_dev;

	struct completion stop_cmp;
	bool power_status;

	struct list_head msg_q;
	struct list_head idle_q;

	struct vpu_statistic sts;
	unsigned int frozen_count;
};

#define LVL_DEBUG	4
#define LVL_INFO	3
#define LVL_IRQ		2
#define LVL_ALL		1
#define LVL_WARN	1
#define LVL_ERR		0

#ifndef TAG
#define TAG	"[VPU Encoder]\t "
#endif

#define vpu_dbg(level, fmt, arg...) \
	do { \
		if (vpu_dbg_level_encoder >= (level)) \
			pr_info(TAG""fmt, ## arg); \
	} while (0)

#define vpu_err(fmt, arg...)	vpu_dbg(LVL_ERR, fmt, ##arg)

struct vpu_attr *get_vpu_ctx_attr(struct vpu_ctx *ctx);
struct vpu_ctx *get_vpu_attr_ctx(struct vpu_attr *attr);

#endif
