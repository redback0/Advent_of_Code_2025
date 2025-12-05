
#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int main(int ac, char** av)
{
    if (ac < 2)
    {
        std::cout << "Please provide input" << std::endl;
        return 1;
    }

    std::ifstream input(av[1]);

    std::string line;

    std::getline(input, line);

    std::vector<std::pair<int64_t, int64_t>> ranges;

    while (input.good() && !line.empty())
    {
        auto split = line.find('-');

        int64_t first = std::atoll(line.substr(0, split).c_str());
        int64_t last = std::atoll(line.substr(split + 1).c_str());

        if (first > last)
        {
            std::cout << "AAAA" << std::endl;
            return (1);
        }

        ranges.push_back(std::pair(first, last));

        std::getline(input, line);
    }

    // could process ranges to be more efficient here

    std::getline(input, line);
    int count = 0;

    while (input.good() && !line.empty())
    {
        int64_t id = std::atoll(line.c_str());

        for (auto& range: ranges)
        {
            if (id >= range.first && id <= range.second)
            {
                count++;
                break;
            }
        }

        std::getline(input, line);
    }

    std::cout << count << std::endl;
}