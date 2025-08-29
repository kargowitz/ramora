#include <fs/ata/ata.h>
#include <stddef.h>  // For NULL

// Helper function to wait for the ATA drive to be ready (BSY bit to be clear)
static void ata_wait_busy() {
    // Poll the alternate status port until BSY is clear
    while (inb(ATA_PRIMARY_ALT_STATUS) & ATA_SR_BSY);
}

// Helper function to wait for the drive to be ready for data transfer (DRQ bit to be set)
static void ata_wait_drq() {
    ata_wait_busy();
    // Poll the status port until DRQ is set or an error occurs
    while (!((inb(ATA_PRIMARY_STATUS) & ATA_SR_DRQ) || (inb(ATA_PRIMARY_STATUS) & ATA_SR_ERR)));
}

void ata_init() {
    // Select the master drive on the primary bus
    outb(ATA_PRIMARY_DRIVE_HEAD, ATA_MASTER_DRIVE);
    // A 400ns delay is required after a drive select.
    // Reading the status port 4 times is a common way to achieve this.
    inb(ATA_PRIMARY_ALT_STATUS);
    inb(ATA_PRIMARY_ALT_STATUS);
    inb(ATA_PRIMARY_ALT_STATUS);
    inb(ATA_PRIMARY_ALT_STATUS);
    ata_wait_busy();
}

void ata_read_sectors(UINT lba, UC count, void* buf) {
    if (count == 0) return;

    ata_wait_busy();

    // Setup the command
    // Select master drive, and send high 4 bits of LBA
    outb(ATA_PRIMARY_DRIVE_HEAD, ATA_MASTER_DRIVE | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_FEATURES, 0x00); // No features
    outb(ATA_PRIMARY_SECCOUNT, count);
    outb(ATA_PRIMARY_LBA_LO, (UC)lba);
    outb(ATA_PRIMARY_LBA_MID, (UC)(lba >> 8));
    outb(ATA_PRIMARY_LBA_HI, (UC)(lba >> 16));
    outb(ATA_PRIMARY_COMMAND, ATA_CMD_READ_PIO);

    // Read the data
    for (int i = 0; i < count; i++) {
        ata_wait_drq();
        // A sector is 512 bytes, which is 128 * 4 bytes (longs)
        // Use insl to read 128 longs from the data port
        insl(ATA_PRIMARY_DATA, (char*)buf + (i * ATA_SECTOR_SIZE), 128);
    }
}

void ata_write_sectors(UINT lba, UC count, const void* buf) {
    if (count == 0) return;

    ata_wait_busy();

    // Setup the command
    outb(ATA_PRIMARY_DRIVE_HEAD, ATA_MASTER_DRIVE | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_FEATURES, 0x00);
    outb(ATA_PRIMARY_SECCOUNT, count);
    outb(ATA_PRIMARY_LBA_LO, (UC)lba);
    outb(ATA_PRIMARY_LBA_MID, (UC)(lba >> 8));
    outb(ATA_PRIMARY_LBA_HI, (UC)(lba >> 16));
    outb(ATA_PRIMARY_COMMAND, ATA_CMD_WRITE_PIO);

    // Write the data
    for (int i = 0; i < count; i++) {
        ata_wait_drq();
        // A sector is 512 bytes, which is 128 * 4 bytes (longs)
        // Use outsl to write 128 longs to the data port
        outsl(ATA_PRIMARY_DATA, (char*)buf + (i * ATA_SECTOR_SIZE), 128);
    }

    // Flush the cache to ensure data is written to disk
    outb(ATA_PRIMARY_COMMAND, ATA_CMD_CACHE_FLUSH);
    ata_wait_busy();
}