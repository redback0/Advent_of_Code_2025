#include <algorithm>
#include <fstream>
#include <iostream>

int main(int ac, char** av)
{
    if (ac < 2)
    {
        std::cout << "Please provide input" << std::endl;
        return 1;
    }

    std::ifstream input(av[1]);
    std::string line;

    int total_joltage = 0;
    int joltage;

    while (input.good())
    {
        std::getline(input, line);

        if (line.empty())
            break;

        auto first = std::max_element(line.begin(), line.end() - 1);
        auto second = std::max_element(first + 1, line.end());

        joltage = ((*first - '0') * 10) + (*second - '0');
        total_joltage += joltage;

        std::cout << line << '\n' << *first << *second << "::" << joltage << std::endl;
    }

    std::cout << "\n\n" << total_joltage << std::endl;
}