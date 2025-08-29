#ifndef ATA_H
#define ATA_H

#include <types.h>
#include <x86.h>

#define ATA_SECTOR_SIZE 512

// I/O Ports for the Primary ATA Bus
#define ATA_PRIMARY_DATA         0x1F0
#define ATA_PRIMARY_ERROR        0x1F1
#define ATA_PRIMARY_FEATURES     0x1F1
#define ATA_PRIMARY_SECCOUNT     0x1F2
#define ATA_PRIMARY_LBA_LO       0x1F3
#define ATA_PRIMARY_LBA_MID      0x1F4
#define ATA_PRIMARY_LBA_HI       0x1F5
#define ATA_PRIMARY_DRIVE_HEAD   0x1F6
#define ATA_PRIMARY_STATUS       0x1F7
#define ATA_PRIMARY_COMMAND      0x1F7
#define ATA_PRIMARY_ALT_STATUS   0x3F6
#define ATA_PRIMARY_DEV_CONTROL  0x3F6

// Status Register Bits
#define ATA_SR_BSY  0x80 // Busy
#define ATA_SR_DRDY 0x40 // Drive Ready
#define ATA_SR_DF   0x20 // Drive Fault
#define ATA_SR_DSC  0x10 // Seek Complete
#define ATA_SR_DRQ  0x08 // Data Request
#define ATA_SR_CORR 0x04 // Corrected Data
#define ATA_SR_IDX  0x02 // Index
#define ATA_SR_ERR  0x01 // Error

// Error Register Bits
#define ATA_ER_BBK   0x80 // Bad Block
#define ATA_ER_UNC   0x40 // Uncorrectable Data
#define ATA_ER_MC    0x20 // Media Changed
#define ATA_ER_IDNF  0x10 // ID Not Found
#define ATA_ER_MCR   0x08 // Media Change Requested
#define ATA_ER_ABRT  0x04 // Command Aborted
#define ATA_ER_TK0NF 0x02 // Track 0 Not Found
#define ATA_ER_AMNF  0x01 // Address Mark Not Found

// ATA Commands
#define ATA_CMD_READ_PIO      0x20
#define ATA_CMD_WRITE_PIO     0x30
#define ATA_CMD_CACHE_FLUSH   0xE7
#define ATA_CMD_IDENTIFY      0xEC

// Drive selection
#define ATA_MASTER_DRIVE 0xE0
#define ATA_SLAVE_DRIVE  0xF0

/*
    Initializes the ATA driver.
*/
void ata_init();

/*
    Reads 'count' sectors from the ATA drive into the buffer 'buf'.
    'lba' is the 28-bit Linear Block Address to start reading from.
    This is a blocking operation.
*/
void ata_read_sectors(UINT lba, UC count, void* buf);

/*
    Writes 'count' sectors to the ATA drive from the buffer 'buf'.
    'lba' is the 28-bit Linear Block Address to start writing to.
    This is a blocking operation.
*/
void ata_write_sectors(UINT lba, UC count, const void* buf);

#endif // ATA_H