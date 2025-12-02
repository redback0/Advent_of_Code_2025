#include <cmath>
#include <fstream>
#include <iostream>

int main(int ac, char** av)
{
    // open file av[1]
    if (ac < 2)
        return 1;

    std::ifstream input(av[1]);
    std::string range_def;

    long long current;
    long long last;
    unsigned long long total = 0;

    while (input.good())
    {
        std::getline(input, range_def, ',');
        if (range_def.empty())
            break;

        auto split = range_def.find('-');

        current = std::atoll(range_def.substr(0, split).c_str());
        last = std::atoll(range_def.substr(split + 1).c_str());

        // std::cout << "RANGE: " << current << " - " << last << std::endl;
        for (; current <= last; current++)
        {
            int num_digits = std::ceil(std::log10(current));
            if (num_digits % 2)
                continue;
            long check = std::pow(10, num_digits / 2) + 1;
            if (current % check == 0)
            {
                // std::cout << current << "::" << num_digits << "::" << check << std::endl;
                total += current;
            }
        }
    }
    std::cout << total << std::endl;
}