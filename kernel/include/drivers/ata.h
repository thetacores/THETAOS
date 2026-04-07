/* SPDX-License-Identifier: GPL-2.0 */
/*
 * ata.h - ATA PIO driver interface
 */

#ifndef ATA_H
#define ATA_H

#include <stdint.h>

void ata_init(void);
int  ata_read_sector(uint32_t lba, uint16_t *buf);   /* 256 words = 512 bytes */
int  ata_write_sector(uint32_t lba, const uint16_t *buf);
int  ata_write_string(uint32_t lba, char *buffer);
int  ata_read_string(uint32_t lba, char *buffer);

void shell_ata_write(int argc, char *argv[]);
void shell_ata_read(int argc, char *argv[]);

#endif
