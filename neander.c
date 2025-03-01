#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MEMORY_SIZE 516
#define LINE_SIZE 16
#define HEADER_SIZE 4

void print_memory(uint8_t *bytes, size_t size) {
    size_t offset = 0;
  
    while (offset < size) {
      printf("%08zx: ", offset);
  
      for (size_t i = 0; i < LINE_SIZE; i++) {
        if (offset + i < size)
          printf("%02x ", bytes[offset + i]);
        else
          printf("   ");
      }
  
      printf("\n");
      offset += LINE_SIZE;
    }
  }
  

int main() {
  uint8_t ac = 0, pc = 0;
  bool z = 0, n = 0;

  FILE *file = fopen("multiplicacao_entrega.mem", "rb");
  if (!file) {
    perror("Can't open the file");
    return 1;
  }

  uint8_t bytes[MEMORY_SIZE] = {0}; 
  uint8_t fileHeader[HEADER_SIZE];
  fread(fileHeader, 1, HEADER_SIZE, file);

  const uint8_t expectedHeader[] = {0x03, 0x4E, 0x44, 0x52};

  if (memcmp(fileHeader, expectedHeader, HEADER_SIZE) != 0) {
    printf("Wrong headers!");
    fclose(file);
    return 1;
}


  fread(bytes + HEADER_SIZE, 1, MEMORY_SIZE - HEADER_SIZE, file);
  fclose(file);

  print_memory(bytes, MEMORY_SIZE);

  while (bytes[pc] != 0xF0) {

    z = (ac == 0);
    n = (ac & 0x80) != 0;

    uint16_t address = bytes[pc + 2] * 2 + HEADER_SIZE;

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

  print_memory(bytes, MEMORY_SIZE);

  return 0;
}
