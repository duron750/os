#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    uint64 free = freemem();
    uint64 total = totalmem();

    fprintf(2, "\ttotal\tused\tfree\n");
    fprintf(2, "Mem:\t");
    if(argc < 2) {
        fprintf(2, "%ld\t%ld\t%ld\n", total, total-free, free);
    } else if(strcmp(argv[1], "-b") == 0) {
        fprintf(2, "%ld\t%ld\t%ld\n", total, total-free, free);
    } else if(strcmp(argv[1], "-k") == 0) {
        fprintf(2, "%ld\t%ld\t%ld\n", total / 1024, (total - free) / 1024, free / 1024);
    } else if(strcmp(argv[1], "-m") == 0) {
        fprintf(2, "%ld\t%ld\t%ld\n", total / (1024 * 1024), (total - free) / (1024 * 1024), free / (1024 * 1024));
    } else {
        fprintf(2, "Usage: freemem [-b|-k|-m]\n");
    }

    exit(0);
}
