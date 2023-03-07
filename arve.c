#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


static uint32_t imem[1000] = {0}; // TODO
static int imem_nb = 0;


// start must be <= stop
// GET_BITS(0x0420, 4, 8) == 0x42
#define GET_BITS(val, start, stop) (((val) >> (start)) & ((1u << ((stop) - (start) + 1)) - 1))

#define PARSE_R \
    uint32_t __attribute__((__unused__)) rd = GET_BITS(ins, 7, 11); \
    uint32_t __attribute__((__unused__)) func3 = GET_BITS(ins, 12, 14); \
    uint32_t __attribute__((__unused__)) rs1 = GET_BITS(ins, 15, 19); \
    uint32_t __attribute__((__unused__)) rs2 = GET_BITS(ins, 20, 24); \
    uint32_t __attribute__((__unused__)) func7 = GET_BITS(ins, 25, 31)

#define PARSE_I \
    uint32_t __attribute__((__unused__)) rd = GET_BITS(ins, 7, 11); \
    uint32_t __attribute__((__unused__)) func3 = GET_BITS(ins, 12, 14); \
    uint32_t __attribute__((__unused__)) rs1 = GET_BITS(ins, 15, 19); \
    uint32_t __attribute__((__unused__)) imm = GET_BITS(ins, 20, 31)


void dump_prog(void) {
    for(int i=0; i<imem_nb; i++) {
        uint32_t ins = imem[i];
        printf("%.8x: %.8x ", i*4, ins);

        uint32_t opcode = ins & GET_BITS(ins, 0, 6);
        if (opcode == 0b0110011) {
            PARSE_R;

            printf("r%u = r%u + r%d\n", rd, rs1, rs2);
        }
        else if (opcode == 0b0010011) {
            PARSE_I;

            // Sign extend
            uint32_t immu;
            if (GET_BITS(imm, 11, 11) == 1) {
                immu = 0xFFFFF000u | imm;
            } else {
                immu = imm;
            }
            int32_t imm32 = *((int32_t*) &immu);

            printf("r%u = r%u + %d\n", rd, rs1, imm32);
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

