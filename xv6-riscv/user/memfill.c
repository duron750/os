// memfill.c - xv6-riscv user program
// Usage: memfill -k <kilobytes>
// Allocates memory via sbrk() and fills every byte with 0xFF.
//
// To add to xv6: copy to user/memfill.c
// Add _memfill to UPROGS in Makefile

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    // ---- 1. Parse arguments -----------------------------------------------
    if (argc != 3) {
        fprintf(2, "Usage: memfill -k <kilobytes>\n");
        exit(1);
    }

    if (argv[1][0] != '-' || argv[1][1] != 'k' || argv[1][2] != '\0') {
        fprintf(2, "Error: unknown flag '%s', expected -k\n", argv[1]);
        exit(1);
    }

    int kb = atoi(argv[2]);

    if (kb <= 0) {
        fprintf(2, "Error: kilobytes must be a positive integer\n");
        exit(1);
    }

    // ---- 2. Calculate bytes -----------------------------------------------
    int bytes = kb * 1024;

    printf("memfill: requesting %d KB (%d bytes) via sbrk()...\n", kb, bytes);

    // ---- 3. Call sbrk() ---------------------------------------------------
    //
    // sbrk(n) grows the process heap by n bytes and returns the
    // OLD break (= start address of the newly allocated region).
    // Returns (char*)-1 on failure.
    //
    char *mem = sbrk(bytes);

    if (mem == (char*) -1) {
        fprintf(2, "Error: sbrk() failed — not enough memory\n");
        exit(1);
    }

    printf("memfill: got %d bytes at address 0x%p\n", bytes, mem);

    // ---- 4. Fill every byte with 0xFF ------------------------------------
    //
    // memset() is available in xv6 user/ulib.c
    // It walks the region writing the given byte value to every slot.
    //
    memset(mem, 0xFF, bytes);

    // ---- 5. Verify a sample -----------------------------------------------
    //
    // Spot-check the first and last byte to confirm writes landed.
    //
    if ((unsigned char)mem[0] != 0xFF || (unsigned char)mem[bytes-1] != 0xFF) {
        fprintf(2, "memfill: WARNING — verification failed!\n");
        exit(1);
    }

    printf("memfill: done. %d KB filled with 0xFF.\n", kb);
    // Hold memory until user presses a key.
    // read() on fd 0 (stdin) blocks the process while keeping
    // the sbrk() allocation alive — useful for observing memory
    // pressure from another xv6 shell.
    printf("memfill: memory held. Press any key to release and exit...\n");
    char buf[1];
    read(0, buf, 1);

    exit(0);
}
