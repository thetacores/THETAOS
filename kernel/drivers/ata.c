/* SPDX-License-Identifier: GPL-2.0 */
/*
 * ata.c - ATA PIO disk driver (primary bus, polling, LBA28, 16-bit data bus)
 */

#include <drivers/ata.h>
#include <cpu/io.h>
#include <klib/kprintf.h>
#include <klib/string.h>

#include <stdint.h>

/* Primary ATA bus I/O ports */
#define ATA_DATA        0x1F0
#define ATA_ERROR       0x1F1
#define ATA_SECTOR_CNT  0x1F2
#define ATA_LBA_LO      0x1F3
#define ATA_LBA_MID     0x1F4
#define ATA_LBA_HI      0x1F5
#define ATA_DRIVE_HEAD  0x1F6
#define ATA_STATUS      0x1F7
#define ATA_CMD         0x1F7
#define ATA_CTRL        0x3F6

/* Status bits */
#define ATA_SR_BSY      0x80
#define ATA_SR_DRQ      0x08
#define ATA_SR_ERR      0x01

/* LBA28 commands */
#define ATA_CMD_READ    0x20
#define ATA_CMD_WRITE   0x30

/* Poll until BSY clears; return -1 on error. */
static int ata_poll(void)
{
	uint8_t status;
/* 400ns delay: read alt-status 4 times */
	for (int i = 0; i < 4; i++)
		inb(ATA_CTRL);

	do {
		status = inb(ATA_STATUS);
		if (status & ATA_SR_ERR)
			return -1;
	} while (status & ATA_SR_BSY);

	return 0;
}

/* LBA28: bits 24-27 go into DRIVE_HEAD low nibble */
static void ata_setup_lba28(uint32_t lba)
{
	outb(ATA_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F)); /* LBA, master, bits 24-27 */
	outb(ATA_SECTOR_CNT, 1);
	outb(ATA_LBA_LO,     (uint8_t)(lba & 0xFF));
	outb(ATA_LBA_MID,    (uint8_t)((lba >> 8)  & 0xFF));
	outb(ATA_LBA_HI,     (uint8_t)((lba >> 16) & 0xFF));
}

void ata_init(void)
{
	outb(ATA_CTRL, 0x04);
	outb(ATA_CTRL, 0x00);

	outb(ATA_DRIVE_HEAD, 0xE0);
	ata_poll();

	kprintf("[ATA] ATA PIO driver initialized (LBA28, 16-bit)\n");
}

int ata_read_sector(uint32_t lba, uint16_t *buf)
{
	if (ata_poll() < 0)
		return -1;

	ata_setup_lba28(lba);
	outb(ATA_CMD, ATA_CMD_READ);

	if (ata_poll() < 0)
		return -1;

	while (!(inb(ATA_STATUS) & ATA_SR_DRQ))
		;

	for (int i = 0; i < 256; i++)
		buf[i] = inw(ATA_DATA);

	return 0;
}

int ata_write_sector(uint32_t lba, const uint16_t *buf)
{
	if (ata_poll() < 0)
		return -1;

	ata_setup_lba28(lba);
	outb(ATA_CMD, ATA_CMD_WRITE);

	if (ata_poll() < 0)
		return -1;

	while (!(inb(ATA_STATUS) & ATA_SR_DRQ))
		;

	for (int i = 0; i < 256; i++)
		outw(ATA_DATA, buf[i]);

	outb(ATA_CMD, 0xE7); /* CACHE FLUSH */
	ata_poll();

	return 0;
}

int ata_write_string(uint32_t lba, char *buffer)
{
	uint16_t sector[256];

	memset(sector, 0, sizeof(sector));
	strcpy((char *)sector, buffer);
	return ata_write_sector(lba, sector);
}

int ata_read_string(uint32_t lba, char *buffer)
{
	uint16_t sector[256];

	int status = ata_read_sector(lba, sector);
	if (status == 0)
		strcpy(buffer, (char *)sector);
	return status;
}

void shell_ata_write(int argc, char *argv[])
{
	if (argc < 3) {
		kprintf("USAGE INVALID : usage -> write {lba} {string}\n");
		return;
	}

	char buffer[512];
	int bi = 0;
	int lba_int = atoi(argv[1]);

	for (int i = 2; i < argc; i++)
	{
		char *p = argv[i];
		while (*p && bi < 511)
		{
			buffer[bi++] = *p++;
		}

		if (i < argc - 1 && bi < 511)
		{
			buffer[bi++] = ' ';
		}
	}
	buffer[bi] = '\0';

	kprintf("Writing to LBA %d: %s\n", lba_int, buffer);


	if (ata_write_string(lba_int, buffer) == 0)
		kprintf("ATA write OK\n");
	else
		kprintf("ATA write FAILED\n");

}

void shell_ata_read(int argc, char *argv[])
{

	if (argc < 2) {
		kprintf("USAGE INVALID : usage -> write {lba} {string}\n");
		return;
	}

	char buffer[512];

	int lba_int = atoi(argv[1]);

	if (ata_read_string(lba_int, buffer) == 0)
		kprintf("ATA read sector %d : %s\n", lba_int, buffer);
	else
		kprintf("ATA read sector %d: FAILED\n", lba_int);
}
