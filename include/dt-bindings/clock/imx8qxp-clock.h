/*
 * Copyright (C) 2016 Freescale Semiconductor, Inc.
 * Copyright 2017 NXP
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __DT_BINDINGS_CLOCK_IMX8QXP_H
#define __DT_BINDINGS_CLOCK_IMX8QXP_H

#define IMX8QXP_CLK_DUMMY					0

#define IMX8QXP_UART0_IPG_CLK					1
#define IMX8QXP_UART0_DIV					2
#define IMX8QXP_UART0_CLK					3

#define IMX8QXP_IPG_DMA_CLK_ROOT				4

/* GPU Clocks. */
#define IMX8QXP_GPU0_CORE_DIV					5
#define IMX8QXP_GPU0_CORE_CLK					6
#define IMX8QXP_GPU0_SHADER_DIV					7
#define IMX8QXP_GPU0_SHADER_CLK					8

#define IMX8QXP_24MHZ						9
#define IMX8QXP_GPT_3M						10
#define IMX8QXP_32KHZ						11

/* LSIO SS */
#define IMX8QXP_LSIO_MEM_CLK					12
#define IMX8QXP_LSIO_BUS_CLK					13
#define IMX8QXP_LSIO_PWM0_DIV					14
#define IMX8QXP_LSIO_PWM0_IPG_S_CLK				15
#define IMX8QXP_LSIO_PWM0_IPG_SLV_CLK				16
#define IMX8QXP_LSIO_PWM0_IPG_MSTR_CLK				17
#define IMX8QXP_LSIO_PWM0_HF_CLK				18
#define IMX8QXP_LSIO_PWM0_CLK					19
#define IMX8QXP_LSIO_PWM1_DIV					20
#define IMX8QXP_LSIO_PWM1_IPG_S_CLK				21
#define IMX8QXP_LSIO_PWM1_IPG_SLV_CLK				22
#define IMX8QXP_LSIO_PWM1_IPG_MSTR_CLK				23
#define IMX8QXP_LSIO_PWM1_HF_CLK				24
#define IMX8QXP_LSIO_PWM1_CLK					25
#define IMX8QXP_LSIO_PWM2_DIV					26
#define IMX8QXP_LSIO_PWM2_IPG_S_CLK				27
#define IMX8QXP_LSIO_PWM2_IPG_SLV_CLK				28
#define IMX8QXP_LSIO_PWM2_IPG_MSTR_CLK				29
#define IMX8QXP_LSIO_PWM2_HF_CLK				30
#define IMX8QXP_LSIO_PWM2_CLK					31
#define IMX8QXP_LSIO_PWM3_DIV					32
#define IMX8QXP_LSIO_PWM3_IPG_S_CLK				33
#define IMX8QXP_LSIO_PWM3_IPG_SLV_CLK				34
#define IMX8QXP_LSIO_PWM3_IPG_MSTR_CLK				35
#define IMX8QXP_LSIO_PWM3_HF_CLK				36
#define IMX8QXP_LSIO_PWM3_CLK					37
#define IMX8QXP_LSIO_PWM4_DIV					38
#define IMX8QXP_LSIO_PWM4_IPG_S_CLK				39
#define IMX8QXP_LSIO_PWM4_IPG_SLV_CLK				40
#define IMX8QXP_LSIO_PWM4_IPG_MSTR_CLK				42
#define IMX8QXP_LSIO_PWM4_HF_CLK				43
#define IMX8QXP_LSIO_PWM4_CLK					44
#define IMX8QXP_LSIO_PWM5_DIV					45
#define IMX8QXP_LSIO_PWM5_IPG_S_CLK				46
#define IMX8QXP_LSIO_PWM5_IPG_SLV_CLK				47
#define IMX8QXP_LSIO_PWM5_IPG_MSTR_CLK				48
#define IMX8QXP_LSIO_PWM5_HF_CLK				49
#define IMX8QXP_LSIO_PWM5_CLK					50
#define IMX8QXP_LSIO_PWM6_DIV					51
#define IMX8QXP_LSIO_PWM6_IPG_S_CLK				52
#define IMX8QXP_LSIO_PWM6_IPG_SLV_CLK				53
#define IMX8QXP_LSIO_PWM6_IPG_MSTR_CLK				54
#define IMX8QXP_LSIO_PWM6_HF_CLK				55
#define IMX8QXP_LSIO_PWM6_CLK					56
#define IMX8QXP_LSIO_PWM7_DIV					57
#define IMX8QXP_LSIO_PWM7_IPG_S_CLK				58
#define IMX8QXP_LSIO_PWM7_IPG_SLV_CLK				59
#define IMX8QXP_LSIO_PWM7_IPG_MSTR_CLK				60
#define IMX8QXP_LSIO_PWM7_HF_CLK				61
#define IMX8QXP_LSIO_PWM7_CLK					62
#define IMX8QXP_LSIO_GPT0_DIV					63
#define IMX8QXP_LSIO_GPT0_IPG_S_CLK				64
#define IMX8QXP_LSIO_GPT0_IPG_SLV_CLK				65
#define IMX8QXP_LSIO_GPT0_IPG_MSTR_CLK				66
#define IMX8QXP_LSIO_GPT0_HF_CLK				67
#define IMX8QXP_LSIO_GPT0_CLK					68
#define IMX8QXP_LSIO_GPT1_DIV					69
#define IMX8QXP_LSIO_GPT1_IPG_S_CLK				70
#define IMX8QXP_LSIO_GPT1_IPG_SLV_CLK				71
#define IMX8QXP_LSIO_GPT1_IPG_MSTR_CLK				72
#define IMX8QXP_LSIO_GPT1_HF_CLK				73
#define IMX8QXP_LSIO_GPT1_CLK					74
#define IMX8QXP_LSIO_GPT2_DIV					75
#define IMX8QXP_LSIO_GPT2_IPG_S_CLK				76
#define IMX8QXP_LSIO_GPT2_IPG_SLV_CLK				77
#define IMX8QXP_LSIO_GPT2_IPG_MSTR_CLK				78
#define IMX8QXP_LSIO_GPT2_HF_CLK				79
#define IMX8QXP_LSIO_GPT2_CLK					80
#define IMX8QXP_LSIO_GPT3_DIV					81
#define IMX8QXP_LSIO_GPT3_IPG_S_CLK				82
#define IMX8QXP_LSIO_GPT3_IPG_SLV_CLK				83
#define IMX8QXP_LSIO_GPT3_IPG_MSTR_CLK				84
#define IMX8QXP_LSIO_GPT3_HF_CLK				85
#define IMX8QXP_LSIO_GPT3_CLK					86
#define IMX8QXP_LSIO_GPT4_DIV					87
#define IMX8QXP_LSIO_GPT4_IPG_S_CLK				88
#define IMX8QXP_LSIO_GPT4_IPG_SLV_CLK				89
#define IMX8QXP_LSIO_GPT4_IPG_MSTR_CLK				90
#define IMX8QXP_LSIO_GPT4_HF_CLK				91
#define IMX8QXP_LSIO_GPT4_CLK					92
#define IMX8QXP_LSIO_FSPI0_DIV					93
#define IMX8QXP_LSIO_FSPI0_HCLK					94
#define IMX8QXP_LSIO_FSPI0_IPG_S_CLK				95
#define IMX8QXP_LSIO_FSPI0_IPG_CLK				96
#define IMX8QXP_LSIO_FSPI0_CLK					97
#define IMX8QXP_LSIO_FSPI1_DIV					98
#define IMX8QXP_LSIO_FSPI1_HCLK					99
#define IMX8QXP_LSIO_FSPI1_IPG_S_CLK				100
#define IMX8QXP_LSIO_FSPI1_IPG_CLK				101
#define IMX8QXP_LSIO_FSPI1_CLK					102
#define IMX8QXP_LSIO_GPIO0_IPG_S_CLK				103
#define IMX8QXP_LSIO_GPIO1_IPG_S_CLK				104
#define IMX8QXP_LSIO_GPIO2_IPG_S_CLK				105
#define IMX8QXP_LSIO_GPIO3_IPG_S_CLK				106
#define IMX8QXP_LSIO_GPIO4_IPG_S_CLK				107
#define IMX8QXP_LSIO_GPIO5_IPG_S_CLK				108
#define IMX8QXP_LSIO_GPIO6_IPG_S_CLK				109
#define IMX8QXP_LSIO_GPIO7_IPG_S_CLK				110
#define IMX8QXP_LSIO_ROMCP_REG_CLK				111
#define IMX8QXP_LSIO_ROMCP_CLK					112
#define IMX8QXP_LSIO_96KROM_CLK					113
#define IMX8QXP_LSIO_OCRAM_MEM_CLK				114
#define IMX8QXP_LSIO_OCRAM_CTRL_CLK				115

/* ADMA SS */
#define IMX8QXP_UART1_IPG_CLK					116
#define IMX8QXP_UART2_IPG_CLK					117
#define IMX8QXP_UART3_IPG_CLK					118
#define IMX8QXP_UART1_DIV					119
#define IMX8QXP_UART2_DIV					120
#define IMX8QXP_UART3_DIV					121
#define IMX8QXP_UART1_CLK					122
#define IMX8QXP_UART2_CLK					123
#define IMX8QXP_UART3_CLK					124
#define IMX8QXP_SPI0_IPG_CLK					125
#define IMX8QXP_SPI1_IPG_CLK					126
#define IMX8QXP_SPI2_IPG_CLK					127
#define IMX8QXP_SPI3_IPG_CLK					128
#define IMX8QXP_SPI0_DIV					129
#define IMX8QXP_SPI1_DIV					130
#define IMX8QXP_SPI2_DIV					131
#define IMX8QXP_SPI3_DIV					132
#define IMX8QXP_SPI0_CLK					133
#define IMX8QXP_SPI1_CLK					134
#define IMX8QXP_SPI2_CLK					135
#define IMX8QXP_SPI3_CLK					136
#define IMX8QXP_CAN0_IPG_CHI_CLK				137
#define IMX8QXP_CAN1_IPG_CHI_CLK				138
#define IMX8QXP_CAN2_IPG_CHI_CLK				139
#define IMX8QXP_CAN0_IPG_CLK					140
#define IMX8QXP_CAN1_IPG_CLK					141
#define IMX8QXP_CAN2_IPG_CLK					142
#define IMX8QXP_CAN0_DIV					143
#define IMX8QXP_CAN1_DIV					144
#define IMX8QXP_CAN2_DIV					145
#define IMX8QXP_CAN0_CLK					146
#define IMX8QXP_CAN1_CLK					147
#define IMX8QXP_CAN2_CLK					148
#define IMX8QXP_I2C0_IPG_CLK					149
#define IMX8QXP_I2C1_IPG_CLK					150
#define IMX8QXP_I2C2_IPG_CLK					151
#define IMX8QXP_I2C3_IPG_CLK					152
#define IMX8QXP_I2C0_DIV					153
#define IMX8QXP_I2C1_DIV					154
#define IMX8QXP_I2C2_DIV					155
#define IMX8QXP_I2C3_DIV					156
#define IMX8QXP_I2C0_CLK					157
#define IMX8QXP_I2C1_CLK					158
#define IMX8QXP_I2C2_CLK					159
#define IMX8QXP_I2C3_CLK					160
#define IMX8QXP_FTM0_IPG_CLK					161
#define IMX8QXP_FTM1_IPG_CLK					162
#define IMX8QXP_FTM0_DIV					163
#define IMX8QXP_FTM1_DIV					164
#define IMX8QXP_FTM0_CLK					165
#define IMX8QXP_FTM1_CLK					166
#define IMX8QXP_ADC0_IPG_CLK					167
#define IMX8QXP_ADC0_DIV					168
#define IMX8QXP_ADC0_CLK					169
#define IMX8QXP_PWM_IPG_CLK					170
#define IMX8QXP_PWM_DIV						171
#define IMX8QXP_PWM_CLK						172
#define IMX8QXP_LCD_IPG_CLK					173
#define IMX8QXP_LCD_DIV						174
#define IMX8QXP_LCD_CLK						175

/* Connectivity SS */
#define IMX8QXP_AXI_CONN_CLK_ROOT				176
#define IMX8QXP_AHB_CONN_CLK_ROOT				177
#define IMX8QXP_IPG_CONN_CLK_ROOT				178
#define IMX8QXP_SDHC0_IPG_CLK					179
#define IMX8QXP_SDHC1_IPG_CLK					180
#define IMX8QXP_SDHC2_IPG_CLK					181
#define IMX8QXP_SDHC0_DIV					182
#define IMX8QXP_SDHC1_DIV					183
#define IMX8QXP_SDHC2_DIV					184
#define IMX8QXP_SDHC0_CLK					185
#define IMX8QXP_SDHC1_CLK					186
#define IMX8QXP_SDHC2_CLK					187
#define IMX8QXP_ENET0_ROOT_DIV					188
#define IMX8QXP_ENET0_REF_DIV					189
#define IMX8QXP_ENET1_REF_DIV					190
#define IMX8QXP_ENET0_BYPASS_DIV				191
#define IMX8QXP_ENET0_RGMII_DIV					192
#define IMX8QXP_ENET1_ROOT_DIV					193
#define IMX8QXP_ENET1_BYPASS_DIV				194
#define IMX8QXP_ENET1_RGMII_DIV					195
#define IMX8QXP_ENET0_AHB_CLK					196
#define IMX8QXP_ENET0_IPG_S_CLK					197
#define IMX8QXP_ENET0_IPG_CLK					198
#define IMX8QXP_ENET1_AHB_CLK					199
#define IMX8QXP_ENET1_IPG_S_CLK					200
#define IMX8QXP_ENET1_IPG_CLK					201
#define IMX8QXP_ENET0_ROOT_CLK					202
#define IMX8QXP_ENET1_ROOT_CLK					203
#define IMX8QXP_ENET0_TX_CLK					204
#define IMX8QXP_ENET1_TX_CLK					205
#define IMX8QXP_ENET0_PTP_CLK					206
#define IMX8QXP_ENET1_PTP_CLK					207
#define IMX8QXP_ENET0_REF_25MHZ_125MHZ_SEL			208
#define IMX8QXP_ENET1_REF_25MHZ_125MHZ_SEL			209
#define IMX8QXP_ENET0_RMII_TX_SEL				210
#define IMX8QXP_ENET1_RMII_TX_SEL				211
#define IMX8QXP_ENET0_RGMII_TX_CLK				212
#define IMX8QXP_ENET1_RGMII_TX_CLK				213
#define IMX8QXP_ENET0_RMII_RX_CLK				214
#define IMX8QXP_ENET1_RMII_RX_CLK				215
#define IMX8QXP_ENET0_REF_25MHZ_125MHZ_CLK			216
#define IMX8QXP_ENET1_REF_25MHZ_125MHZ_CLK			217
#define IMX8QXP_ENET0_REF_50MHZ_CLK				218
#define IMX8QXP_ENET1_REF_50MHZ_CLK				219
#define IMX8QXP_GPMI_BCH_IO_DIV					220
#define IMX8QXP_GPMI_BCH_DIV					221
#define IMX8QXP_GPMI_APB_CLK					222
#define IMX8QXP_GPMI_APB_BCH_CLK				223
#define IMX8QXP_GPMI_BCH_IO_CLK					224
#define IMX8QXP_GPMI_BCH_CLK					225
#define IMX8QXP_APBHDMA_CLK					226
#define IMX8QXP_USB3_ACLK_DIV					227
#define IMX8QXP_USB3_BUS_DIV					228
#define IMX8QXP_USB3_LPM_DIV					229
#define IMX8QXP_USB3_IPG_CLK					230
#define IMX8QXP_USB3_CORE_PCLK					231
#define IMX8QXP_USB3_PHY_CLK					232
#define IMX8QXP_USB3_ACLK					233
#define IMX8QXP_USB3_BUS_CLK					234
#define IMX8QXP_USB3_LPM_CLK					235
#define IMX8QXP_USB2_OH_AHB_CLK					236
#define IMX8QXP_USB2_OH_IPG_S_CLK				237
#define IMX8QXP_USB2_OH_IPG_S_PL301_CLK				238
#define IMX8QXP_USB2_PHY_IPG_CLK				239
#define IMX8QXP_EDMA_CLK					240
#define IMX8QXP_EDMA_IPG_CLK					241
#define IMX8QXP_MLB_HCLK					242
#define IMX8QXP_MLB_CLK						243
#define IMX8QXP_MLB_IPG_CLK					244

/* Display controller SS */
/* DC part1 */
#define IMX8QXP_DC_AXI_EXT_CLK					245
#define IMX8QXP_DC_AXI_INT_CLK					246
#define IMX8QXP_DC_CFG_CLK					247
#define IMX8QXP_DC0_DISP0_CLK					248
#define IMX8QXP_DC0_DISP1_CLK					249
#define IMX8QXP_DC0_PRG0_RTRAM_CLK				250
#define IMX8QXP_DC0_PRG0_APB_CLK				251
#define IMX8QXP_DC0_PRG1_RTRAM_CLK				252
#define IMX8QXP_DC0_PRG1_APB_CLK				253
#define IMX8QXP_DC0_PRG2_RTRAM_CLK				254
#define IMX8QXP_DC0_PRG2_APB_CLK				255
#define IMX8QXP_DC0_PRG3_RTRAM_CLK				256
#define IMX8QXP_DC0_PRG3_APB_CLK				257
#define IMX8QXP_DC0_PRG4_RTRAM_CLK				258
#define IMX8QXP_DC0_PRG4_APB_CLK				259
#define IMX8QXP_DC0_PRG5_RTRAM_CLK				260
#define IMX8QXP_DC0_PRG5_APB_CLK				261
#define IMX8QXP_DC0_PRG6_RTRAM_CLK				262
#define IMX8QXP_DC0_PRG6_APB_CLK				263
#define IMX8QXP_DC0_PRG7_RTRAM_CLK				264
#define IMX8QXP_DC0_PRG7_APB_CLK				265
#define IMX8QXP_DC0_PRG8_RTRAM_CLK				266
#define IMX8QXP_DC0_PRG8_APB_CLK				267
#define IMX8QXP_DC0_DPR0_APB_CLK				268
#define IMX8QXP_DC0_DPR0_B_CLK					269
#define IMX8QXP_DC0_RTRAM0_CLK					270
#define IMX8QXP_DC0_RTRAM1_CLK					271

/* MIPI-LVDS part1 */
#define IMX8QXP_MIPI_IPG_CLK					272
#define IMX8QXP_MIPI0_I2C0_DIV					273
#define IMX8QXP_MIPI0_I2C1_DIV					274
#define IMX8QXP_MIPI0_I2C0_CLK					275
#define IMX8QXP_MIPI0_I2C1_CLK					276
#define IMX8QXP_MIPI0_I2C0_IPG_S_CLK				277
#define IMX8QXP_MIPI0_I2C0_IPG_CLK				278
#define IMX8QXP_MIPI0_I2C1_IPG_S_CLK				279
#define IMX8QXP_MIPI0_I2C1_IPG_CLK				280
#define IMX8QXP_MIPI0_PWM_IPG_S_CLK				281
#define IMX8QXP_MIPI0_PWM_IPG_CLK				282
#define IMX8QXP_MIPI0_PWM_32K_CLK				283
#define IMX8QXP_MIPI0_GPIO_IPG_CLK				284

#define IMX8QXP_IMG_JPEG_ENC_IPG_CLK				285
#define IMX8QXP_IMG_JPEG_ENC_CLK				286
#define IMX8QXP_IMG_JPEG_DEC_IPG_CLK				287
#define IMX8QXP_IMG_JPEG_DEC_CLK				288
#define IMX8QXP_IMG_PXL_LINK_DC0_CLK				289
#define IMX8QXP_IMG_PXL_LINK_DC1_CLK				290
#define IMX8QXP_IMG_PXL_LINK_CSI0_CLK				291
#define IMX8QXP_IMG_PXL_LINK_CSI1_CLK				292
#define IMX8QXP_IMG_PXL_LINK_HDMI_IN_CLK			293
#define IMX8QXP_IMG_PDMA_0_CLK					294
#define IMX8QXP_IMG_PDMA_1_CLK					295
#define IMX8QXP_IMG_PDMA_2_CLK					296
#define IMX8QXP_IMG_PDMA_3_CLK					297
#define IMX8QXP_IMG_PDMA_4_CLK					298
#define IMX8QXP_IMG_PDMA_5_CLK					299
#define IMX8QXP_IMG_PDMA_6_CLK					300
#define IMX8QXP_IMG_PDMA_7_CLK					301
#define IMX8QXP_IMG_AXI_CLK					302
#define IMX8QXP_IMG_IPG_CLK					303
#define IMX8QXP_IMG_PXL_CLK					304

#define IMX8QXP_CSI0_I2C0_DIV					305
#define IMX8QXP_CSI0_PWM0_DIV					306
#define IMX8QXP_CSI0_CORE_DIV					307
#define IMX8QXP_CSI0_ESC_DIV					308
#define IMX8QXP_CSI0_IPG_CLK_S					309
#define IMX8QXP_CSI0_IPG_CLK					310
#define IMX8QXP_CSI0_APB_CLK					311
#define IMX8QXP_CSI0_I2C0_IPG_CLK				312
#define IMX8QXP_CSI0_I2C0_CLK					313
#define IMX8QXP_CSI0_PWM0_IPG_CLK				314
#define IMX8QXP_CSI0_PWM0_CLK					315
#define IMX8QXP_CSI0_CORE_CLK					316
#define IMX8QXP_CSI0_ESC_CLK					317

#define IMX8QXP_HSIO_AXI_CLK					318
#define IMX8QXP_HSIO_PER_CLK					319
#define IMX8QXP_HSIO_PCIE_MSTR_AXI_CLK				320
#define IMX8QXP_HSIO_PCIE_SLV_AXI_CLK				321
#define IMX8QXP_HSIO_PCIE_DBI_AXI_CLK				322
#define IMX8QXP_HSIO_PCIE_X1_PER_CLK				323
#define IMX8QXP_HSIO_PHY_X1_PER_CLK				324
#define IMX8QXP_HSIO_MISC_PER_CLK				325
#define IMX8QXP_HSIO_PHY_X1_APB_CLK				326
#define IMX8QXP_HSIO_GPIO_CLK					327
#define IMX8QXP_HSIO_PHY_X1_PCLK				328

#define IMX8QXP_A35_DIV						329

/* ACM */
#define IMX8QXP_EXT_AUD_MCLK0					330
#define IMX8QXP_EXT_AUD_MCLK1					331
#define IMX8QXP_ESAI0_RX_CLK					332
#define IMX8QXP_ESAI0_RX_HF_CLK					333
#define IMX8QXP_ESAI0_TX_CLK					334
#define IMX8QXP_ESAI0_TX_HF_CLK					335
#define IMX8QXP_SPDIF0_RX					336
#define IMX8QXP_SAI0_RX_BCLK					337
#define IMX8QXP_SAI0_TX_BCLK					338
#define IMX8QXP_SAI1_RX_BCLK					339
#define IMX8QXP_SAI1_TX_BCLK					340
#define IMX8QXP_SAI2_RX_BCLK					341
#define IMX8QXP_SAI3_RX_BCLK					342
#define IMX8QXP_SAI4_RX_BCLK					343

#define IMX8QXP_ACM_AUD_CLK0_SEL				344
#define IMX8QXP_ACM_AUD_CLK0_CLK				345
#define IMX8QXP_ACM_AUD_CLK1_SEL				346
#define IMX8QXP_ACM_AUD_CLK1_CLK				347
#define IMX8QXP_ACM_MCLKOUT0_SEL				348
#define IMX8QXP_ACM_MCLKOUT0_CLK				349
#define IMX8QXP_ACM_MCLKOUT1_SEL				350
#define IMX8QXP_ACM_MCLKOUT1_CLK				351
#define IMX8QXP_ACM_ESAI0_MCLK_SEL				352
#define IMX8QXP_ACM_ESAI0_MCLK_CLK				353
#define IMX8QXP_ACM_GPT0_MUX_CLK_SEL				354
#define IMX8QXP_ACM_GPT0_MUX_CLK_CLK				355
#define IMX8QXP_ACM_GPT1_MUX_CLK_SEL				356
#define IMX8QXP_ACM_GPT1_MUX_CLK_CLK				357
#define IMX8QXP_ACM_GPT2_MUX_CLK_SEL				358
#define IMX8QXP_ACM_GPT2_MUX_CLK_CLK				359
#define IMX8QXP_ACM_GPT3_MUX_CLK_SEL				360
#define IMX8QXP_ACM_GPT3_MUX_CLK_CLK				361
#define IMX8QXP_ACM_GPT4_MUX_CLK_SEL				362
#define IMX8QXP_ACM_GPT4_MUX_CLK_CLK				363
#define IMX8QXP_ACM_GPT5_MUX_CLK_SEL				364
#define IMX8QXP_ACM_GPT5_MUX_CLK_CLK				365
#define IMX8QXP_ACM_SAI0_MCLK_SEL				366
#define IMX8QXP_ACM_SAI0_MCLK_CLK				367
#define IMX8QXP_ACM_SAI1_MCLK_SEL				368
#define IMX8QXP_ACM_SAI1_MCLK_CLK				369
#define IMX8QXP_ACM_SAI2_MCLK_SEL				370
#define IMX8QXP_ACM_SAI2_MCLK_CLK				371
#define IMX8QXP_ACM_SAI3_MCLK_SEL				372
#define IMX8QXP_ACM_SAI3_MCLK_CLK				373
#define IMX8QXP_ACM_SAI4_MCLK_SEL				374
#define IMX8QXP_ACM_SAI4_MCLK_CLK				375
#define IMX8QXP_ACM_SAI5_MCLK_SEL				376
#define IMX8QXP_ACM_SAI5_MCLK_CLK				377
#define IMX8QXP_ACM_SPDIF0_TX_CLK_SEL				378
#define IMX8QXP_ACM_SPDIF0_TX_CLK_CLK				379
#define IMX8QXP_ACM_MQS_TX_CLK_SEL				380
#define IMX8QXP_ACM_MQS_TX_CLK_CLK				381
#define IMX8QXP_ACM_ASRC0_MUX_CLK_SEL				382
#define IMX8QXP_ACM_ASRC1_MUX_CLK_SEL				383
#define IMX8QXP_ACM_ASRC0_MUX_CLK_CLK				384
#define IMX8QXP_ACM_ASRC1_MUX_CLK_CLK				385

#define IMX8QXP_IPG_AUD_CLK_ROOT				386

/* Audio */
#define IMX8QXP_AUD_PLL0_DIV					387
#define IMX8QXP_AUD_PLL0					388
#define IMX8QXP_AUD_PLL1_DIV					389
#define IMX8QXP_AUD_PLL1					390
#define IMX8QXP_AUD_AMIX_IPG					391
#define IMX8QXP_AUD_ESAI_0_IPG					392
#define IMX8QXP_AUD_ESAI_0_EXTAL_IPG				393
#define IMX8QXP_AUD_SAI_0_IPG					394
#define IMX8QXP_AUD_SAI_0_MCLK					395
#define IMX8QXP_AUD_SAI_1_IPG					396
#define IMX8QXP_AUD_SAI_1_MCLK					397
#define IMX8QXP_AUD_SAI_2_IPG					398
#define IMX8QXP_AUD_SAI_2_MCLK					399
#define IMX8QXP_AUD_SAI_3_IPG					400
#define IMX8QXP_AUD_SAI_3_MCLK					401
#define IMX8QXP_AUD_SAI_4_IPG					402
#define IMX8QXP_AUD_SAI_4_MCLK					403
#define IMX8QXP_AUD_SAI_5_IPG					404
#define IMX8QXP_AUD_SAI_5_MCLK					405
#define IMX8QXP_AUD_MQS_IPG					406
#define IMX8QXP_AUD_MQS_HMCLK					407
#define IMX8QXP_AUD_GPT5_IPG					408
#define IMX8QXP_AUD_GPT5_CLKIN					409
#define IMX8QXP_AUD_GPT6_IPG					410
#define IMX8QXP_AUD_GPT6_CLKIN					411
#define IMX8QXP_AUD_GPT7_IPG					412
#define IMX8QXP_AUD_GPT7_CLKIN					413
#define IMX8QXP_AUD_GPT8_IPG					414
#define IMX8QXP_AUD_GPT8_CLKIN					415
#define IMX8QXP_AUD_GPT9_IPG					416
#define IMX8QXP_AUD_GPT9_CLKIN					417
#define IMX8QXP_AUD_GPT10_IPG					418
#define IMX8QXP_AUD_GPT10_CLKIN					419
#define IMX8QXP_AUD_ACM_AUD_PLL_CLK0_DIV			420
#define IMX8QXP_AUD_ACM_AUD_PLL_CLK0_CLK			421
#define IMX8QXP_AUD_ACM_AUD_PLL_CLK1_DIV			422
#define IMX8QXP_AUD_ACM_AUD_PLL_CLK1_CLK			423
#define IMX8QXP_AUD_ACM_AUD_REC_CLK0_DIV			424
#define IMX8QXP_AUD_ACM_AUD_REC_CLK0_CLK			425
#define IMX8QXP_AUD_ACM_AUD_REC_CLK1_DIV			426
#define IMX8QXP_AUD_ACM_AUD_REC_CLK1_CLK			427
#define IMX8QXP_AUD_MCLKOUT0					428
#define IMX8QXP_AUD_MCLKOUT1					429
#define IMX8QXP_AUD_SPDIF_0_TX_CLK				430
#define IMX8QXP_AUD_SPDIF_0_GCLKW				431
#define IMX8QXP_AUD_SPDIF_0_IPG					432
#define IMX8QXP_AUD_ASRC_0_IPG					433
#define IMX8QXP_AUD_ASRC_1_IPG					434
#define IMX8QXP_AUD_HIFI_ADB_ACLK				435
#define IMX8QXP_AUD_HIFI_IPG					436
#define IMX8QXP_AUD_HIFI_CORE_CLK				437
#define IMX8QXP_AUD_OCRAM_IPG					438

/* DC part2 */
#define IMX8QXP_DC0_DISP0_DIV					439
#define IMX8QXP_DC0_DISP1_DIV					440
#define IMX8QXP_DC0_BYPASS_0_DIV				441
#define IMX8QXP_DC0_BYPASS_1_DIV				442
#define IMX8QXP_DC0_PLL0_DIV					443
#define IMX8QXP_DC0_PLL1_DIV					444
#define IMX8QXP_DC0_PLL0_CLK					445
#define IMX8QXP_DC0_PLL1_CLK					446

/* MIPI-LVDS part2 */
#define IMX8QXP_MIPI0_BYPASS_CLK				447
#define IMX8QXP_MIPI0_PIXEL_DIV					448
#define IMX8QXP_MIPI0_PIXEL_CLK					449
#define IMX8QXP_MIPI0_LVDS_PIXEL_DIV				450
#define IMX8QXP_MIPI0_LVDS_PIXEL_CLK				451
#define IMX8QXP_MIPI0_LVDS_BYPASS_CLK				452
#define IMX8QXP_MIPI0_LVDS_PHY_DIV				453
#define IMX8QXP_MIPI0_LVDS_PHY_CLK				454
#define IMX8QXP_MIPI0_DSI_TX_ESC_DIV				455
#define IMX8QXP_MIPI0_DSI_RX_ESC_DIV				456
#define IMX8QXP_MIPI0_DSI_TX_ESC_CLK				457
#define IMX8QXP_MIPI0_DSI_RX_ESC_CLK				458
#define IMX8QXP_MIPI0_LIS_IPG_CLK				459
#define IMX8QXP_MIPI1_I2C0_DIV					460
#define IMX8QXP_MIPI1_I2C1_DIV					461
#define IMX8QXP_MIPI1_I2C0_CLK					462
#define IMX8QXP_MIPI1_I2C1_CLK					463
#define IMX8QXP_MIPI1_I2C0_IPG_S_CLK				464
#define IMX8QXP_MIPI1_I2C0_IPG_CLK				465
#define IMX8QXP_MIPI1_I2C1_IPG_S_CLK				466
#define IMX8QXP_MIPI1_I2C1_IPG_CLK				467
#define IMX8QXP_MIPI1_PWM_IPG_S_CLK				468
#define IMX8QXP_MIPI1_PWM_IPG_CLK				469
#define IMX8QXP_MIPI1_PWM_32K_CLK				470
#define IMX8QXP_MIPI1_GPIO_IPG_CLK				471
#define IMX8QXP_MIPI1_BYPASS_CLK				472
#define IMX8QXP_MIPI1_PIXEL_DIV					473
#define IMX8QXP_MIPI1_PIXEL_CLK					474
#define IMX8QXP_MIPI1_LVDS_PIXEL_DIV				475
#define IMX8QXP_MIPI1_LVDS_PIXEL_CLK				476
#define IMX8QXP_MIPI1_LVDS_BYPASS_CLK				477
#define IMX8QXP_MIPI1_LVDS_PHY_DIV				478
#define IMX8QXP_MIPI1_LVDS_PHY_CLK				479
#define IMX8QXP_MIPI1_DSI_TX_ESC_DIV				480
#define IMX8QXP_MIPI1_DSI_RX_ESC_DIV				481
#define IMX8QXP_MIPI1_DSI_TX_ESC_CLK				482
#define IMX8QXP_MIPI1_DSI_RX_ESC_CLK				483

#define IMX8QXP_MIPI1_LIS_IPG_CLK				484

/* CM40 */
#define IMX8QXP_CM40_IPG_CLK					485
#define IMX8QXP_CM40_I2C_DIV					486
#define IMX8QXP_CM40_I2C_CLK					487
#define IMX8QXP_CM40_I2C_IPG_CLK				488

/* VPU clocks. */
#define IMX8QXP_VPU_ENC_CLK					489
#define IMX8QXP_VPU_DEC_CLK					490

/* MIPI-LVDS part3 */
#define IMX8QXP_MIPI0_DSI_PLL_CLK				491
#define IMX8QXP_MIPI0_DSI_PLL_DIV2_CLK				492
#define IMX8QXP_MIPI0_LVDS_PIXEL_SEL				493
#define IMX8QXP_MIPI0_LVDS_PHY_SEL				494
#define IMX8QXP_MIPI0_DSI_TX_ESC_SEL				495
#define IMX8QXP_MIPI0_DSI_RX_ESC_SEL				496
#define IMX8QXP_MIPI0_DSI_PHY_SEL				498
#define IMX8QXP_MIPI0_DSI_PHY_DIV				499
#define IMX8QXP_MIPI0_DSI_PHY_CLK				500
#define IMX8QXP_MIPI1_DSI_PLL_CLK				501
#define IMX8QXP_MIPI1_DSI_PLL_DIV2_CLK				502
#define IMX8QXP_MIPI1_LVDS_PIXEL_SEL				503
#define IMX8QXP_MIPI1_LVDS_PHY_SEL				504
#define IMX8QXP_MIPI1_DSI_TX_ESC_SEL				505
#define IMX8QXP_MIPI1_DSI_RX_ESC_SEL				506
#define IMX8QXP_MIPI1_DSI_PHY_SEL				507
#define IMX8QXP_MIPI1_DSI_PHY_DIV				508
#define IMX8QXP_MIPI1_DSI_PHY_CLK				509

/* DC part3 */
#define IMX8QXP_DC0_DPR1_APB_CLK				510
#define IMX8QXP_DC0_DPR1_B_CLK					511

#define IMX8QXP_CLK_END						512
#endif /* __DT_BINDINGS_CLOCK_IMX8QXP_H */
