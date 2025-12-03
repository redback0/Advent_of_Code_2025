#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

#define NUM_DIGITS 12

int main(int ac, char** av)
{
    if (ac < 2)
    {
        std::cout << "Please provide input" << std::endl;
        return 1;
    }

    std::ifstream input(av[1]);
    std::string line;

    long long total_joltage = 0;
    long long joltage;

    while (input.good())
    {
        std::getline(input, line);

        if (line.empty())
            break;

        std::cout << line << std::endl;

        auto searchFrom = line.begin();
        std::string::iterator digit;

        joltage = 0;
        for (int i = NUM_DIGITS; i > 0; i--)
        {
            digit = std::max_element(searchFrom, line.end() - (i - 1));

            joltage += (*digit - '0') * std::pow(10, i - 1);
            searchFrom = digit + 1;
        }

        std::cout << '\n' << joltage << std::endl;

        total_joltage += joltage;
    }

    std::cout << "\n\n" << total_joltage << std::endl;
}