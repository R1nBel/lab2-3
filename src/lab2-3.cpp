#define WIN32_LEAN_AND_MEAN
#include "BigInt.cpp"
#include <iostream>
#include <set>
#include <map>
#include <chrono>
#include <fstream>

void operations_count_pow_exp_experiments(unsigned long base_digits, unsigned long min_exp, unsigned long max_exp, unsigned long step_exp, int seed, int quantity_per_exp)
{
    std::ofstream csv_file("../experiments/experiments_data_exp.csv");
    if (!csv_file.is_open())
    {
        std::cerr << "Error: Cannot open experiments_data_exp.csv for writing!" << std::endl;
        return;
    }

    csv_file << "experiment_number;exponent;avg_time_microseconds" << std::endl;

    srand(seed);

    int experiment_idx = 0;

    vector<BigInt> bases;

    for (int q = 0; q < quantity_per_exp; q++)
    {
        std::string num_str;
        num_str.reserve(base_digits);

        for (unsigned long d = 0; d < base_digits; d++)
        {
            int digit = (d == 0) ? 1 + rand() % 9 : rand() % 10;
            num_str += std::to_string(digit);
        }

        BigInt a(num_str);
        bases.push_back(a);
    }

    std::cout << "Exponent experiments from " << min_exp
        << " to " << max_exp << " step " << step_exp << std::endl;

    std::cout << "Each exponent receives " << quantity_per_exp << " experiments" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    for (unsigned long exp = min_exp; exp <= max_exp; exp += step_exp)
    {
        std::cout << "Processing exponent = " << exp << std::endl;

        unsigned long long sum_time = 0;
        unsigned long long sum_ops = 0;

        for (int q = 0; q < quantity_per_exp; q++)
        {
            auto start = std::chrono::high_resolution_clock::now();
            bases[q].bigPow(exp);
            auto end = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

            sum_time += duration.count();

            std::cout << "  Experiment #" << experiment_idx
                << " exp = " << exp
                << ", time = " << duration.count() << std::endl;
        }

        unsigned long long avg_time = sum_time / quantity_per_exp;

        csv_file << experiment_idx << ";"
            << exp << ";"
            << avg_time << std::endl;

        std::cout << "AVERAGE for exponent " << exp
            << ": time = " << avg_time << std::endl;

        std::cout << "------------------------------------------------" << std::endl;

        experiment_idx++;
    }

    csv_file.close();
    std::cout << "All exponent experiments completed." << std::endl;
}

void operations_count_pow_base_experiments(unsigned long min_blocks, unsigned long max_blocks, unsigned long step_blocks, unsigned long exponent, int seed, int experiments_per_size)
{
    std::ofstream csv_file("../experiments/experiments_data_base.csv");
    if (!csv_file.is_open())
    {
        std::cerr << "Error: Cannot open base_dependence.csv for writing!" << std::endl;
        return;
    }

    csv_file << "experiment_number;base_blocks;avg_time_microseconds" << std::endl;

    srand(seed);

    int experiment_idx = 0;

    std::cout << "Starting base-size dependence experiments..." << std::endl;
    std::cout << "Exponent = " << exponent << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    for (unsigned long blocks = min_blocks; blocks <= max_blocks; blocks += step_blocks)
    {
        std::cout << "Generating bases with " << blocks << " blocks" << std::endl;

        unsigned long long sum_time = 0;
        unsigned long long sum_ops = 0;

        for (int e = 0; e < experiments_per_size; e++)
        {
            BigInt a;

            for (unsigned long j = 0; j < blocks; j++)
            {
                unsigned long block = 0;
                for (int k = 0; k < 3; k++)
                {
                    block *= 1000;
                    block += rand() % 1000;
                }
                a = a.mulShort(1000000000) + BigInt(block);
            }

            auto start = std::chrono::high_resolution_clock::now();
            a.bigPow(exponent);
            auto end = std::chrono::high_resolution_clock::now();

            auto duration =
                std::chrono::duration_cast<std::chrono::microseconds>(end - start);

            sum_time += duration.count();

            std::cout << "Experiment #" << experiment_idx
                << ", base blocks = " << blocks
                << ", time = " << duration.count() << " micros" << std::endl;
        }

        unsigned long long avg_time = sum_time / experiments_per_size;

        csv_file << experiment_idx << ";"
            << blocks << ";"
            << avg_time << ";" << std::endl;

        std::cout << "AVERAGE for base blocks " << blocks
            << ": time = " << avg_time << std::endl;

        std::cout << "------------------------------------------------" << std::endl;

        experiment_idx++;
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
        int a;
        unsigned long m;

        std::cout << "Enter SMALL base: ";
        std::cin >> a;
        std::cout << "Enter exp: ";
        std::cin >> m;

        BigInt bigA = a;

        BigInt result = bigA.bigPow(m);

        unsigned long long ethalon_result = 1;

        bool overflow = false;

        for (unsigned long j = 0; j < m; j++)
        {
            if (ethalon_result > ULLONG_MAX / a)
            {
                overflow = true;
                break;
            }
            ethalon_result *= a;
        }

        if (overflow)
        {
            std::cout << "Test #" << i << " - Skipped due to potential overflow" << std::endl;
            continue;
        }

        std::cout << "Test #" << i << ": base = " << a << ", exp = " << m << ", required result = " << ethalon_result << ", result = " << result;

        if (result == ethalon_result)
        {
            std::cout << " - Passed";
        }
        else
        {
            std::cout << " - Not Passed";
        }

        std::cout << std::endl;
    }
}

int main()
{
    //manual_pow_test(5);

    //operations_count_pow_exp_experiments(32, 2, 2048, 2, 0, 1);

    //operations_count_pow_base_experiments(1, 64, 1, 64, 0, 10);

    return 0;
}