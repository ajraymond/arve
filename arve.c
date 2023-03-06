#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void usage(const char *argv)
{
    printf("Usage: %s <program.bin>\n", argv);
    printf("    -h: Print this message\n");
}

int main(int argc, char* argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
            case 'h':
            default:
                usage(argv[0]);
                exit(1);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "ERROR: Missing program name\n");
        exit(1);
    }

    return 0;
}

