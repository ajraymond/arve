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


enum {
    X0,
    X1,
    X2,
    X3,
    X4,
    X5,
    X6,
    X7,
    X8,
    X9,
    X10,
    X11,
    X12,
    X13,
    X14,
    X15,
    X16,
    X17,
    X18,
    X19,
    X20,
    X21,
    X22,
    X23,
    X24,
    X25,
    X26,
    X27,
    X28,
    X29,
    X30,
    X31,
    PC,
    //
    REGISTERS_NB,
};

static uint64_t R[REGISTERS_NB] = {0};


void dump_registers(void)
{
    fprintf(stderr,
            "x0=%.16lx "
            "x1=%.16lx "
            "x2=%.16lx "
            "x3=%.16lx "
            "x4=%.16lx "
            "x5=%.16lx "
            "x6=%.16lx "
            "x7=%.16lx "
            "x8=%.16lx "
            "x9=%.16lx "
            "x10=%.16lx "
            "x11=%.16lx "
            "x12=%.16lx "
            "x13=%.16lx "
            "x14=%.16lx "
            "x15=%.16lx "
            "x16=%.16lx "
            "x17=%.16lx "
            "x18=%.16lx "
            "x19=%.16lx "
            "x20=%.16lx "
            "x21=%.16lx "
            "x22=%.16lx "
            "x23=%.16lx "
            "x24=%.16lx "
            "x25=%.16lx "
            "x26=%.16lx "
            "x27=%.16lx "
            "x28=%.16lx "
            "x29=%.16lx "
            "x30=%.16lx "
            "x31=%.16lx\n",
            R[X0], R[X1], R[X2], R[X3], R[X4], R[X5], R[X6], R[X7], R[X8], R[X9],
            R[X10], R[X11], R[X12], R[X13], R[X14], R[X15], R[X16], R[X17], R[X18], R[X19],
            R[X20], R[X21], R[X22], R[X23], R[X24], R[X25], R[X26], R[X27], R[X28], R[X29],
            R[X30], R[X31]);
}

void run_prog(void) {
    for(int i=0; i<imem_nb; i++) {
        uint32_t ins = imem[i];
        printf("%.8x: %.8x ", i*4, ins);

        dump_registers();
        uint32_t opcode = ins & GET_BITS(ins, 0, 6);
        if (opcode == 0b0110011) {
            PARSE_R;

            if (func3 == 0b000) {
                // ADD
                printf("x%u = x%u + x%d\n", rd, rs1, rs2);
                R[rd] = R[rs1] + R[rs2];
            } else {
                // AND
                printf("and x%u, x%u, x%u\n", rd, rs1, rs2);
                R[rd] = R[rs1] & R[rs2];
            }
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

            printf("x%u = x%u + %d\n", rd, rs1, imm32);
            R[rd] = R[rs1] + imm32;
        }
        else {
            printf("[???]\n");
        }

        // Special zero register
        R[X0] = 0;

        dump_registers();
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
    run_prog();

    return 0;
}

