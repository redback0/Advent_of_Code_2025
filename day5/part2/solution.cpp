
#include <algorithm>
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

    std::sort(ranges.begin(), ranges.end());

    auto range = ranges.begin();
    auto next = range + 1;

    while (next != ranges.end())
    {
        if (next->first >= range->first && next->first <= range->second)
        {
            if (next->second > range->second)
            {
                range->second = next->second;
            }
            ranges.erase(next);
            next = range + 1;
        }
        else
        {
            range++;
            next++;
        }
    }

    int64_t total = 0;

    for (auto& range : ranges)
    {
        std::cout << range.first << " - " << range.second << std::endl;

        total += range.second - range.first + 1;
    }

    std::cout << '\n' << total << std::endl;
}