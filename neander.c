#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MEMORYSIZE 516
#define LINESIZE 16
#define HEADERSIZE 4
#define RESULTOFFSET 0x202

void print_memory(uint8_t *bytes, size_t size) {
    size_t offset = 0;
    while (offset < size) {
        printf("%08zx: ", offset);
        for (size_t i = 0; i < LINESIZE; i++) {
            if (offset + i < size)
                printf("%02x ", bytes[offset + i]);
            else
                printf("   ");
        }
        printf("\n");
        offset += LINESIZE;
    }
}

int main() {
    uint8_t ac = 0, pc = 0;
    bool z = 0, n = 0;
    FILE *file = fopen("multiplicacao_entrega_corrigida.mem", "rb");
    if (!file) {
        perror("Can't open the file");
        return 1;
    }
    uint8_t bytes[MEMORYSIZE] = {0};
    uint8_t fileHeader[HEADERSIZE];
    fread(fileHeader, 1, HEADERSIZE, file);
    const uint8_t expectedHeader[] = {0x03, 0x4E, 0x44, 0x52};
    if (memcmp(fileHeader, expectedHeader, HEADERSIZE) != 0) {
        printf("Wrong headers!");
        fclose(file);
        return 1;
    }
    fread(bytes + HEADERSIZE, 1, MEMORYSIZE - HEADERSIZE, file);
    fclose(file);
    print_memory(bytes, MEMORYSIZE);
    
    while (bytes[pc] != 0xF0) {
        z = (ac == 0);
        n = (ac & 0x80) != 0;
        uint16_t address = bytes[pc + 2] * 2 + HEADERSIZE;
        switch (bytes[pc]) {
            case 0x00:
                break;
            case 0x10:
                bytes[address] = ac;
                break;
            case 0x20:
                ac = bytes[address];
                break;
            case 0x30:
                ac += bytes[address];
                break;
            case 0x40:
                ac |= bytes[address];
                break;
            case 0x50:
                ac &= bytes[address];
                break;
            case 0x60:
                ac = ~ac;
                pc += 2;
                continue;
            case 0x80:
                pc = address;
                continue;
            case 0x90:
                if (n) {
                    pc = address;
                    continue;
                }
                break;
            case 0xA0:
                if (z) {
                    pc = address;
                    continue;
                }
                break;
        }
        pc += 4;
    }
    
    print_memory(bytes, MEMORYSIZE);
    
    uint8_t rawResult = bytes[RESULTOFFSET];
    int8_t result = (int8_t)rawResult;
    
    printf("The answer is: 0x%02X (hexadecimal) and = %d decimal\n", 
      rawResult, result);
    
    return 0;
}