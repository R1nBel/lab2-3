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
#include <exception>

void operations_count_pow_exp_experiments(unsigned long base_digits, unsigned long min_exp, unsigned long max_exp, int seed, int quantity_per_exp)
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
            num_str += static_cast<char>('0' + rand() % 10);

        bases.emplace_back(num_str.c_str());
    }

    for (unsigned long exp = min_exp; exp <= max_exp; exp *= 2)
    {
        long long sum_time = 0;
        int success_count = 0;

        for (int q = 0; q < quantity_per_exp; q++)
        {
            try
            {
                auto start = std::chrono::high_resolution_clock::now();
                BigInt result = bases[q].bigPow(static_cast<long long>(exp));
                auto end = std::chrono::high_resolution_clock::now();

                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                sum_time += static_cast<long long>(duration.count());
                success_count++;

                std::cout << "Experiment #" << experiment_idx
                    << " exp = " << exp
                    << ", time = " << duration.count() << " micros" << std::endl;
            }
            catch (const std::exception& e)
            {
                std::cerr << "Experiment #" << experiment_idx
                    << " exp = " << exp
                    << " skipped: " << e.what() << std::endl;
            }

            experiment_idx++;
        }

        if (success_count > 0)
        {
            long long avg_time = sum_time / success_count;
            csv_file << experiment_idx - quantity_per_exp << ";"
                << exp << ";"
                << avg_time << std::endl;

            std::cout << "AVERAGE for exponent " << exp
                << ": time = " << avg_time << " micros" << std::endl;
        }
        else
        {
            std::cout << "All experiments for exponent " << exp << " were skipped" << std::endl;
        }
    }

    csv_file.close();
}

void manual_pow_test(int quantity)
{
    for (int i = 0; i < quantity; i++)
    {
        long long a;
        unsigned long m;

        std::cin >> a >> m;

        try
        {
            BigInt bigA(a);
            BigInt result = bigA.bigPow(static_cast<long long>(m));

            long long ethalon_result = 1;
            bool overflow = false;

            auto safe_multiply = [&](long long x, long long y) -> bool {
                if (x == 0 || y == 0) { ethalon_result = 0; return true; }
                if (x > 0 && y > 0 && x > std::numeric_limits<long long>::max() / y) return false;
                if (x < 0 && y < 0 && x < std::numeric_limits<long long>::max() / y) return false;
                if (x > 0 && y < 0 && y < std::numeric_limits<long long>::min() / x) return false;
                if (x < 0 && y > 0 && x < std::numeric_limits<long long>::min() / y) return false;
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
                std::cout << "Test #" << i << " skipped" << std::endl;
                continue;
            }

            BigInt bigEthalon(ethalon_result);

            if (result == bigEthalon)
                std::cout << "Test #" << i << " passed" << std::endl;
            else
                std::cout << "Test #" << i << " failed" << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Test #" << i << " skipped: " << e.what() << std::endl;
        }
    }
}

int main()
{
    //manual_pow_test(2);

    operations_count_pow_exp_experiments(32, 2, 8192, 0, 1);

    //operations_count_pow_base_experiments(1, 64, 1, 64, 0, 10);

    return 0;
}