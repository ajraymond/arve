#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


static uint32_t imem[1000] = {0}; // TODO
static int imem_nb = 0;


// start must be <= stop
// GET_BITS(0x0420, 4, 8) == 0x42
#define GET_BITS(val, start, stop) (((val) >> (start)) & ((1u << ((stop) - (start) + 1)) - 1))

void dump_prog(void) {
    for(int i=0; i<imem_nb; i++) {
        uint32_t ins = imem[i];
        printf("%.8x: %.8x ", i*4, ins);

        uint32_t itype = ins & 0x7F;
        printf("T=%.2x ", itype);
        if (itype == 0b0010011) {
            uint32_t rd = GET_BITS(ins, 7, 11);
            uint32_t rs1 = GET_BITS(ins, 15, 19);
            uint32_t immu12 = GET_BITS(ins, 20, 31);

            uint32_t immu;
            if (GET_BITS(immu12, 11, 11) == 1) {
                immu = 0xFFFFF000u | immu12;
            } else {
                immu = immu12;
            }
            int32_t imm = *((int32_t*) &immu);

            printf("[ADDI] r%u <- r%u + %d\n", rd, rs1, imm);
        }
        else {
            printf("[???]\n");
        }
    }
}

void read_prog(const char *prog_file)
{
    FILE *prog_fd = fopen(prog_file, "r");
    if (! prog_fd) {
        perror("Unable to open program file");
        exit(1);
    }

    while(!feof(prog_fd)) {
        uint32_t ins;
        //TODO: deal with endianness
        size_t ins_read = fread(&ins, sizeof(ins), 1, prog_fd);
        if (ins_read == 0) {
            break;
        }
        else if (ins_read != 1) {
            fprintf(stderr, "Too few bytes read for instruction (%zd)\n", ins_read);
            break;
        }
        imem[imem_nb++] = ins;
    }

    fclose(prog_fd);
} 

void usage(const char *argv)
{
    printf("Usage: %s <program.bin>\n", argv);
    printf("    -h: Print this message\n");
}

int main(int argc, char *argv[])
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
    const char *prog_file = argv[optind];
    printf("Program name: %s\n", prog_file);

    read_prog(prog_file);
    dump_prog();

    return 0;
}

