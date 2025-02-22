#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define MEM_SIZE 516
#define HALT_OPCODE 0xF0

void printMemory(const uint8_t *bytes, int size, uint8_t ac, uint8_t pc) {
    size_t offset = 0;

    while (offset < size) {
        printf("%08zx: ", offset);

        for (size_t i = 0; i < 16; i++) {
            if (offset + i < size)
                printf("%02x ", bytes[offset + i]);
            else
                printf("   ");
        }

        printf("\n");
        offset += 16;
    }

    printf("AC: %d \nPC: %d\n\n", ac, pc);
}

int main(int argc, char const *argv[]) {
    uint8_t ac = 0, pc = 0;
    bool z = 0, n = 0;

    FILE *file = fopen("soma.mem", "rb");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    uint8_t bytes[MEM_SIZE];
    fread(bytes, 1, MEM_SIZE, file);
    fclose(file);

    printMemory(bytes, MEM_SIZE, ac, pc);

    while (bytes[pc] != HALT_OPCODE) {
        switch (bytes[pc]) {
            case 0x00:
                break;
            case 0x10:
                bytes[bytes[pc + 2] * 2 + 4] = ac;
                break;
            case 0x20:
                ac = bytes[bytes[pc + 2] * 2 + 4];
                break;
            case 0x30:
                ac += bytes[bytes[pc + 2] * 2 + 4];
                break;
            case 0x40:
                ac += bytes[bytes[pc + 2] * 2 + 4] || ac;
                break;
            case 0x50:
                ac += bytes[bytes[pc + 2] * 2 + 4] && ac;
                break;
            case 0x60:
                ac += ~ac;
                break;
            case 0x80:
                pc = bytes[pc + 2] * 2 + 4;
                break;
            case 0x90:
                if (n)
                    pc = bytes[bytes[pc + 2] * 2 + 4];
                break;
            case 0xA0:
                if (z)
                    pc = bytes[bytes[pc + 2] * 2 + 4];
                break;
        }

        z = (ac == 0);
        n = (ac & 0x80) != 0; // Verifica o bit mais significativo para determinar se ac é negativo
        pc += 4;
    }

    printMemory(bytes, MEM_SIZE, ac, pc);

    return 0;
}
