#include "BigInt.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <limits>
#include <vector>
#include <map>

struct MemoryStats {
    size_t total_allocations = 0;
    size_t peak_memory = 0;
    size_t current_memory = 0;

    void reset() {
        total_allocations = 0;
        peak_memory = 0;
        current_memory = 0;
    }

    void allocate(size_t size) {
        total_allocations++;
        current_memory += size;
        if (current_memory > peak_memory) {
            peak_memory = current_memory;
        }
    }

    void deallocate(size_t size) {
        if (size <= current_memory) {
            current_memory -= size;
        }
    }
};

MemoryStats global_memory_stats;

void* operator new(size_t size) {
    global_memory_stats.allocate(size);
    return malloc(size);
}

void* operator new[](size_t size) {
    global_memory_stats.allocate(size);
    return malloc(size);
}

void operator delete(void* ptr) noexcept {
    free(ptr);
}

void operator delete[](void* ptr) noexcept {
    free(ptr);
}

void manual_mulMod_test() {
    std::cout << "Starting manual mulMod testing..." << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    BigInt a, b, m;
    BigInt result;

    std::cout << "Enter BIG a: ";
    std::cin >> a;
    std::cout << "Enter BIG b: ";
    std::cin >> b;
    std::cout << "Enter BIG m (must be positive): ";
    std::cin >> m;

    if (m < 1) {
        std::cout << "Error: modulus must be positive!" << std::endl;
        return;
    }

    result = BigInt::mulMod(a, b, m);
        
    std::cout << "Result = " << result << std::endl;
}

void experiments_mulMod_memory_asymptotic(
    unsigned long min_modulus_blocks,
    unsigned long max_modulus_blocks,
    int experiments_per_size,
    int seed)
{
    std::time_t now = std::time(nullptr);
    std::tm localTime{};
    localtime_s(&localTime, &now);

    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M", &localTime);

    std::string filename =
        "../experiments/experiments_memory_asymptotic_" +
        std::string(buffer) + ".csv";

    std::ofstream csv_file(filename);
    if (!csv_file.is_open()) {
        std::cerr << "Error: Cannot open .csv for writing!" << std::endl;
        return;
    }

    csv_file << "modulus_blocks;factor_blocks;avg_peak_memory\n";

    srand(static_cast<unsigned int>(seed));

    std::cout << "Starting asymptotic memory experiment for mulMod\n";
    std::cout << "------------------------------------------------\n";

    for (unsigned long modulus_blocks = min_modulus_blocks;
        modulus_blocks <= max_modulus_blocks;
        modulus_blocks *= 2)
    {
        unsigned long factor_blocks = (2 * modulus_blocks) / 3;

        long long sum_peak_memory = 0;

        std::cout << "Modulus blocks = " << modulus_blocks
            << ", factor blocks = " << factor_blocks << std::endl;

        for (int e = 0; e < experiments_per_size; e++) {

            BigInt modulus(1LL);
            for (unsigned long i = 0; i < modulus_blocks; i++) {
                unsigned int block = 0;
                for (int k = 0; k < 3; k++) {
                    block = block * 1000 + rand() % 1000;
                }
                modulus = modulus.mulShort(BigInt::BASE)
                    + BigInt(static_cast<long long>(block));
            }

            if (modulus <= BigInt(1LL))
                modulus = BigInt(2LL);

            BigInt a(1LL);
            for (unsigned long i = 0; i < factor_blocks; i++) {
                unsigned int block = 0;
                for (int k = 0; k < 3; k++) {
                    block = block * 1000 + rand() % 1000;
                }
                a = a.mulShort(BigInt::BASE)
                    + BigInt(static_cast<long long>(block));
            }

            BigInt b(1LL);
            for (unsigned long i = 0; i < factor_blocks; i++) {
                unsigned int block = 0;
                for (int k = 0; k < 3; k++) {
                    block = block * 1000 + rand() % 1000;
                }
                b = b.mulShort(BigInt::BASE)
                    + BigInt(static_cast<long long>(block));
            }

            global_memory_stats.reset();

            BigInt result = BigInt::mulMod(a, b, modulus);

            sum_peak_memory += global_memory_stats.peak_memory;
        }

        long long avg_peak_memory =
            sum_peak_memory / experiments_per_size;

        csv_file << modulus_blocks << ";"
            << factor_blocks << ";"
            << avg_peak_memory << "\n";

        std::cout << "AVG peak memory: " << avg_peak_memory
            << " bytes" << std::endl;
        std::cout << "------------------------------------------------\n";
    }

    csv_file.close();
    std::cout << "Asymptotic memory experiment completed.\n";
}


int main() {
    int min_moduls_blocks = 8;
    int max_moduls_blocks = 1024;
    int exp_per_size = 10;
    int seed = 1;

    experiments_mulMod_memory_asymptotic(min_moduls_blocks, max_moduls_blocks, exp_per_size, seed);

    //manual_mulMod_test();

    return 0;
}
