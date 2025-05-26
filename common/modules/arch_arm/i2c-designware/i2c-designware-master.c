// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Synopsys DesignWare I2C adapter driver (master only).
 *
 * Based on the TI DAVINCI I2C adapter driver.
 *
 * Copyright (C) 2006 Texas Instruments.
 * Copyright (C) 2007 MontaVista Software Inc.
 * Copyright (C) 2009 Provigent Ltd.
 */
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/export.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/pm_runtime.h>
#include <linux/reset.h>

#include "i2c-designware-core.h"

#ifdef CONFIG_ARCH_PHYTIUM
#include <linux/gpio.h>

#include <linux/acpi.h>
#include <linux/arm-smccc.h>

#define CPLD_STATUS_REG           0xE0
#define CPLD_UNLOCK_REG           0xE4

/* CPLD_STATUS_REG */
#define CPLD_STATUS_I2C0_SCL      0x1
#define CPLD_STATUS_I2C0_SDA      0x2
#define CPLD_STATUS_I2C1_SCL      0x4
#define CPLD_STATUS_I2C1_SDA      0x8
#define CPLD_STATUS_I2C0_BUSY     0x10
#define CPLD_STATUS_I2C1_BUSY     0x20
#define CPLD_STATUS_I2C0_DONE     0x40
#define CPLD_STATUS_I2C1_DONE     0x80
#define CPLD_STATUS_I2C0_MASK     (CPLD_STATUS_I2C0_SCL | CPLD_STATUS_I2C0_SDA)
#define CPLD_STATUS_I2C1_MASK     (CPLD_STATUS_I2C1_SCL | CPLD_STATUS_I2C1_SDA)

/* CPLD_UNLOCK_REG */
#define CPLD_UNLOCK_I2C0_9CLK     0x1
#define CPLD_UNLOCK_I2C1_9CLK     0x2
#define CPLD_UNLOCK_I2C0_DONE     0x4
#define CPLD_UNLOCK_I2C1_DONE     0x8

#define WB_GPIO_MULTIPLEX0_BASE 0x28100C00
#define WB_GPIO_MULTIPLEX_SIZE  0x1000
#define WB_GPIO_FUNC_0          0x6 /* enable lpc function mode */

#define WB_LPC_CPLD_BASE        0x20000000
#define WB_LPC_CPLD_SIZE        0x100
void __iomem *g_lpc_cpld_base = NULL;

static DEFINE_SPINLOCK(wb_cpld_9clk_unlock);

int wb_gpio_lpc_mode_enable(void)
{
#ifdef CONFIG_ARCH_PHYTIUM_FT1500
    u32 value;
    void __iomem *gpio_base;
#endif
    spin_lock(&wb_cpld_9clk_unlock);
#ifdef CONFIG_ARCH_PHYTIUM_FT1500
    gpio_base = ioremap((unsigned long)WB_GPIO_MULTIPLEX0_BASE,
        (unsigned long)WB_GPIO_MULTIPLEX_SIZE);
    if (IS_ERR(gpio_base)) {
        pr_err("gpio multiplex base remap failed\n");
        spin_unlock(&wb_cpld_9clk_unlock);
        return PTR_ERR(gpio_base);
    }
    value = readl(gpio_base);
    if ((value & WB_GPIO_FUNC_0) != WB_GPIO_FUNC_0) {
        /* enable lpc function mode */
        writel(WB_GPIO_FUNC_0, gpio_base);
    }
    iounmap(gpio_base);
#endif
    if (!g_lpc_cpld_base) {
        g_lpc_cpld_base = ioremap((unsigned long)WB_LPC_CPLD_BASE,
            (unsigned long)WB_LPC_CPLD_SIZE);
        if (IS_ERR(g_lpc_cpld_base)) {
            pr_err("g_lpc_cpld_base remap failed\n");
            spin_unlock(&wb_cpld_9clk_unlock);
            return PTR_ERR(g_lpc_cpld_base);
        }
    }
    spin_unlock(&wb_cpld_9clk_unlock);

    return 0;
}

static s32 i2c_reg_data_write(struct i2c_adapter *adapter,
						 u16 addr, unsigned int flags,u8 command, u8 value)
{
	union i2c_smbus_data data;
	data.byte = value;
	return i2c_smbus_xfer(adapter, addr, flags,
					I2C_SMBUS_WRITE, command,
					I2C_SMBUS_BYTE_DATA, &data);
}

static s32 i2c_reg_data_read(struct i2c_adapter *adapter,
						 u16 addr, unsigned int flags,u8 command)
{
	union i2c_smbus_data data;
	int status;

	status = i2c_smbus_xfer(adapter, addr, flags,
				I2C_SMBUS_READ, command,
				I2C_SMBUS_BYTE_DATA, &data);
	return (status < 0) ? status : data.byte;
}

static int i2c_reset_by_bus(struct dw_i2c_dev *dev)
{
	int reset_bus = dev->reset_bus;
	int offset = dev->reset_reg_off;
	int reset_addr = dev->reset_addr;
	char val = 0;
	int ret = -1;
	struct i2c_adapter *reset_adap;
    unsigned int flags = 0;

	if (reset_bus < 0 || offset < 0 ) {
		goto out;
	}

	reset_adap = i2c_get_adapter(reset_bus);
	if (!reset_adap) {
		dev_err(dev->dev,"cound not get i2c adapter\n");
		ret = -EPROBE_DEFER;
		goto out;
	}

	udelay(dev->rst_delay_b);
	val = i2c_reg_data_read(reset_adap, reset_addr, flags, offset);
	val &= ~(dev->mask);
	val |= dev->reset_on;

	i2c_reg_data_write(reset_adap, reset_addr, flags, offset, val);
	udelay(dev->rst_delay);

	udelay(dev->rst_delay_a);

	i2c_put_adapter(reset_adap);

	ret = 0;
out:
	return ret;
}

static int i2c_reset_by_gpio(struct dw_i2c_dev *dev)
{
	static u32 gpio_last_status = 0;
	static u32 gpio_init_ok = 0;
	int ret = -1;
	int reset_gpio = dev->reset_gpio;

	if (reset_gpio < 0) {
		goto out;
	}

	if (gpio_init_ok == 0) {
		if ((ret = gpio_request(dev->reset_gpio, "I2C_RESET")) < 0) {
			dev_info(dev->dev,"gpio_request failed\n");
			goto out;
		}
		gpio_direction_output(dev->reset_gpio, 1);
		gpio_init_ok = 1;
	}

	udelay(dev->rst_delay_b);
	if ( gpio_last_status == 0) {
		dev_info(dev->dev,"set to high\n");
		gpio_set_value(reset_gpio, 1);
		gpio_last_status = 1;
	} else {
		dev_info(dev->dev,"set to low\n");
		gpio_set_value(reset_gpio, 0);
		gpio_last_status = 0;
	}

	udelay(dev->rst_delay);

	udelay(dev->rst_delay_a);

	ret = 0;
out:
	return ret;;
}

static int i2c_reset_by_addr(struct dw_i2c_dev *dev)
{
	int reg_base = dev->reg_base;
	volatile u8 val;
	volatile u8 status;
	void __iomem *mem_reg_base = dev->mem_reg_base;
	void __iomem *reg_i2c_status;
	int ret = -1;
	int timeout = 10000;

	if (reg_base == 0) {
		goto out;
	}
	udelay(dev->rst_delay_b);
	val = readl(mem_reg_base);
	val &= ~(dev->mask);
	val |= dev->reset_on;

	writel(val, mem_reg_base);
	udelay(dev->rst_delay);

	/* 
	 * When using the done CPLD, 
	 * it is necessary to write to the done register to complete 9 clocks; otherwise, 
	 * the next 9 clocks cannot be sent. 
	 */
	if ((dev->done_on != -1) && (dev->mask_done != -1) && (dev->i2c_busy != -1)) {
		reg_i2c_status = ioremap(dev->reg_i2c_status, 1);

		/* 
		 * Add a timeout to prevent deadlocks. 
		 * Reading and writing can only proceed when reg_i2c_status is normal. 
		 */
		while (reg_i2c_status && (timeout--)) {
		/* 1、check whether the 9 clocks have been sent completely */
			status = readl(reg_i2c_status);
			if ( (status & dev->i2c_busy) == 0) {
				break;
			}
			udelay(10);
		}
		if (reg_i2c_status) {
			iounmap(reg_i2c_status);
		}
		/* 2、write to the done register */
		val = readl(mem_reg_base);
		val &= ~(dev->mask_done);
		val |= dev->done_on;
		writel(val, mem_reg_base);
	}

	udelay(dev->rst_delay_a);
	ret = 0;
out:
	return ret;
}

/**
 * i2c_cpld_9clk_unlock(int cpld_idx) - Detect and restore when I2C SCL&SDA is abnormal
 *
 * This functions Detect and restore when I2C SCL&SDA is abnormal.
 * CPLD initiates 9CLK to unlock when I2C SCL&SDA is abnormal.
 */
int i2c_cpld_9clk_unlock(struct dw_i2c_dev *dev)
{
	int ret = -1;
	if (dev->reset_src == RESET_BY_I2C_BUS) {
		dev_info(dev->dev,"try to send 9clk by i2c\n");
		ret = i2c_reset_by_bus(dev);
	} else if(dev->reset_src == RESET_BY_GPIO) {
		dev_info(dev->dev,"try to send 9clk by gpio\n");
		ret = i2c_reset_by_gpio(dev);
	} else if(dev->reset_src == RESET_BY_MEMADDR) {
		dev_info(dev->dev,"try to send 9clk by memaddr\n");
		ret = i2c_reset_by_addr(dev);
	}

	if (ret < 0) {
		dev_info(dev->dev,"try to send 9clk but not support\n");
	}
	
	return ret;
}

#ifndef CONFIG_ARCH_PHYTIUM_FT1500
int i2c_reset_controller(u32 ctrl_idx)
{
    struct arm_smccc_res res;
    
    if (ctrl_idx == 0x1) {
        arm_smccc_smc(0xc300FFF6,3,0x2,0,0,0,0,0,&res);
    } else if (ctrl_idx == 0x2) {
        arm_smccc_smc(0xc300FFF6,3,0x4,0,0,0,0,0,&res);
    }
    udelay(10);
    return 0;
}
#endif
#endif

static void i2c_dw_configure_fifo_master(struct dw_i2c_dev *dev)
{
	/* Configure Tx/Rx FIFO threshold levels */
	dw_writel(dev, dev->tx_fifo_depth / 2, DW_IC_TX_TL);
	dw_writel(dev, 0, DW_IC_RX_TL);

	/* Configure the I2C master */
	dw_writel(dev, dev->master_cfg, DW_IC_CON);
}

static int i2c_dw_set_timings_master(struct dw_i2c_dev *dev)
{
	const char *mode_str, *fp_str = "";
	u32 comp_param1;
	u32 sda_falling_time, scl_falling_time;
	struct i2c_timings *t = &dev->timings;
	u32 ic_clk;
	int ret;

	ret = i2c_dw_acquire_lock(dev);
	if (ret)
		return ret;
	comp_param1 = dw_readl(dev, DW_IC_COMP_PARAM_1);
	i2c_dw_release_lock(dev);

	/* Set standard and fast speed dividers for high/low periods */
	sda_falling_time = t->sda_fall_ns ?: 300; /* ns */
	scl_falling_time = t->scl_fall_ns ?: 300; /* ns */

	/* Calculate SCL timing parameters for standard mode if not set */
	if (!dev->ss_hcnt || !dev->ss_lcnt) {
		ic_clk = i2c_dw_clk_rate(dev);
		dev->ss_hcnt =
			i2c_dw_scl_hcnt(ic_clk,
					4000,	/* tHD;STA = tHIGH = 4.0 us */
					sda_falling_time,
					0,	/* 0: DW default, 1: Ideal */
					15);	/* Adjust the actual SCL clock frequency below 100khz */
		dev->ss_lcnt =
			i2c_dw_scl_lcnt(ic_clk,
					4700,	/* tLOW = 4.7 us */
					scl_falling_time,
					15);	/* Adjust the actual SCL clock frequency below 100khz */
	}
	dev_dbg(dev->dev, "Standard Mode HCNT:LCNT = %d:%d\n",
		dev->ss_hcnt, dev->ss_lcnt);

	/*
	 * Set SCL timing parameters for fast mode or fast mode plus. Only
	 * difference is the timing parameter values since the registers are
	 * the same.
	 */
	if (t->bus_freq_hz == 1000000) {
		/*
		 * Check are fast mode plus parameters available and use
		 * fast mode if not.
		 */
		if (dev->fp_hcnt && dev->fp_lcnt) {
			dev->fs_hcnt = dev->fp_hcnt;
			dev->fs_lcnt = dev->fp_lcnt;
			fp_str = " Plus";
		}
	}
	/*
	 * Calculate SCL timing parameters for fast mode if not set. They are
	 * needed also in high speed mode.
	 */
	if (!dev->fs_hcnt || !dev->fs_lcnt) {
		ic_clk = i2c_dw_clk_rate(dev);
		dev->fs_hcnt =
			i2c_dw_scl_hcnt(ic_clk,
					600,	/* tHD;STA = tHIGH = 0.6 us */
					sda_falling_time,
					0,	/* 0: DW default, 1: Ideal */
					0);	/* No offset */
		dev->fs_lcnt =
			i2c_dw_scl_lcnt(ic_clk,
					1300,	/* tLOW = 1.3 us */
					scl_falling_time,
					0);	/* No offset */
	}
	dev_dbg(dev->dev, "Fast Mode%s HCNT:LCNT = %d:%d\n",
		fp_str, dev->fs_hcnt, dev->fs_lcnt);

	/* Check is high speed possible and fall back to fast mode if not */
	if ((dev->master_cfg & DW_IC_CON_SPEED_MASK) ==
		DW_IC_CON_SPEED_HIGH) {
		if ((comp_param1 & DW_IC_COMP_PARAM_1_SPEED_MODE_MASK)
			!= DW_IC_COMP_PARAM_1_SPEED_MODE_HIGH) {
			dev_err(dev->dev, "High Speed not supported!\n");
			dev->master_cfg &= ~DW_IC_CON_SPEED_MASK;
			dev->master_cfg |= DW_IC_CON_SPEED_FAST;
			dev->hs_hcnt = 0;
			dev->hs_lcnt = 0;
		} else if (dev->hs_hcnt && dev->hs_lcnt) {
			dev_dbg(dev->dev, "High Speed Mode HCNT:LCNT = %d:%d\n",
				dev->hs_hcnt, dev->hs_lcnt);
		}
	}

	ret = i2c_dw_set_sda_hold(dev);
	if (ret)
		goto out;

	switch (dev->master_cfg & DW_IC_CON_SPEED_MASK) {
	case DW_IC_CON_SPEED_STD:
		mode_str = "Standard Mode";
		break;
	case DW_IC_CON_SPEED_HIGH:
		mode_str = "High Speed Mode";
		break;
	default:
		mode_str = "Fast Mode";
	}
	dev_dbg(dev->dev, "Bus speed: %s%s\n", mode_str, fp_str);

out:
	return ret;
}

/**
 * i2c_dw_init() - Initialize the designware I2C master hardware
 * @dev: device private data
 *
 * This functions configures and enables the I2C master.
 * This function is called during I2C init function, and in case of timeout at
 * run time.
 */
static int i2c_dw_init_master(struct dw_i2c_dev *dev)
{
	int ret;

	ret = i2c_dw_acquire_lock(dev);
	if (ret)
		return ret;

	/* Disable the adapter */
	__i2c_dw_disable(dev);

	/* Write standard speed timing parameters */
	dw_writel(dev, dev->ss_hcnt, DW_IC_SS_SCL_HCNT);
	dw_writel(dev, dev->ss_lcnt, DW_IC_SS_SCL_LCNT);

	/* Write fast mode/fast mode plus timing parameters */
	dw_writel(dev, dev->fs_hcnt, DW_IC_FS_SCL_HCNT);
	dw_writel(dev, dev->fs_lcnt, DW_IC_FS_SCL_LCNT);

	/* Write high speed timing parameters if supported */
	if (dev->hs_hcnt && dev->hs_lcnt) {
		dw_writel(dev, dev->hs_hcnt, DW_IC_HS_SCL_HCNT);
		dw_writel(dev, dev->hs_lcnt, DW_IC_HS_SCL_LCNT);
	}

	/* Write SDA hold time if supported */
	if (dev->sda_hold_time)
		dw_writel(dev, dev->sda_hold_time, DW_IC_SDA_HOLD);

	i2c_dw_configure_fifo_master(dev);
	i2c_dw_release_lock(dev);

	return 0;
}

static void i2c_dw_xfer_init(struct dw_i2c_dev *dev)
{
	struct i2c_msg *msgs = dev->msgs;
	u32 ic_con, ic_tar = 0;

	/* Disable the adapter */
	__i2c_dw_disable(dev);

	/* If the slave address is ten bit address, enable 10BITADDR */
	ic_con = dw_readl(dev, DW_IC_CON);
	if (msgs[dev->msg_write_idx].flags & I2C_M_TEN) {
		ic_con |= DW_IC_CON_10BITADDR_MASTER;
		/*
		 * If I2C_DYNAMIC_TAR_UPDATE is set, the 10-bit addressing
		 * mode has to be enabled via bit 12 of IC_TAR register.
		 * We set it always as I2C_DYNAMIC_TAR_UPDATE can't be
		 * detected from registers.
		 */
		ic_tar = DW_IC_TAR_10BITADDR_MASTER;
	} else {
		ic_con &= ~DW_IC_CON_10BITADDR_MASTER;
	}

	dw_writel(dev, ic_con, DW_IC_CON);

	/*
	 * Set the slave (target) address and enable 10-bit addressing mode
	 * if applicable.
	 */
	dw_writel(dev, msgs[dev->msg_write_idx].addr | ic_tar, DW_IC_TAR);

	/* Enforce disabled interrupts (due to HW issues) */
	i2c_dw_disable_int(dev);

	/* Enable the adapter */
	__i2c_dw_enable(dev);

	/* Dummy read to avoid the register getting stuck on Bay Trail */
	dw_readl(dev, DW_IC_ENABLE_STATUS);

	/* Clear and enable interrupts */
	dw_readl(dev, DW_IC_CLR_INTR);
	dw_writel(dev, DW_IC_INTR_MASTER_MASK, DW_IC_INTR_MASK);
}

/*
 * Initiate (and continue) low level master read/write transaction.
 * This function is only called from i2c_dw_isr, and pumping i2c_msg
 * messages into the tx buffer.  Even if the size of i2c_msg data is
 * longer than the size of the tx buffer, it handles everything.
 */
static void
i2c_dw_xfer_msg(struct dw_i2c_dev *dev)
{
	struct i2c_msg *msgs = dev->msgs;
	u32 intr_mask;
	int tx_limit, rx_limit;
	u32 addr = msgs[dev->msg_write_idx].addr;
	u32 buf_len = dev->tx_buf_len;
	u8 *buf = dev->tx_buf;
	bool need_restart = false;

	intr_mask = DW_IC_INTR_MASTER_MASK;

	for (; dev->msg_write_idx < dev->msgs_num; dev->msg_write_idx++) {
		u32 flags = msgs[dev->msg_write_idx].flags;

		/*
		 * If target address has changed, we need to
		 * reprogram the target address in the I2C
		 * adapter when we are done with this transfer.
		 */
		if (msgs[dev->msg_write_idx].addr != addr) {
			dev_err(dev->dev,
				"%s: invalid target address\n", __func__);
			dev->msg_err = -EINVAL;
			break;
		}

		if (!(dev->status & STATUS_WRITE_IN_PROGRESS)) {
			/* new i2c_msg */
			buf = msgs[dev->msg_write_idx].buf;
			buf_len = msgs[dev->msg_write_idx].len;

			/* If both IC_EMPTYFIFO_HOLD_MASTER_EN and
			 * IC_RESTART_EN are set, we must manually
			 * set restart bit between messages.
			 */
			if ((dev->master_cfg & DW_IC_CON_RESTART_EN) &&
					(dev->msg_write_idx > 0))
				need_restart = true;
		}

		tx_limit = dev->tx_fifo_depth - dw_readl(dev, DW_IC_TXFLR);
		rx_limit = dev->rx_fifo_depth - dw_readl(dev, DW_IC_RXFLR);

		while (buf_len > 0 && tx_limit > 0 && rx_limit > 0) {
			u32 cmd = 0;

			/*
			 * If IC_EMPTYFIFO_HOLD_MASTER_EN is set we must
			 * manually set the stop bit. However, it cannot be
			 * detected from the registers so we set it always
			 * when writing/reading the last byte.
			 */

			/*
			 * i2c-core always sets the buffer length of
			 * I2C_FUNC_SMBUS_BLOCK_DATA to 1. The length will
			 * be adjusted when receiving the first byte.
			 * Thus we can't stop the transaction here.
			 */
			if (dev->msg_write_idx == dev->msgs_num - 1 &&
			    buf_len == 1 && !(flags & I2C_M_RECV_LEN))
				cmd |= BIT(9);

			if (need_restart) {
#ifndef CONFIG_ARCH_PHYTIUM_FT1500
    			/*
    			 * fixme! phytium ft1500a4 Restart must not be allowed in feiteng 
    			 * i2c write operation, otherwise write address will be abnormal.
    			 */
				cmd |= BIT(10);
#endif				
				need_restart = false;
			}

			if (msgs[dev->msg_write_idx].flags & I2C_M_RD) {

				/* Avoid rx buffer overrun */
				if (dev->rx_outstanding >= dev->rx_fifo_depth)
					break;

				dw_writel(dev, cmd | 0x100, DW_IC_DATA_CMD);
				rx_limit--;
				dev->rx_outstanding++;
			} else
				dw_writel(dev, cmd | *buf++, DW_IC_DATA_CMD);
			tx_limit--; buf_len--;
		}

		dev->tx_buf = buf;
		dev->tx_buf_len = buf_len;

		/*
		 * Because we don't know the buffer length in the
		 * I2C_FUNC_SMBUS_BLOCK_DATA case, we can't stop
		 * the transaction here.
		 */
		if (buf_len > 0 || flags & I2C_M_RECV_LEN) {
			/* more bytes to be written */
			dev->status |= STATUS_WRITE_IN_PROGRESS;
			break;
		} else
			dev->status &= ~STATUS_WRITE_IN_PROGRESS;
	}

	/*
	 * If i2c_msg index search is completed, we don't need TX_EMPTY
	 * interrupt any more.
	 */
	if (dev->msg_write_idx == dev->msgs_num)
		intr_mask &= ~DW_IC_INTR_TX_EMPTY;

	if (dev->msg_err)
		intr_mask = 0;

	dw_writel(dev, intr_mask,  DW_IC_INTR_MASK);
}

static u8
i2c_dw_recv_len(struct dw_i2c_dev *dev, u8 len)
{
	struct i2c_msg *msgs = dev->msgs;
	u32 flags = msgs[dev->msg_read_idx].flags;

	/*
	 * Adjust the buffer length and mask the flag
	 * after receiving the first byte.
	 */
	len += (flags & I2C_CLIENT_PEC) ? 2 : 1;
	dev->tx_buf_len = len - min_t(u8, len, dev->rx_outstanding);
	msgs[dev->msg_read_idx].len = len;
	msgs[dev->msg_read_idx].flags &= ~I2C_M_RECV_LEN;

	return len;
}

static void
i2c_dw_read(struct dw_i2c_dev *dev)
{
	struct i2c_msg *msgs = dev->msgs;
	int rx_valid;

	for (; dev->msg_read_idx < dev->msgs_num; dev->msg_read_idx++) {
		u32 len;
		u8 *buf;

		if (!(msgs[dev->msg_read_idx].flags & I2C_M_RD))
			continue;

		if (!(dev->status & STATUS_READ_IN_PROGRESS)) {
			len = msgs[dev->msg_read_idx].len;
			buf = msgs[dev->msg_read_idx].buf;
		} else {
			len = dev->rx_buf_len;
			buf = dev->rx_buf;
		}

		rx_valid = dw_readl(dev, DW_IC_RXFLR);

		for (; len > 0 && rx_valid > 0; len--, rx_valid--) {
			u32 flags = msgs[dev->msg_read_idx].flags;

			*buf = dw_readl(dev, DW_IC_DATA_CMD);
			/* Ensure length byte is a valid value */
			if (flags & I2C_M_RECV_LEN &&
				*buf <= I2C_SMBUS_BLOCK_MAX && *buf > 0) {
				len = i2c_dw_recv_len(dev, *buf);
			}
			buf++;
			dev->rx_outstanding--;
		}

		if (len > 0) {
			dev->status |= STATUS_READ_IN_PROGRESS;
			dev->rx_buf_len = len;
			dev->rx_buf = buf;
			return;
		} else
			dev->status &= ~STATUS_READ_IN_PROGRESS;
	}
}

static int i2c_dw_recover(struct dw_i2c_dev *dev)
{
	/* i2c_dw_init implicitly disables the adapter */
	i2c_recover_bus(&dev->adapter);
#ifndef CONFIG_ARCH_PHYTIUM_FT1500
	i2c_reset_controller(dev->ctrl_idx);
#endif
#ifdef CONFIG_ARCH_PHYTIUM
	i2c_cpld_9clk_unlock(dev);
#endif
	i2c_dw_init_master(dev);
	return 0;
}

/*
 * Prepare controller for a transaction and call i2c_dw_xfer_msg.
 */
static int
i2c_dw_xfer(struct i2c_adapter *adap, struct i2c_msg msgs[], int num)
{
	struct dw_i2c_dev *dev = i2c_get_adapdata(adap);
	int ret;
	uint32_t msg_byte;
	uint32_t msg_index;
	uint32_t timeout_ms = 0;
	struct i2c_timings *t = &dev->timings;

	/* dev_dbg(dev->dev, "%s: msgs: %d\n", __func__, num); */

	pm_runtime_get_sync(dev->dev);

	reinit_completion(&dev->cmd_complete);
	dev->msgs = msgs;
	dev->msgs_num = num;
	dev->cmd_err = 0;
	dev->msg_write_idx = 0;
	dev->msg_read_idx = 0;
	dev->msg_err = 0;
	dev->status = STATUS_IDLE;
	dev->abort_source = 0;
	dev->rx_outstanding = 0;

	ret = i2c_dw_acquire_lock(dev);
	if (ret)
		goto done_nolock;

	ret = i2c_dw_wait_bus_not_busy(dev);
	if (ret < 0) {
		dev_info(dev->dev, "wait bus timeout and try to reset i2c\n");
		i2c_dw_recover(dev);
		ret = -ETIMEDOUT;
		goto done;
	}

	/* Start the transfers */
	i2c_dw_xfer_init(dev);

	msg_byte = 0;
	for (msg_index = 0; msg_index < num; msg_index++) {
		msg_byte += msgs[msg_index].len + 1;
	}

	/* 20 times more than normal transfer */
	/* Here 200ms is added as a bias to prevent cpu drawback (such as ft2000, which will cause 
         * slow response to i2c interrupts in flash copy) causing frequent i2c timeouts. 
         * Add 200ms here to avoid 
         */
	if (t->bus_freq_hz > 0) {
		timeout_ms = 20 * 9 * (msg_byte * 1000) / (t->bus_freq_hz) + 200;
	} else {
		timeout_ms = 1000;
	}

	/* Wait for tx to complete */
	if (!wait_for_completion_timeout(&dev->cmd_complete, msecs_to_jiffies(timeout_ms))) {
		dev_info(dev->dev, "controller timed out\n");
		i2c_dw_recover(dev);
		ret = -ETIMEDOUT;
		goto done;
	}

	/*
	 * We must disable the adapter before returning and signaling the end
	 * of the current transfer. Otherwise the hardware might continue
	 * generating interrupts which in turn causes a race condition with
	 * the following transfer.  Needs some more investigation if the
	 * additional interrupts are a hardware bug or this driver doesn't
	 * handle them correctly yet.
	 */
	__i2c_dw_disable_nowait(dev);

	if (dev->msg_err) {
		ret = dev->msg_err;
		goto done;
	}

	/* No error */
	if (likely(!dev->cmd_err && !dev->status)) {
		ret = num;
		goto done;
	}

	/* We have an error */
	if (dev->cmd_err == DW_IC_ERR_TX_ABRT) {
		ret = i2c_dw_handle_tx_abort(dev);
		goto done;
	}

	if (dev->status)
		dev_info(dev->dev,
			"transfer terminated early - interrupt latency too high?\n");

	ret = -EIO;

done:
	i2c_dw_release_lock(dev);


done_nolock:
	pm_runtime_mark_last_busy(dev->dev);
	pm_runtime_put_autosuspend(dev->dev);

	return ret;
}

static const struct i2c_algorithm i2c_dw_algo = {
	.master_xfer = i2c_dw_xfer,
	.functionality = i2c_dw_func,
};

static const struct i2c_adapter_quirks i2c_dw_quirks = {
	.flags = I2C_AQ_NO_ZERO_LEN,
};

static u32 i2c_dw_read_clear_intrbits(struct dw_i2c_dev *dev)
{
	u32 stat;

	/*
	 * The IC_INTR_STAT register just indicates "enabled" interrupts.
	 * Ths unmasked raw version of interrupt status bits are available
	 * in the IC_RAW_INTR_STAT register.
	 *
	 * That is,
	 *   stat = dw_readl(IC_INTR_STAT);
	 * equals to,
	 *   stat = dw_readl(IC_RAW_INTR_STAT) & dw_readl(IC_INTR_MASK);
	 *
	 * The raw version might be useful for debugging purposes.
	 */
	stat = dw_readl(dev, DW_IC_INTR_STAT);

	/*
	 * Do not use the IC_CLR_INTR register to clear interrupts, or
	 * you'll miss some interrupts, triggered during the period from
	 * dw_readl(IC_INTR_STAT) to dw_readl(IC_CLR_INTR).
	 *
	 * Instead, use the separately-prepared IC_CLR_* registers.
	 */
	if (stat & DW_IC_INTR_RX_UNDER)
		dw_readl(dev, DW_IC_CLR_RX_UNDER);
	if (stat & DW_IC_INTR_RX_OVER)
		dw_readl(dev, DW_IC_CLR_RX_OVER);
	if (stat & DW_IC_INTR_TX_OVER)
		dw_readl(dev, DW_IC_CLR_TX_OVER);
	if (stat & DW_IC_INTR_RD_REQ)
		dw_readl(dev, DW_IC_CLR_RD_REQ);
	if (stat & DW_IC_INTR_TX_ABRT) {
		/*
		 * The IC_TX_ABRT_SOURCE register is cleared whenever
		 * the IC_CLR_TX_ABRT is read.  Preserve it beforehand.
		 */
		dev->abort_source = dw_readl(dev, DW_IC_TX_ABRT_SOURCE);
		dw_readl(dev, DW_IC_CLR_TX_ABRT);
	}
	if (stat & DW_IC_INTR_RX_DONE)
		dw_readl(dev, DW_IC_CLR_RX_DONE);
	if (stat & DW_IC_INTR_ACTIVITY)
		dw_readl(dev, DW_IC_CLR_ACTIVITY);
	if (stat & DW_IC_INTR_STOP_DET)
		dw_readl(dev, DW_IC_CLR_STOP_DET);
	if (stat & DW_IC_INTR_START_DET)
		dw_readl(dev, DW_IC_CLR_START_DET);
	if (stat & DW_IC_INTR_GEN_CALL)
		dw_readl(dev, DW_IC_CLR_GEN_CALL);

	return stat;
}

/*
 * Interrupt service routine. This gets called whenever an I2C master interrupt
 * occurs.
 */
static int i2c_dw_irq_handler_master(struct dw_i2c_dev *dev)
{
	u32 stat;

	stat = i2c_dw_read_clear_intrbits(dev);
	if (stat & DW_IC_INTR_TX_ABRT) {
		dev->cmd_err |= DW_IC_ERR_TX_ABRT;
		dev->status = STATUS_IDLE;

		/*
		 * Anytime TX_ABRT is set, the contents of the tx/rx
		 * buffers are flushed. Make sure to skip them.
		 */
		dw_writel(dev, 0, DW_IC_INTR_MASK);
		goto tx_aborted;
	}

	if (stat & DW_IC_INTR_RX_FULL)
		i2c_dw_read(dev);

	if (stat & DW_IC_INTR_TX_EMPTY)
		i2c_dw_xfer_msg(dev);

	/*
	 * No need to modify or disable the interrupt mask here.
	 * i2c_dw_xfer_msg() will take care of it according to
	 * the current transmit status.
	 */

tx_aborted:
	if ((stat & (DW_IC_INTR_TX_ABRT | DW_IC_INTR_STOP_DET)) || dev->msg_err)
		complete(&dev->cmd_complete);
	else if (unlikely(dev->flags & ACCESS_INTR_MASK)) {
		/* Workaround to trigger pending interrupt */
		stat = dw_readl(dev, DW_IC_INTR_MASK);
		i2c_dw_disable_int(dev);
		dw_writel(dev, stat, DW_IC_INTR_MASK);
	}

	return 0;
}

static irqreturn_t i2c_dw_isr(int this_irq, void *dev_id)
{
	struct dw_i2c_dev *dev = dev_id;
	u32 stat, enabled;

	enabled = dw_readl(dev, DW_IC_ENABLE);
	stat = dw_readl(dev, DW_IC_RAW_INTR_STAT);
	dev_dbg(dev->dev, "enabled=%#x stat=%#x\n", enabled, stat);
	if (!enabled || !(stat & ~DW_IC_INTR_ACTIVITY)) {
        return IRQ_HANDLED;
    }

	i2c_dw_irq_handler_master(dev);

	return IRQ_HANDLED;
}

static void i2c_dw_prepare_recovery(struct i2c_adapter *adap)
{
	struct dw_i2c_dev *dev = i2c_get_adapdata(adap);

	i2c_dw_disable(dev);
	reset_control_assert(dev->rst);
	i2c_dw_prepare_clk(dev, false);
}

static void i2c_dw_unprepare_recovery(struct i2c_adapter *adap)
{
	struct dw_i2c_dev *dev = i2c_get_adapdata(adap);

	i2c_dw_prepare_clk(dev, true);
	reset_control_deassert(dev->rst);
	i2c_dw_init_master(dev);
}

static int i2c_dw_init_recovery_info(struct dw_i2c_dev *dev)
{
	struct i2c_bus_recovery_info *rinfo = &dev->rinfo;
	struct i2c_adapter *adap = &dev->adapter;
	struct gpio_desc *gpio;
	int r;

	gpio = devm_gpiod_get(dev->dev, "scl", GPIOD_OUT_HIGH);
	if (IS_ERR(gpio)) {
		r = PTR_ERR(gpio);
		if (r == -ENOENT || r == -ENOSYS)
			return 0;
		return r;
	}
	rinfo->scl_gpiod = gpio;

	gpio = devm_gpiod_get_optional(dev->dev, "sda", GPIOD_IN);
	if (IS_ERR(gpio))
		return PTR_ERR(gpio);
	rinfo->sda_gpiod = gpio;

	rinfo->recover_bus = i2c_generic_scl_recovery;
	rinfo->prepare_recovery = i2c_dw_prepare_recovery;
	rinfo->unprepare_recovery = i2c_dw_unprepare_recovery;
	adap->bus_recovery_info = rinfo;

	dev_info(dev->dev, "running with gpio recovery mode! scl%s",
		 rinfo->sda_gpiod ? ",sda" : "");

	return 0;
}

int i2c_dw_probe(struct dw_i2c_dev *dev)
{
	struct i2c_adapter *adap = &dev->adapter;
	unsigned long irq_flags;
	int ret;

	init_completion(&dev->cmd_complete);

	dev->init = i2c_dw_init_master;
	dev->disable = i2c_dw_disable;
	dev->disable_int = i2c_dw_disable_int;

	ret = i2c_dw_set_reg_access(dev);
	if (ret)
		return ret;

	ret = i2c_dw_set_timings_master(dev);
	if (ret)
		return ret;

	ret = dev->init(dev);
	if (ret)
		return ret;

	snprintf(adap->name, sizeof(adap->name),
		 "Synopsys DesignWare I2C adapter");
	adap->retries = 0;
	adap->algo = &i2c_dw_algo;
	adap->quirks = &i2c_dw_quirks;
	adap->dev.parent = dev->dev;
	i2c_set_adapdata(adap, dev);

	if (dev->pm_disabled) {
		irq_flags = IRQF_NO_SUSPEND;
	} else {
		irq_flags = IRQF_SHARED | IRQF_COND_SUSPEND;
	}

	i2c_dw_disable_int(dev);
	ret = devm_request_irq(dev->dev, dev->irq, i2c_dw_isr, irq_flags,
			       dev_name(dev->dev), dev);
	if (ret) {
		dev_err(dev->dev, "failure requesting irq %i: %d\n",
			dev->irq, ret);
		return ret;
	}

	ret = i2c_dw_init_recovery_info(dev);
	if (ret)
		return ret;

	/*
	 * Increment PM usage count during adapter registration in order to
	 * avoid possible spurious runtime suspend when adapter device is
	 * registered to the device core and immediate resume in case bus has
	 * registered I2C slaves that do I2C transfers in their probe.
	 */
	pm_runtime_get_noresume(dev->dev);
	ret = i2c_add_numbered_adapter(adap);
	if (ret)
		dev_err(dev->dev, "failure adding adapter: %d\n", ret);
	pm_runtime_put_noidle(dev->dev);

#ifdef CONFIG_ARCH_PHYTIUM
    wb_gpio_lpc_mode_enable();
#endif

	return ret;
}
EXPORT_SYMBOL_GPL(i2c_dw_probe);

MODULE_DESCRIPTION("Synopsys DesignWare I2C bus master adapter");
MODULE_LICENSE("GPL");
