/*
 * Implements the modbus handling portions of the bar faker.
 * Goals:
 * - look like an EM superficially (ie, sync capable master)
 * - provide extra options for lying about when the zero crossings were
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "mb.h"
#include <FreeRTOS.h>
#include <task.h>

//#include <remake/product_codes.h>
#include "syscfg.h"

#include "modbus.h"
#include "fmb_helpers.h"
#include "sync.h"
#include "serial_support.h"


struct mb_state_t {
	/* Common Core Modbus register map */
	struct smart_map_common core; /* 0x2000 - 0x200d */

	struct bar_faker_app_state *app;
};

static struct mb_state_t mb_state;



/**
 * Handle a modbus write to the em1 compatible range
 * @param src buffer we will read from
 * @param offset offset from 0x2000 in classic range
 * @param n how many registers were written
 * @return
 */
static bool mb_handle_em1_compat_write(uint8_t *src, uint16_t offset, int n) {
	int initial_offset = offset;
	(void)src;
	while (offset < initial_offset + n) {
		switch (offset) {
		default:
			/* just ignore unknown/readonly regs */
			offset++;
			break;
		}
	}
	return true;
}

/**
 * Fake read of em1 data, _only_ implements what's required
 *
 * @param dst modbus buffer
 * @param offset offset from 0x2000 into the classic range
 * @param n how many registers to read
 * @return whether it worked or not.  (always works)
 */
static bool mb_handle_em1_compat_read(uint8_t *dst, uint16_t offset, int n) {
	int initial_offset = offset;
	int i;
	int64_t sum_in;
	int val;
	float valf, valf2;
	uint16_t *tabidx = (uint16_t *) &mb_state.core;
	while (offset < initial_offset + n) {
		if (offset < sizeof(mb_state.core) / 2) {
			dst += fmb_write16(dst, tabidx[offset]);
			offset++;
			continue;
		}
		switch(offset) {
		case sg_frequency:
			/* TODO - read from dac channel 1 */
			dst += fmb_write16(dst, 50000);
			offset++;
			break;
		case sg_current1:
		case sg_current2:
		case sg_current3:
			/* TODO read from dac channel */
			dst += fmb_write32(dst, 12345);
			offset += 2;
			break;
		case sg_voltage1:
		case sg_voltage2:
		case sg_voltage3:
			/* Voltages are always artificial */
			dst += fmb_write32(dst, 230000);
			offset += 2;
			break;
		case sg_powerfactor1:
		case sg_powerfactor2:
		case sg_powerfactor3:
			/* FIXME - report based on our own bullshit?
			 * it doesn't matter though, just 1.0 is fine
			 */
			dst += fmb_write16(dst, 100);
			offset++;
			break;
		case sg_nr_of_phases:
			dst += fmb_write16(dst, 3);
			offset++;
			break;
		default:
			/* zero fill unknown/unaligned trailers */
			dst += fmb_write16(dst, 0);
			offset++;
			break;
		}
	}
	return true;
}

static bool mb_handle_em1_compat(uint8_t *dst, bool write, uint16_t offset, int n) {
	if (write) {
		ER_DPRINTF("mb_em1_write %d@%#x\n", n, offset);
		return mb_handle_em1_compat_write(dst, offset, n);
	} else {
		ER_DPRINTF("mb_em1_read %d@%#x\n", n, offset);
		return mb_handle_em1_compat_read(dst, offset, n);
	}
}

/**
 * This is where we fake out the zero crossing informations
 */
static void mb_read_powersync(uint8_t *dst, uint16_t addr, int n) {

	int initial_addr = addr;
	/* "valid", but complete lies */
	uint32_t last_zx[3];
	for (int i = 0; i < 3; i++) {
		last_zx[i] = mb_state.app->last_zx + mb_state.app->zx_phase_adjust[i];
		last_zx[i] += mb_state.app->base_offset;
	}

	while (addr < initial_addr + n) {
		switch (addr) {
		case 0:
			/* FIXME - might vary? */
			dst += fmb_write16(dst, 50000);
			addr++;
			break;
		case 2:
		case 4:
		case 6:
			/* Voltages are always fake */
			dst += fmb_write32(dst, 230000);
			addr += 2;
			break;
		case 8:
		case 10:
		case 12:
			dst += fmb_write32(dst, last_zx[(addr-8)/2]);
			addr += 2;
			break;
		default:
			/* zero fill unknown/unaligned trailers */
			dst += fmb_write16(dst, 0);
			addr++;
			break;
		}
	}
}

static void mb_read_sysstats(uint8_t *dst, uint16_t addr, int n) {
	int initial_addr = addr;
	while (addr < initial_addr + n) {
		switch (addr) {
		case 0:
			dst += fmb_write64(dst, xTaskGetTickCount());
			addr+=4;
			break;
		default:
			/* zero fill unknown/unaligned trailers */
			dst += fmb_write16(dst, 0);
			addr++;
			break;
		}
	}
}


static void mb_handle_em2_read(uint8_t *dst, uint16_t addr, int n)
{
	int initial_addr = addr;
	int64_t sum_in;
	while (addr < initial_addr + n) {
		switch (addr) {
		case 1:
			sum_in = 0xaabbccdd;
			dst += fmb_write64(dst, sum_in);
			addr += 4;
			break;
		default:
			/* zero fill unknown/unaligned trailers */
			dst += fmb_write16(dst, 0);
			addr++;
			break;
		}
	}
}




eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress,
	USHORT usNRegs, eMBRegisterMode eMode)
{
	/*
	 * FreeModbus has a hard++, converting address to reg#, but calling it address
	 * We prefer raw addresses please, 0 based.
	 */
	usAddress--;
	eMBErrorCode status = MB_ENOREG;

#if 1
	if ((usAddress >= MB_SMART_DEVICES_BASE_ADDRESS)
		&& (usAddress + usNRegs <= MB_SMART_DEVICES_BASE_ADDRESS + sg_num_registers)) {
		/* Handle them as offsets within the original static table */
		mb_handle_em1_compat(pucRegBuffer, eMode == MB_REG_WRITE,
			usAddress - MB_SMART_DEVICES_BASE_ADDRESS, usNRegs);
		status = MB_ENOERR;
	} else if (usAddress >= MB_BASE_ADDRESS_SYSSTATS && usAddress + usNRegs < MB_BASE_ADDRESS_SYSSTATS_END && eMode == MB_REG_READ) {
		/* System stats */
		mb_read_sysstats(pucRegBuffer, usAddress - MB_BASE_ADDRESS_SYSSTATS, usNRegs);
		status = MB_ENOERR;
	} else if ((usAddress >= 0x2100) && usAddress + usNRegs < 0x2200) {
		usAddress -= 0x2100;
		if (eMode == MB_REG_READ) {
			mb_handle_em2_read(pucRegBuffer, usAddress, usNRegs);
			status = MB_ENOERR;
		}
	} else if ((usAddress >= 0x2400) && usAddress + usNRegs < 0x2450) {
		/* pseudo PTP timing magic */
		if (eMode == MB_REG_READ) {
			ER_DPRINTF("reading sync %x %d\n", usAddress, usNRegs);
			sync_mb_read(pucRegBuffer, usAddress, usNRegs);
		} else if (eMode == MB_REG_WRITE) {
			if (usAddress == 0x2400) {
				//ER_DPRINTF("Ignoring sync itself\n");
			} else {
				ER_DPRINTF("writing sync %x %d\n", usAddress, usNRegs);
				if (sync_mb_write(pucRegBuffer, usAddress, usNRegs)) {
					status = MB_EINVAL;
				}
			}
		}
		status = MB_ENOERR;
	} else if (usAddress >= 0x2450 && usAddress + usNRegs < 0x2500 && eMode == MB_REG_READ) {
		/* Read out of values for power sync */
		ER_DPRINTF("psread %x %d\n", usAddress, usNRegs);
		mb_read_powersync(pucRegBuffer, usAddress - 0x2450, usNRegs);
		status = MB_ENOERR;
	}
#endif
	return status;

}

void task_modbus(void *pvParameters)
{
	mb_state.app = pvParameters;
	eMBErrorCode eStatus;

	uint8_t unitid = setup_serial_number(mb_state.core.serial);
	mb_state.core.vendor = MB_VENDOR_CODE;
	// TODO - fix up / unify product codes with existing code;
	mb_state.core.product = MB_PRODUCT_CODE_EM_FAKE_DAC1;
	mb_state.core.slaveid.slaveid = unitid;
	// TODO - comm settings? nah...

	eStatus = eMBInit(MB_RTU, unitid, 1, 19200, MB_PAR_EVEN, 1);
	assert(eStatus == MB_ENOERR);

	eStatus = eMBEnable();
	assert(eStatus == MB_ENOERR);
	(void) eStatus;
	/* TODO - either exit the task, or let the task restart and try
	 * and fix itself if these asserts failed */
	ER_DPRINTF("mb live: %#x\n", unitid);

	while (1) {
		eStatus = eMBPoll();
		if (eStatus != MB_ENOERR) {
			ER_DPRINTF("MBPoll failed: %d\n", eStatus);
		}

	}
}

