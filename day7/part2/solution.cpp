
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
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
    std::vector<int64_t> beams(line.size());


    for (int i = 0; i < line.size(); i++)
    {
        if (line[i] == 'S')
            beams[i] = 1;
    }

    for (std::getline(input, line);
        input.good() && !line.empty();
        std::getline(input, line))
    {
        std::cout << line << std::endl;

        auto split = std::find(line.begin(), line.end(), '^');

        for (; split != line.end(); split = std::find(split + 1, line.end(), '^'))
        {
            auto beam = beams.begin() + std::distance(line.begin(), split);

            *(beam - 1) += *beam;
            *(beam + 1) += *beam;
            *beam = 0;
        }

        for (auto& beam : beams)
        {
            if (beam > 9)
                std::cout << 'B';
            else
                std::cout << beam;
        }

        std::cout << std::endl;
    }

    int64_t total_splits = 0;

    for (auto beam : beams)
    {
        total_splits += beam;
    }

    std::cout << total_splits << std::endl;
}