
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

int main(int ac, char** av)
{
    if (ac < 2)
    {
        std::cout << "Please provide input" << std::endl;
        return 1;
    }

    std::ifstream input(av[1]);
    std::string line;
    std::string beams;

    std::getline(input, beams);

    int64_t total_splits = 0;

    for (std::getline(input, line);
        input.good() && !line.empty();
        std::getline(input, line))
    {

        auto beam = std::find(beams.begin(), beams.end(), 'S');

        for (; beam != beams.end(); beam = std::find(beam + 1, beams.end(), 'S'))
        {
            auto chara = line.begin() + std::distance(beams.begin(), beam);

            switch (*chara)
            {
                case '^':
                    *(chara - 1) = 'S';
                    *chara = '.';
                    *(chara + 1) = 'S';
                    total_splits++;
                break;
                default:
                    *chara = 'S';
            }
        }

        beams = line;
    }

    std::cout << total_splits << std::endl;
}