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

void manual_mulMod_test(int quantity) {
    std::cout << "Starting manual mulMod testing..." << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    for (int i = 0; i < quantity; i++) {
        long long a, b, m;

        std::cout << "Enter SMALL a: ";
        std::cin >> a;
        std::cout << "Enter SMALL b: ";
        std::cin >> b;
        std::cout << "Enter SMALL m (must be positive): ";
        std::cin >> m;

        if (m <= 0) {
            std::cout << "Error: modulus must be positive!" << std::endl;
            i--;
            continue;
        }

        BigInt bigA(a);
        BigInt bigB(b);
        BigInt bigM(m);

        global_memory_stats.reset();

        BigInt result = bigA.mulMod(bigB, bigM);

        std::cout << "Memory usage - Total allocations: " << global_memory_stats.total_allocations
            << ", Peak memory: " << global_memory_stats.peak_memory << " bytes" << std::endl;

        bool overflow = false;
        long long ethalon_result = 0;

        auto safe_multiply_mod = [&](long long x, long long y, long long mod) -> bool {
            if (x == 0 || y == 0) {
                ethalon_result = 0;
                return true;
            }

            if (x > 0) {
                if (y > 0) {
                    if (x > std::numeric_limits<long long>::max() / y) {
                        return false;
                    }
                }
                else if (y < 0) {
                    if (y < std::numeric_limits<long long>::min() / x) {
                        return false;
                    }
                }
            }
            else if (x < 0) {
                if (y > 0) {
                    if (x < std::numeric_limits<long long>::min() / y) {
                        return false;
                    }
                }
                else if (y < 0) {
                    if (x < std::numeric_limits<long long>::max() / y) {
                        return false;
                    }
                }
            }

            long long product = x * y;
            ethalon_result = product % mod;
            if (ethalon_result < 0) {
                ethalon_result += mod;
            }
            return true;
            };

        if (!safe_multiply_mod(a, b, m)) {
            std::cout << "Test #" << i << " - Skipped due to potential overflow" << std::endl;
            continue;
        }

        BigInt bigEthalon(ethalon_result);

        std::cout << "Test #" << i
            << ": a = " << a
            << ", b = " << b
            << ", m = " << m
            << ", expected result = " << ethalon_result
            << ", got = " << result.toChar();

        if (result == bigEthalon) {
            std::cout << " - Passed";
        }
        else {
            std::cout << " - Failed";
            std::cout << "\n  Expected: " << bigEthalon.toChar();
            std::cout << "\n  Got: " << result.toChar();
        }

        std::cout << std::endl << std::endl;
    }
}

void experiments_mulMod_memory_factors(
    unsigned long min_blocks,
    unsigned long max_blocks,
    unsigned long step_blocks,
    unsigned long modulus_blocks,
    int seed,
    int experiments_per_size) {

    std::time_t now = std::time(nullptr);
    std::tm localTime{};
    localtime_s(&localTime, &now);

    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M", &localTime);

    std::string filename = "../experiments/experiments_memory_factors_" + std::string(buffer) + ".csv";

    std::ofstream csv_file(filename);
    if (!csv_file.is_open()) {
        std::cerr << "Error: Cannot open .csv for writing!" << std::endl;
        return;
    }

    csv_file << "experiment_number;factor_blocks;modulus_blocks;peak_memory_bytes;total_allocations" << std::endl;

    srand(static_cast<unsigned int>(seed));

    int experiment_idx = 0;

    BigInt modulus(1LL);
    for (unsigned long j = 0; j < modulus_blocks; j++) {
        unsigned int block_val = 0;
        for (int k = 0; k < 3; k++) {
            block_val *= 1000;
            block_val += rand() % 1000;
        }
        modulus = modulus.mulShort(BigInt::BASE) + BigInt(static_cast<long long>(block_val));
    }

    if (modulus <= BigInt(1LL)) {
        modulus = BigInt(2LL);
    }

    std::cout << "Starting memory analysis for mulMod factors dependence..." << std::endl;
    std::cout << "Modulus has " << modulus_blocks << " blocks" << std::endl;
    std::cout << "Modulus value: " << modulus.toChar() << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    for (unsigned long blocks = min_blocks; blocks <= max_blocks; blocks += step_blocks) {
        std::cout << "Testing factors with " << blocks << " blocks" << std::endl;

        long long sum_peak_memory = 0;
        long long sum_allocations = 0;

        for (int e = 0; e < experiments_per_size; e++) {
            BigInt a(1LL);
            for (unsigned long j = 0; j < blocks; j++) {
                unsigned int block_val = 0;
                for (int k = 0; k < 3; k++) {
                    block_val *= 1000;
                    block_val += rand() % 1000;
                }
                a = a.mulShort(BigInt::BASE) + BigInt(static_cast<long long>(block_val));
            }

            BigInt b(1LL);
            for (unsigned long j = 0; j < blocks; j++) {
                unsigned int block_val = 0;
                for (int k = 0; k < 3; k++) {
                    block_val *= 1000;
                    block_val += rand() % 1000;
                }
                b = b.mulShort(BigInt::BASE) + BigInt(static_cast<long long>(block_val));
            }

            global_memory_stats.reset();

            BigInt result = a.mulMod(b, modulus);

            sum_peak_memory += global_memory_stats.peak_memory;
            sum_allocations += global_memory_stats.total_allocations;

            std::cout << "Experiment #" << experiment_idx
                << ", factor blocks = " << blocks
                << ", modulus blocks = " << modulus_blocks
                << ", peak memory = " << global_memory_stats.peak_memory << " bytes"
                << ", allocations = " << global_memory_stats.total_allocations << std::endl;

            experiment_idx++;
        }

        long long avg_peak_memory = sum_peak_memory / experiments_per_size;
        long long avg_allocations = sum_allocations / experiments_per_size;

        csv_file << experiment_idx - experiments_per_size << ";"
            << blocks << ";"
            << modulus_blocks << ";"
            << avg_peak_memory << ";"
            << avg_allocations << std::endl;

        std::cout << "AVERAGE for factor blocks " << blocks
            << ": peak memory = " << avg_peak_memory << " bytes"
            << ", allocations = " << avg_allocations << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    csv_file.close();
    std::cout << "All memory experiments for factors completed." << std::endl;
}

void experiments_mulMod_memory_modulus(
    unsigned long min_blocks,
    unsigned long max_blocks,
    unsigned long step_blocks,
    unsigned long factor_blocks,
    int seed,
    int experiments_per_size) {

    std::time_t now = std::time(nullptr);
    std::tm localTime{};
    localtime_s(&localTime, &now);

    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M", &localTime);

    std::string filename = "../experiments/experiments_memory_modulus_" + std::string(buffer) + ".csv";

    std::ofstream csv_file(filename);
    if (!csv_file.is_open()) {
        std::cerr << "Error: Cannot open .csv for writing!" << std::endl;
        return;
    }

    csv_file << "experiment_number;factor_blocks;modulus_blocks;peak_memory_bytes;total_allocations" << std::endl;

    srand(static_cast<unsigned int>(seed));

    int experiment_idx = 0;

    BigInt a(1LL);
    BigInt b(1LL);
    for (unsigned long j = 0; j < factor_blocks; j++) {
        unsigned int block_val_a = 0;
        unsigned int block_val_b = 0;
        for (int k = 0; k < 3; k++) {
            block_val_a *= 1000;
            block_val_a += rand() % 1000;
            block_val_b *= 1000;
            block_val_b += rand() % 1000;
        }
        a = a.mulShort(BigInt::BASE) + BigInt(static_cast<long long>(block_val_a));
        b = b.mulShort(BigInt::BASE) + BigInt(static_cast<long long>(block_val_b));
    }

    std::cout << "Starting memory analysis for mulMod modulus dependence..." << std::endl;
    std::cout << "Factors have " << factor_blocks << " blocks each" << std::endl;
    std::cout << "Factor a: " << a.toChar() << std::endl;
    std::cout << "Factor b: " << b.toChar() << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    for (unsigned long blocks = min_blocks; blocks <= max_blocks; blocks += step_blocks) {
        std::cout << "Testing modulus with " << blocks << " blocks" << std::endl;

        long long sum_peak_memory = 0;
        long long sum_allocations = 0;

        for (int e = 0; e < experiments_per_size; e++) {
            // Генерация модуля
            BigInt modulus(1LL);
            for (unsigned long j = 0; j < blocks; j++) {
                unsigned int block_val = 0;
                for (int k = 0; k < 3; k++) {
                    block_val *= 1000;
                    block_val += rand() % 1000;
                }
                modulus = modulus.mulShort(BigInt::BASE) + BigInt(static_cast<long long>(block_val));
            }

            // Гарантируем, что модуль > 1
            if (modulus <= BigInt(1LL)) {
                modulus = BigInt(2LL);
            }

            // Сброс статистики памяти
            global_memory_stats.reset();

            // Выполнение mulMod с измерением памяти
            BigInt result = a.mulMod(b, modulus);

            sum_peak_memory += global_memory_stats.peak_memory;
            sum_allocations += global_memory_stats.total_allocations;

            std::cout << "Experiment #" << experiment_idx
                << ", factor blocks = " << factor_blocks
                << ", modulus blocks = " << blocks
                << ", peak memory = " << global_memory_stats.peak_memory << " bytes"
                << ", allocations = " << global_memory_stats.total_allocations << std::endl;

            experiment_idx++;
        }

        long long avg_peak_memory = sum_peak_memory / experiments_per_size;
        long long avg_allocations = sum_allocations / experiments_per_size;

        csv_file << experiment_idx - experiments_per_size << ";"
            << factor_blocks << ";"
            << blocks << ";"
            << avg_peak_memory << ";"
            << avg_allocations << std::endl;

        std::cout << "AVERAGE for modulus blocks " << blocks
            << ": peak memory = " << avg_peak_memory << " bytes"
            << ", allocations = " << avg_allocations << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    csv_file.close();
    std::cout << "All memory experiments for modulus completed." << std::endl;
}

int main() {

    // manual_mulMod_test(5);

    experiments_mulMod_memory_factors(1, 32, 1, 4, 0, 10);

    // experiments_mulMod_memory_modulus(1, 32, 1, 4, 0, 10);

    return 0;
}