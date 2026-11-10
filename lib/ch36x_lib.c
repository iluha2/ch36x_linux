/*
 * ch365/ch367/ch368 PCI/PCIE application library
 *
 * Copyright (C) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
 * Web:      http://wch.cn
 * Author:   WCH <tech@wch.cn>
 *
 * Copyright (C) 2026 INFOTECS AT
 * Author:   Iluha Companets
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Cross-compile with cross-gcc -I /path/to/cross-kernel/include
 *
 * Update Log:
 * V1.0 - initial version
 * V1.1 - modify APIs related to interrupt
 * V1.2 - add APIs related to SPI transfer
 * V1.3 - fix bugs of memory block read/write
 * V1.4 - add feature - MSI using (fork https://github.com/iluha2/ch36x_linux)
 */

#include <fcntl.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "ch36x_lib.h"


/**
 * ch36x_open - open ch36x device
 * @devname: the device name to open
 *
 * The function return the new file descriptor, or -1 if an error occurred
 */
int ch36x_open(const char *devname)
{
	int fd = open(devname, O_RDWR);
	if (fd > 0) {
		fcntl(fd, F_SETOWN, getpid());
		fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | FASYNC);
	}
	return fd;
}

/**
 * ch36x_close - close ch36x device
 * @fd: the device handle
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_close(int fd)
{
	return close(fd);
}

/**
 * ch36x_get_chiptype - get chip model
 * @fd: file descriptor of ch36x device
 * @chiptype: pointer to chiptype
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_get_chiptype(int fd, enum CHIP_TYPE *chiptype)
{
	return ioctl(fd, CH36x_GET_CHIPTYPE, (unsigned long)chiptype);
}

/**
 * ch36x_get_version - get driver version
 * @fd: file descriptor of ch36x device
 * @version: pointer to version string
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_get_version(int fd, char *version)
{
	return ioctl(fd, CH36x_GET_VERSION, (unsigned long)version);
}

/**
 * ch36x_get_irq - get irq number
 * @fd: file descriptor of ch36x device
 * @irq: pointer to irq number
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_get_irq(int fd, int *irq)
{
	return ioctl(fd, CH36x_GET_IRQ, (unsigned long)irq);
}

/**
 * ch36x_get_ioaddr - get io base address of ch36x
 * @fd: file descriptor of ch36x device
 * @ioaddr: pointer to io base address
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_get_ioaddr(int fd, void *ioaddr)
{
	return ioctl(fd, CH36x_GET_IO_BASE_ADDR, (unsigned long)ioaddr);
}

/**
 * ch36x_get_memaddr - get io memory address of ch36x
 * @fd: file descriptor of ch36x device
 * @memaddr: pointer to memory base address
 *
 * The function return 0 if success, others if fail.
 */

int ch36x_get_memaddr(int fd, void *memaddr)
{
	return ioctl(fd, CH36x_GET_MEM_BASE_ADDR, (unsigned long)memaddr);
}

/**
 * ch36x_read_config_byte - read one byte from config space
 * @fd: file descriptor of ch36x device
 * @offset: config space register offset
 * @obyte: pointer to read byte
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_read_config_byte(int fd, uint8_t offset, uint8_t *obyte)
{
	struct ch36x_read_config_t {
		uint8_t offset;
		uint8_t obyte;
	} __attribute__((packed));

	struct ch36x_read_config_t ch36x_read_config;
	ch36x_read_config.offset = offset;

	int ret = ioctl(fd, CH36x_READ_CONFIG_BYTE, (unsigned long)&ch36x_read_config);
	if (ret >= 0)
		*obyte = ch36x_read_config.obyte;

	return ret;
}

/**
 * ch36x_read_config_word - read one word from config space
 * @fd: file descriptor of ch36x device
 * @offset: config space register offset
 * @oword: pointer to read word
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_read_config_word(int fd, uint8_t offset, uint16_t *oword)
{
	struct ch36x_read_config_t {
		uint8_t offset;
		uint16_t oword;
	} __attribute__((packed));

	struct ch36x_read_config_t ch36x_read_config;
	ch36x_read_config.offset = offset;

	int ret = ioctl(fd, CH36x_READ_CONFIG_WORD, (unsigned long)&ch36x_read_config);
	if (ret >= 0)
		*oword = ch36x_read_config.oword;

	return ret;
}

/**
 * ch36x_read_config_dword - read one dword from config space
 * @fd: file descriptor of ch36x device
 * @offset: config space register offset
 * @oword: pointer to read dword
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_read_config_dword(int fd, uint8_t offset, uint32_t *odword)
{
	struct ch36x_read_config_t {
		uint8_t offset;
		uint32_t odword;
	} __attribute__((packed));

	struct ch36x_read_config_t ch36x_read_config;
	ch36x_read_config.offset = offset;

	int ret = ioctl(fd, CH36x_READ_CONFIG_DWORD, (unsigned long)&ch36x_read_config);
	if (ret >= 0)
		*odword = ch36x_read_config.odword;

	return ret;
}

/**
 * ch36x_write_config_byte - write one byte to config space
 * @fd: file descriptor of ch36x device
 * @offset: config space register offset
 * @ibyte: byte to write
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_write_config_byte(int fd, uint8_t offset, uint8_t ibyte)
{
	struct ch36x_write_config_t {
		uint8_t offset;
		uint8_t ibyte;
	} __attribute__((packed));

	struct ch36x_write_config_t ch36x_write_config;
	ch36x_write_config.offset = offset;
	ch36x_write_config.ibyte = ibyte;

	return ioctl(fd, CH36x_WRITE_CONFIG_BYTE, (unsigned long)&ch36x_write_config);
}

/**
 * ch36x_write_config_word - write one word to config space
 * @fd: file descriptor of ch36x device
 * @offset: config space register offset
 * @iword: word to write
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_write_config_word(int fd, uint8_t offset, uint16_t iword)
{
	struct ch36x_write_config_t {
		uint8_t offset;
		uint16_t iword;
	} __attribute__((packed));

	struct ch36x_write_config_t ch36x_write_config;
	ch36x_write_config.offset = offset;
	ch36x_write_config.iword = iword;

	return ioctl(fd, CH36x_WRITE_CONFIG_WORD, (unsigned long)&ch36x_write_config);
}

/**
 * ch36x_write_config_dword - write one dword to config space
 * @fd: file descriptor of ch36x device
 * @offset: config space register offset
 * @dword: dword to write
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_write_config_dword(int fd, uint8_t offset, uint32_t idword)
{
	struct ch36x_write_config_t {
		uint8_t offset;
		uint32_t idword;
	} __attribute__((packed));

	struct ch36x_write_config_t ch36x_write_config;
	ch36x_write_config.offset = offset;
	ch36x_write_config.idword = idword;

	return ioctl(fd, CH36x_WRITE_CONFIG_DWORD, (unsigned long)&ch36x_write_config);
}

/**
 * ch36x_read_io_byte - read one byte from io space
 * @fd: file descriptor of ch36x device
 * @offset: io space register offset
 * @obyte: pointer to read byte
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_read_io_byte(int fd, uint8_t offset, uint8_t *obyte)
{
	struct ch36x_read_io_t {
		uint8_t offset;
		uint8_t obyte;
	} __attribute__((packed));

	struct ch36x_read_io_t ch36x_read_io;
	ch36x_read_io.offset = offset;

	int ret = ioctl(fd, CH36x_READ_IO_BYTE, (unsigned long)&ch36x_read_io);
	if (ret >= 0)
		*obyte = ch36x_read_io.obyte;

	return ret;
}

/**
 * ch36x_read_io_word - read one byte from io word
 * @fd: file descriptor of ch36x device
 * @offset: io space register offset
 * @oword: pointer to read word
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_read_io_word(int fd, uint8_t offset, uint16_t *oword)
{
	struct ch36x_read_io_t {
		uint8_t offset;
		uint16_t oword;
	} __attribute__((packed));

	struct ch36x_read_io_t ch36x_read_io;
	ch36x_read_io.offset = offset;

	int ret = ioctl(fd, CH36x_READ_IO_WORD, (unsigned long)&ch36x_read_io);
	if (ret >= 0)
		*oword = ch36x_read_io.oword;

	return ret;
}

/**
 * ch36x_read_io_dword - read one dword from io space
 * @fd: file descriptor of ch36x device
 * @offset: io space register offset
 * @odword: pointer to read dword
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_read_io_dword(int fd, uint8_t offset, uint32_t *odword)
{
	struct ch36x_read_io_t {
		uint8_t offset;
		uint32_t odword;
	} __attribute__((packed));

	struct ch36x_read_io_t ch36x_read_io;
	ch36x_read_io.offset = offset;

	int ret = ioctl(fd, CH36x_READ_IO_DWORD, (unsigned long)&ch36x_read_io);
	if (ret >= 0)
		*odword = ch36x_read_io.odword;

	return ret;
}

/**
 * ch36x_write_io_byte - write one byte to io space
 * @fd: file descriptor of ch36x device
 * @offset: io space register offset
 * @ibyte: byte to write
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_write_io_byte(int fd, uint8_t offset, uint8_t ibyte)
{
	struct ch36x_write_io_t {
		uint8_t offset;
		uint8_t ibyte;
	} __attribute__((packed));

	struct ch36x_write_io_t ch36x_write_io;
	ch36x_write_io.offset = offset;
	ch36x_write_io.ibyte = ibyte;

	return ioctl(fd, CH36x_WRITE_IO_BYTE, (unsigned long)&ch36x_write_io);
}

/**
 * ch36x_write_io_word - write one word to io space
 * @fd: file descriptor of ch36x device
 * @offset: io space register offset
 * @iword: word to write
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_write_io_word(int fd, uint8_t offset, uint16_t iword)
{
	struct ch36x_write_io_t {
		uint8_t offset;
		uint16_t iword;
	} __attribute__((packed));

	struct ch36x_write_io_t ch36x_write_io;
	ch36x_write_io.offset = offset;
	ch36x_write_io.iword = iword;

	return ioctl(fd, CH36x_WRITE_IO_WORD, (unsigned long)&ch36x_write_io);
}

/**
 * ch36x_write_io_dword - write one dword to io space
 * @fd: file descriptor of ch36x device
 * @offset: io space register offset
 * @idword: dword to write
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_write_io_dword(int fd, uint8_t offset, uint32_t idword)
{
	struct ch36x_write_io_t {
		uint8_t offset;
		uint32_t idword;
	} __attribute__((packed));

	struct ch36x_write_io_t ch36x_write_io;
	ch36x_write_io.offset = offset;
	ch36x_write_io.idword = idword;

	return ioctl(fd, CH36x_WRITE_IO_DWORD, (unsigned long)&ch36x_write_io);
}

/**
 * ch36x_read_mem_byte - read one byte from memory space
 * @fd: file descriptor of ch36x device
 * @offset: memory space address offset
 * @obyte: pointer to read byte
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_read_mem_byte(int fd, uint16_t offset, uint8_t *obyte)
{
	struct ch36x_read_mem_t {
		uint16_t offset;
		uint8_t obyte;
	} __attribute__((packed));

	struct ch36x_read_mem_t ch36x_read_mem;
	ch36x_read_mem.offset = offset;

	int ret = ioctl(fd, CH36x_READ_MEM_BYTE, (unsigned long)&ch36x_read_mem);
	if (ret >= 0)
		*obyte = ch36x_read_mem.obyte;

	return ret;
}

/**
 * ch36x_read_mem_word - read one word from memory space
 * @fd: file descriptor of ch36x device
 * @offset: memory space address offset
 * @oword: pointer to read word
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_read_mem_word(int fd, uint16_t offset, uint16_t *oword)
{
	struct ch36x_read_mem_t {
		uint16_t offset;
		uint16_t oword;
	} __attribute__((packed));

	struct ch36x_read_mem_t ch36x_read_mem;
	ch36x_read_mem.offset = offset;

	int ret = ioctl(fd, CH36x_READ_MEM_WORD, (unsigned long)&ch36x_read_mem);
	if (ret >= 0)
		*oword = ch36x_read_mem.oword;

	return ret;
}

/**
 * ch36x_read_mem_dword - read one dword from memory space
 * @fd: file descriptor of ch36x device
 * @offset: memory space address offset
 * @odword: pointer to read dword
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_read_mem_dword(int fd, uint16_t offset, uint32_t *odword)
{
	struct ch36x_read_mem_t {
		uint16_t offset;
		uint32_t odword;
	} __attribute__((packed));

	struct ch36x_read_mem_t ch36x_read_mem;
	ch36x_read_mem.offset = offset;

	int ret = ioctl(fd, CH36x_READ_MEM_DWORD, (unsigned long)&ch36x_read_mem);
	if (ret >= 0)
		*odword = ch36x_read_mem.odword;

	return ret;
}

/**
 * ch36x_write_mem_byte - write one byte to mem space
 * @fd: file descriptor of ch36x device
 * @offset: memory space address offset
 * @ibyte: byte to write
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_write_mem_byte(int fd, uint16_t offset, uint8_t ibyte)
{
	struct ch36x_write_mem_t {
		uint16_t offset;
		uint8_t ibyte;
	} __attribute__((packed));

	struct ch36x_write_mem_t ch36x_write_mem;
	ch36x_write_mem.offset = offset;
	ch36x_write_mem.ibyte = ibyte;

	return ioctl(fd, CH36x_WRITE_MEM_BYTE, (unsigned long)&ch36x_write_mem);
}

/**
 * ch36x_write_mem_word - write one word to mem space
 * @fd: file descriptor of ch36x device
 * @offset: memory space address offset
 * @iword: word to write
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_write_mem_word(int fd, uint16_t offset, uint16_t iword)
{
	struct ch36x_write_mem_t {
		uint16_t offset;
		uint16_t iword;
	} __attribute__((packed));

	struct ch36x_write_mem_t ch36x_write_mem;
	ch36x_write_mem.offset = offset;
	ch36x_write_mem.iword = iword;

	return ioctl(fd, CH36x_WRITE_MEM_WORD, (unsigned long)&ch36x_write_mem);
}

/**
 * ch36x_write_mem_dword - write one dword to mem space
 * @fd: file descriptor of ch36x device
 * @offset: memory space address offset
 * @idword: dword to write
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_write_mem_dword(int fd, uint16_t offset, uint32_t idword)
{
	struct ch36x_write_mem_t {
		uint16_t offset;
		uint32_t idword;
	} __attribute__((packed));

	struct ch36x_write_mem_t ch36x_write_mem;
	ch36x_write_mem.offset = offset;
	ch36x_write_mem.idword = idword;

	return ioctl(fd, CH36x_WRITE_MEM_DWORD, (unsigned long)&ch36x_write_mem);
}

/**
 * ch36x_read_mem_block - read bytes from mem space
 * @fd: file descriptor of ch36x device
 * @type: SIZE_BYTE: 8-bit read, SIZE_DWORD: 32-bit read
 * @offset: memory space address offset
 * @obuffer: pointer to read buffer
 * @len: length to read
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_read_mem_block(int fd, uint8_t type, uint16_t offset, uint8_t *obuffer, uint32_t len)
{
	struct ch36x_read_mem_t {
		uint8_t type;
		uint16_t offset;
		uint32_t len;
		uint8_t data[0];
	} __attribute__((packed));

	if ((len <= 0) || (len > sizeof(mCH368_MEM_REG)))
		return -1;

	if ((type != SIZE_BYTE) && (type != SIZE_DWORD))
		return -1;

	struct ch36x_read_mem_t *ch36x_read_mem = malloc(sizeof(struct ch36x_read_mem_t) + len);
	ch36x_read_mem->type = type;
	ch36x_read_mem->offset = offset;
	ch36x_read_mem->len = len;

	int ret = ioctl(fd, CH36x_READ_MEM_BLOCK, (unsigned long)ch36x_read_mem);
	if (ret >= 0)
		memcpy(obuffer, ch36x_read_mem->data, len);

	free(ch36x_read_mem);
	return ret;
}

/**
 * ch36x_write_mem_block - write bytes to mem space
 * @fd: file descriptor of ch36x device
 * @type: SIZE_BYTE: 8-bit write, SIZE_DWORD: 32-bit write
 * @offset: memory space address offset
 * @ibuffer: pointer to write buffer
 * @len: length to write
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_write_mem_block(int fd, uint8_t type, uint16_t offset, uint8_t *ibuffer, uint32_t len)
{
	struct ch36x_write_mem_t {
		uint8_t type;
		uint16_t offset;
		uint32_t len;
		uint8_t data[0];
	} __attribute__((packed));

	if ((len <= 0) || (len > sizeof(mCH368_MEM_REG)))
		return -1;

	if ((type != SIZE_BYTE) && (type != SIZE_DWORD))
		return -1;

	struct ch36x_write_mem_t *ch36x_write_mem = malloc(sizeof(struct ch36x_write_mem_t) + len);
	ch36x_write_mem->type = type;
	ch36x_write_mem->offset = offset;
	ch36x_write_mem->len = len;
	memcpy(ch36x_write_mem->data, ibuffer, len);

	int ret = ioctl(fd, CH36x_WRITE_MEM_BLOCK, (unsigned long)ch36x_write_mem);

	free(ch36x_write_mem);
	return ret;
}

/**
 * ch36x_enable_isr - enable ch36x interrupt
 * @fd: file descriptor of ch36x device
 * @mode: interrupt mode
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_enable_isr(int fd, enum INTMODE mode)
{
	return ioctl(fd, CH36x_ENABLE_INT, (unsigned long)&mode);
}

/**
 * ch36x_disable_isr - disable ch36x interrupt
 * @fd: file descriptor of ch36x device
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_disable_isr(int fd)
{
	return ioctl(fd, CH36x_DISABLE_INT, NULL);
}

/**
 * ch36x_set_int_routine - set interrupt handler
 * @fd: file descriptor of ch36x device
 * @isr_handler: handler to call when interrupt occurs
 *
 */
void ch36x_set_int_routine(int fd, void *isr_handler)
{
	if (isr_handler != NULL)
		signal(SIGIO, isr_handler);
}

/**
 * ch36x_set_stream - set spi mode
 * @fd: file descriptor of ch36x device
 * @mode: bit0 on SPI Freq, 0->31.3MHz, 1->15.6MHz
 * 		  bit1 on SPI I/O Pinout, 0->SPI3(SCS/SCL/SDX), 1->SPI4(SCS/SCL/SDX/SDI)
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_set_stream(int fd, uint8_t mode)
{
	return ioctl(fd, CH36x_SET_STREAM, (unsigned long)&mode);
}

/**
 * ch36x_stream_spi - spi transfer
 * @fd: file descriptor of ch36x device
 * @ibuffer: spi buffer to write
 * @len: length to xfer
 * @obuffer: pointer to read buffer
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_stream_spi(int fd, uint8_t *ibuffer, uint32_t ilen, uint8_t *obuffer, uint32_t olen)
{
	struct ch36x_stream_spi_t {
		uint32_t ilen;
		uint32_t olen;
		uint8_t ibuffer[mMAX_BUFFER_LENGTH];
		uint8_t obuffer[mMAX_BUFFER_LENGTH];
	} __attribute__((packed));

	if ((ilen < 0) || (ilen > mMAX_BUFFER_LENGTH)) {
		return -1;
	}
	if ((olen < 0) || (olen > mMAX_BUFFER_LENGTH)) {
		return -1;
	}

	struct ch36x_stream_spi_t ch36x_stream_spi;
	ch36x_stream_spi.ilen = ilen;
	ch36x_stream_spi.olen = olen;
	memcpy(ch36x_stream_spi.ibuffer, ibuffer, ilen);

	int ret = ioctl(fd, CH36x_STREAM_SPI, (unsigned long)&ch36x_stream_spi);
	if (ret >= 0)
		memcpy(obuffer, ch36x_stream_spi.obuffer, olen);

	return ret;
}

/**
 * ch36x_flash_lock - lock/unlock spi flash
 * @fd: file descriptor of ch36x device
 * @lock: lock flag, 1 on lock, 0 on unlock
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_flash_lock(int fd, uint8_t lock)
{
	return ioctl(fd, CH36x_FLASH_LOCK, (unsigned long)&lock);
}

/**
 * ch36x_flash_erase - erase spi flash
 * @fd: file descriptor of ch36x device
 * @addr: spi flash address to erase
 * @ilen: length to erase
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_flash_erase(int fd, uint32_t addr, uint32_t ilen)
{
	struct ch36x_stream_spi_t {
		uint32_t addr;
		uint32_t ilen;
	} __attribute__((packed));

	if (ilen < 0)
		return -1;

	struct ch36x_stream_spi_t ch36x_stream_spi;
	ch36x_stream_spi.addr = addr;
	ch36x_stream_spi.ilen = ilen;

	return ioctl(fd, CH36x_FLASH_ERASE, (unsigned long)&ch36x_stream_spi);
}

/**
 * ch36x_flash_read - read spi flash
 * @fd: file descriptor of ch36x device
 * @addr: spi flash address to read
 * @obuffer: pointer to read buffer
 * @olen: length to xfer
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_flash_read(int fd, uint32_t addr, uint8_t *obuffer, uint32_t olen)
{
	struct ch36x_stream_spi_t {
		uint32_t addr;
		uint32_t olen;
		uint8_t data[0];
	} __attribute__((packed));

	if (olen < 0)
		return -1;

	struct ch36x_stream_spi_t *ch36x_stream_spi = malloc(sizeof(struct ch36x_stream_spi_t) + olen);
	ch36x_stream_spi->addr = addr;
	ch36x_stream_spi->olen = olen;

	int ret = ioctl(fd, CH36x_FLASH_READ, (unsigned long)ch36x_stream_spi);
	if (ret >= 0)
		memcpy(obuffer, ch36x_stream_spi->data, olen);

	free(ch36x_stream_spi);
	return ret;
}

/**
 * ch36x_flash_write - write spi flash
 * @fd: file descriptor of ch36x device
 * @addr: spi flash address to write
 * @ibuffer: pointer to write buffer
 * @ilen: length to xfer
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_flash_write(int fd, uint32_t addr, uint8_t *ibuffer, uint32_t ilen)
{
	struct ch36x_stream_spi_t {
		uint32_t addr;
		uint32_t ilen;
		uint8_t data[0];
	} __attribute__((packed));

	if (ilen < 0)
		return -1;

	struct ch36x_stream_spi_t *ch36x_stream_spi = malloc(sizeof(struct ch36x_stream_spi_t) + ilen);
	ch36x_stream_spi->addr = addr;
	ch36x_stream_spi->ilen = ilen;
	memcpy(ch36x_stream_spi->data, ibuffer, ilen);

	int ret = ioctl(fd, CH36x_FLASH_WRITE, (unsigned long)ch36x_stream_spi);

	free(ch36x_stream_spi);
	return ret;
}


int CH36X_SDA_GET(int fd)
{
    uint8_t obyte = 0;
    if(	0 == ch36x_read_io_byte(fd, offsetof(mCH367_IO_REG, mCH367GPIR), &obyte) )
        obyte &= CH367_GPOR_SET_SDA_BIT;
    return obyte;
}

int CH36X_SDA_SET(int fd)
{
    uint8_t obyte;
    int res = ch36x_read_io_byte(fd, offsetof(mCH367_IO_REG, mCH367GPOR), &obyte);
    if( !res ) // if success
        res = ch36x_write_io_byte(fd, offsetof(mCH367_IO_REG, mCH367GPOR), obyte | CH367_GPOR_SET_SDA_BIT);
    return res;
}

int CH36X_SDA_CLR(int fd)
{
    uint8_t obyte;
    int res = ch36x_read_io_byte(fd, offsetof(mCH367_IO_REG, mCH367GPOR), &obyte);
    if( !res ) // if success
        res = ch36x_write_io_byte(fd, offsetof(mCH367_IO_REG, mCH367GPOR), obyte & ~CH367_GPOR_SET_SDA_BIT);
    return res;
}

int CH36X_SCL_SET(int fd)
{
	uint8_t obyte;
    int res = ch36x_read_io_byte(fd, offsetof(mCH367_IO_REG, mCH367GPOR), &obyte);
    if( !res ) // if success
        res = ch36x_write_io_byte(fd, offsetof(mCH367_IO_REG, mCH367GPOR), obyte | CH367_GPOR_SET_SCL_BIT);
    return res;
}

int CH36X_SCL_CLR(int fd)
{
	uint8_t obyte;
    int res = ch36x_read_io_byte(fd, offsetof(mCH367_IO_REG, mCH367GPOR), &obyte);
    if( !res ) // if success
        res = ch36x_write_io_byte(fd, offsetof(mCH367_IO_REG, mCH367GPOR), obyte & ~CH367_GPOR_SET_SCL_BIT);
    return res;
}

#define I2C_DELAY_5US_N(N)  { if (N) usleep(N*5); }
#define I2C_DELAY_10MS_N(N) { if (N) usleep(N*10*1000); }

int I2C_Start(int fd)
{
    int        res = CH36X_SDA_SET(fd);
    if( !res ) res = CH36X_SCL_SET(fd);
    if( !res ) res = CH36X_SDA_CLR(fd);
    if( !res ) res = CH36X_SCL_CLR(fd);
    return res;
}

int I2C_Stop(int fd)
{
    int        res = CH36X_SDA_CLR(fd);
    if( !res ) res = CH36X_SCL_CLR(fd);
    if( !res ) res = CH36X_SCL_SET(fd);
    if( !res ) res = CH36X_SDA_SET(fd);
    return res;
}

int I2C_WaitAck(int fd)
{
    if( CH36X_SCL_SET(fd) )
        return -1;

    int cnt = 0;
    while( CH36X_SDA_GET(fd) )
    {
        if( cnt++ > 500 )
        {
			I2C_Stop(fd);
            return -2;
		}
        I2C_DELAY_5US_N(cnt);
    }

    return
        CH36X_SCL_CLR(fd) ? -3 : 0;
}

int I2C_SendAck(int fd)
{
    int        res = CH36X_SCL_CLR(fd);
    if( !res ) res = CH36X_SDA_CLR(fd);
    if( !res ) res = CH36X_SCL_SET(fd);
    if( !res ) res = CH36X_SCL_CLR(fd);
    if( !res ) res = CH36X_SDA_SET(fd);
    return res;
}

int I2C_SendNack(int fd)
{
    int        res = CH36X_SCL_CLR(fd);
    if( !res ) res = CH36X_SDA_SET(fd);
    if( !res ) res = CH36X_SCL_SET(fd);
    if( !res ) res = CH36X_SCL_CLR(fd);
    return res;
}

int I2C_SendByte(int fd, uint8_t ibyte)
{
    for( int bit = 0; bit < 8; bit++ )
    {
        if( CH36X_SCL_CLR(fd) )
            return -10-bit;
        if( ibyte & 0x80 )
        {
            if( CH36X_SDA_SET(fd) )
                return -20-bit;
        }
		else
        {
            if( CH36X_SDA_CLR(fd) )
                return -30-bit;
        }
		ibyte <<= 1;
        if( CH36X_SCL_SET(fd) )
            return -40-bit;
	}
    return
        CH36X_SCL_CLR(fd);
}

int I2C_ReadByte(int fd, uint8_t ack, uint8_t *obyte)
{
    *obyte = 0;
    for (int bit = 0; bit < 8; bit++)
    {
        *obyte <<= 1;
        if( CH36X_SCL_CLR(fd) )
            return -10-bit;
        if( CH36X_SCL_SET(fd) )
            return -20-bit;
        if( CH36X_SDA_GET(fd) )
            (*obyte)++;
    }
    return
        ack ? I2C_SendAck(fd) : I2C_SendNack(fd);
}

/**
 * ch36x_i2c_write - write i2c data
 * @fd: file descriptor of ch36x device
 * @addr: i2c device address(low 7 bits specified)
 * @reg: i2c data unit address
 * @ibuffer: data to write
 * @ilen: pointer to length to write
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_i2c_write(int fd, uint8_t addr, uint8_t reg, uint8_t *ibuffer, uint32_t ilen)
{
	/* issue i2c start */
    if( I2C_Start(fd) )
        return -1;

	/* write device address with write flag*/
    if( I2C_SendByte(fd, (addr << 1) | 0x00) )
    {
        I2C_Stop(fd);
        return -2;
    }

	/* issue 9th clock to check ack */
    if( I2C_WaitAck(fd) )
    {
		I2C_Stop(fd);
        return -3;
    }

	/* write i2c data unit address */
    if( I2C_SendByte(fd, reg) )
    {
        I2C_Stop(fd);
        return -4;
    }

	/* issue 9th clock to check ack */
    if( I2C_WaitAck(fd) )
    {
		I2C_Stop(fd);
        return -5;
    }

	/* write data */
	uint8_t ibyte;
    for( int i = 0; i < ilen; i++ )
    {
		ibyte = ibuffer[i];

		/* send one byte */
        if( I2C_SendByte(fd, ibyte) )
        {
            I2C_Stop(fd);
            return -100-i;
        }

		/* issue 9th clock to check ack */
        if( I2C_WaitAck(fd) )
        {
			I2C_Stop(fd);
            return -200-i;
        }
	}

	/* issue i2c stop */
	I2C_Stop(fd);

    /* delay after write */
    I2C_DELAY_10MS_N(ilen);

    return 0;
}

/**
 * ch36x_i2c_read - read i2c data
 * @fd: file descriptor of ch36x device
 * @addr: i2c device address(low 7 bits specified)
 * @reg: i2c data unit address
 * @obuffer: pointer to read data
 * @olen: pointer to length to read
 *
 * The function return 0 if success, others if fail.
 */
int ch36x_i2c_read(int fd, uint8_t addr, uint8_t reg, uint8_t *obuffer, uint32_t olen)
{
	/* issue i2c start */
    if( I2C_Start(fd) )
        return -1;

	/* write device address with write flag*/
    if( I2C_SendByte(fd, (addr << 1) | 0x00) )
    {
        I2C_Stop(fd);
        return -2;
    }

	/* issue 9th clock to check ack */
    if( I2C_WaitAck(fd) )
    {
		I2C_Stop(fd);
        return -3;
    }

	/* write i2c data unit address */
    if( I2C_SendByte(fd, reg) )
    {
        I2C_Stop(fd);
        return -4;
    }

	/* issue 9th clock to check ack */
    if( I2C_WaitAck(fd) )
    {
		I2C_Stop(fd);
        return -5;
    }

    /* issue i2c repeat start */
    if( I2C_Start(fd) )
    {
        I2C_Stop(fd);
        return -6;
    }

    /* write device address with read flag*/
    if( I2C_SendByte(fd, (addr << 1) | 0x01) )
    {
        I2C_Stop(fd);
        return -7;
    }

    /* issue 9th clock to check ack */
    if( I2C_WaitAck(fd) )
    {
        I2C_Stop(fd);
        return -8;
    }

	/* read data */
    for( int i = 0; i < olen; i++ )
    {
		/* the last byte has been readed, send nack to device, else send ack */
        uint8_t ack = (i == (olen - 1)) ? 0 : 1;
        if( I2C_ReadByte(fd, ack, obuffer++) )
        {
            I2C_Stop(fd);
            return -100-i;
        }
    }

	/* issue i2c stop */
	I2C_Stop(fd);

    /* delay after read */
    I2C_DELAY_10MS_N(olen);

    return 0;
}
