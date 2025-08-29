#ifndef IDT_H
#define IDT_H

#include <types.h>

// Defines an entry in the Interrupt Descriptor Table.
// The __attribute__((packed)) is important to prevent the compiler from adding padding.
struct idt_entry {
    USH base_lo; // The lower 16 bits of the ISR's address
    USH sel;     // The kernel segment selector
    UC always0;     // This must always be zero
    UC flags;       // More flags, like ring level and present bit
    USH base_hi; // The upper 16 bits of the ISR's address
} __attribute__((packed));

// A pointer to the array of interrupt handlers.
// This format is required by the 'lidt' instruction.
struct idt_ptr {
    USH limit;
    SIZE base;      // The address of the first element in idt_entry array
} __attribute__((packed));


// Initializes the IDT and remaps the PIC.
void idt_init();

#endif // IDT_H