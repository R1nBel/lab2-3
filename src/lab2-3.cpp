#define WIN32_LEAN_AND_MEAN
#include "BigInt.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <fstream>
#include <string>
#include <limits>
#include <vector>
#include <ctime>

void operations_count_pow_exp_experiments(unsigned long base_digits, unsigned long min_exp, unsigned long max_exp, unsigned long step_exp, int seed, int quantity_per_exp)
{
    std::time_t now = std::time(nullptr);
    std::tm localTime{};
    localtime_s(&localTime, &now);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M", &localTime);

    std::string filename = "../experiments/experiments_data_exp_" + std::string(buffer) + ".csv";
    std::ofstream csv_file(filename);
    if (!csv_file.is_open())
    {
        std::cerr << "Error: Cannot open .csv for writing!" << std::endl;
        return;
    }

    csv_file << "experiment_number;exponent;avg_time_microseconds" << std::endl;

    srand(static_cast<unsigned int>(seed));

    int experiment_idx = 0;

    std::vector<BigInt> bases;
    bases.reserve(quantity_per_exp);

    for (int q = 0; q < quantity_per_exp; q++)
    {
        std::string num_str;
        num_str.reserve(base_digits);

        num_str += static_cast<char>('1' + rand() % 9);

        for (unsigned long d = 1; d < base_digits; d++)
        {
            num_str += static_cast<char>('0' + rand() % 10);
        }

        bases.emplace_back(num_str.c_str());
    }

    std::cout << "Exponent experiments from " << min_exp
        << " to " << max_exp << " step " << step_exp << std::endl;
    std::cout << "Each exponent receives " << quantity_per_exp << " experiments" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    for (unsigned long exp = min_exp; exp <= max_exp; exp += step_exp)
    {
        std::cout << "Processing exponent = " << exp << std::endl;

        long long sum_time = 0;

        for (int q = 0; q < quantity_per_exp; q++)
        {
            auto start = std::chrono::high_resolution_clock::now();
            BigInt result = bases[q].bigPow(static_cast<long long>(exp));
            auto end = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            sum_time += static_cast<long long>(duration.count());

            std::cout << "  Experiment #" << experiment_idx
                << " exp = " << exp
                << ", time = " << duration.count() << " micros" << std::endl;

            experiment_idx++;
        }

        long long avg_time = sum_time / quantity_per_exp;
        csv_file << experiment_idx - quantity_per_exp << ";"
            << exp << ";"
            << avg_time << std::endl;

        std::cout << "AVERAGE for exponent " << exp
            << ": time = " << avg_time << " micros" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    csv_file.close();
    std::cout << "All exponent experiments completed." << std::endl;
}

void operations_count_pow_base_experiments(unsigned long min_blocks, unsigned long max_blocks, unsigned long step_blocks, unsigned long exponent, int seed, int experiments_per_size)
{
    std::time_t now = std::time(nullptr);
    std::tm localTime{};
    localtime_s(&localTime, &now);

    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M", &localTime);

    std::string filename = "../experiments/experiments_data_base_" + std::string(buffer) + ".csv";

    std::ofstream csv_file(filename);
    if (!csv_file.is_open())
    {
        std::cerr << "Error: Cannot open .csv for writing!" << std::endl;
        return;
    }

    csv_file << "experiment_number;base_blocks;avg_time_microseconds" << std::endl;

    srand(static_cast<unsigned int>(seed));

    int experiment_idx = 0;

    std::cout << "Starting base-size dependence experiments..." << std::endl;
    std::cout << "Exponent = " << exponent << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    for (unsigned long blocks = min_blocks; blocks <= max_blocks; blocks += step_blocks)
    {
        std::cout << "Generating bases with " << blocks << " blocks" << std::endl;

        long long sum_time = 0;

        for (int e = 0; e < experiments_per_size; e++)
        {
            BigInt a(1LL);

            for (unsigned long j = 0; j < blocks; j++)
            {
                unsigned int block_val = 0;
                for (int k = 0; k < 3; k++)
                {
                    block_val *= 1000;
                    block_val += rand() % 1000;
                }

                a = a.mulShort(BigInt::BASE) + BigInt(static_cast<long long>(block_val));
            }

            auto start = std::chrono::high_resolution_clock::now();
            BigInt result = a.bigPow(static_cast<long long>(exponent));
            auto end = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            sum_time += static_cast<long long>(duration.count());

            std::cout << "Experiment #" << experiment_idx
                << ", base blocks = " << blocks
                << ", time = " << duration.count() << " micros" << std::endl;

            experiment_idx++;
        }

        long long avg_time = sum_time / experiments_per_size;
        csv_file << experiment_idx - experiments_per_size << ";"
            << blocks << ";"
            << avg_time << std::endl;

        std::cout << "AVERAGE for base blocks " << blocks
            << ": time = " << avg_time << " micros" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }

    csv_file.close();
    std::cout << "All base-dependence experiments completed." << std::endl;
}

void manual_pow_test(int quantity)
{
    std::cout << "Starting manual testing..." << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    for (int i = 0; i < quantity; i++)
    {
        long long a;
        unsigned long m;

        std::cout << "Enter SMALL base: ";
        std::cin >> a;
        std::cout << "Enter exp: ";
        std::cin >> m;

        BigInt bigA(a);

        BigInt result = bigA.bigPow(static_cast<long long>(m));

        long long ethalon_result = 1;
        bool overflow = false;

        auto safe_multiply = [&](long long x, long long y) -> bool {
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

            ethalon_result = x * y;
            return true;
        };

        for (unsigned long j = 0; j < m; j++)
        {
            if (!safe_multiply(ethalon_result, a))
            {
                overflow = true;
                break;
            }
        }

        if (overflow)
        {
            std::cout << "Test #" << i << " - Skipped due to potential overflow" << std::endl;
            continue;
        }

        BigInt bigEthalon(ethalon_result);

        std::cout << "Test #" << i
            << ": base = " << a
            << ", exp = " << m
            << ", required result = " << ethalon_result
            << ", result = " << result.toChar();

        if (result == bigEthalon)
        {
            std::cout << " - Passed";
        }
        else
        {
            std::cout << " - Not Passed";
            std::cout << "\n  Expected: " << bigEthalon.toChar();
            std::cout << "\n  Got: " << result.toChar();
        }

        std::cout << std::endl;
    }
}

int main()
{
    //manual_pow_test(5);

    operations_count_pow_exp_experiments(32, 2, 2048, 2, 0, 1);

    //operations_count_pow_base_experiments(1, 64, 1, 64, 0, 10);

    return 0;
}