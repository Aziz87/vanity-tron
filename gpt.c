#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>

#define ADDRESS_LENGTH 42

__global__ void generateAddresses(char *addresses, int prefixLength, int suffixLength) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    while (tid < gridDim.x * blockDim.x) {
        // Генерация случайного адреса кошелька TRON
        char address[ADDRESS_LENGTH];
        for (int i = 0; i < ADDRESS_LENGTH; i++) {
            address[i] = 'a' + rand() % 26;
        }

        // Проверка совпадения заданных символов в начале и в конце
        int prefixCount = 0;
        int suffixCount = 0;
        for (int i = 0; i < prefixLength; i++) {
            if (address[i] == 'a') {
                prefixCount++;
            }
        }
        for (int i = ADDRESS_LENGTH - 1; i >= ADDRESS_LENGTH - suffixLength; i--) {
            if (address[i] == 'a') {
                suffixCount++;
            }
        }
        if (prefixCount == prefixLength && suffixCount == suffixLength) {
            // Запись совпавшего адреса в массив
            for (int i = 0; i < ADDRESS_LENGTH; i++) {
                addresses[tid * ADDRESS_LENGTH + i] = address[i];
            }
        }

        tid += gridDim.x * blockDim.x;
    }
}

int main(int argc, char **argv) {
    int prefixLength = atoi(argv[1]);
    int suffixLength = atoi(argv[2]);
    int numThreads = atoi(argv[3]);

    // Вычисление количества блоков и потоков
    int numBlocks = (numThreads + 255) / 256;
    dim3 grid(numBlocks);
    dim3 block(256);

    // Выделение памяти на GPU для массива адресов кошельков TRON
    char *d_addresses;
    cudaMalloc(&d_addresses, numThreads * ADDRESS_LENGTH);

    // Генерация адресов кошельков TRON на GPU
    generateAddresses<<<grid, block>>>(d_addresses, prefixLength, suffixLength);

    // Копирование результата с GPU на CPU
    char *h_addresses = (char*)malloc(numThreads * ADDRESS_LENGTH);
    cudaMemcpy(h_addresses, d_addresses, numThreads * ADDRESS_LENGTH, cudaMemcpyDeviceToHost);

    // Вывод совпавших адресов кошельков TRON
    for (int i = 0; i < numThreads; i++) {
        printf("%s\n", h_addresses + i * ADDRESS_LENGTH);
    }

    // Освобождение памяти на GPU и CPU
    cudaFree(d_addresses);
    free(h_addresses);

    return 0;
}
