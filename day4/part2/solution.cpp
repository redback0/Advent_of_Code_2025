#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int count_adjacent(int ci, int cj, std::vector<std::string> map)
{
    int adjacent = 0;

    for (int i = std::max(0, ci - 1); i < std::min((int)map.size(), ci + 2); i++)
    {
        for (int j = std::max(0, cj - 1); j < std::min((int)map[ci].size(), cj + 2); j++)
        {
            if (i == ci && j == cj)
                continue;
            if (map[i][j] == '@' || map[i][j] == 'x')
                adjacent++;
        }
    }
    return (adjacent);
}

int main(int ac, char** av)
{
    if (ac < 2)
    {
        std::cout << "Please provide input" << std::endl;
        return 1;
    }

    std::ifstream input(av[1]);

    std::vector<std::string> map;
    

    while (input.good())
    {
        std::string line;
        std::getline(input, line);
        if (line.empty())
            break;
        map.push_back(line);
    }

    int count = 0;
    int cycle = 1;

    while (cycle != 0)
    {
        cycle = 0;
        for (int ci = 0; ci < map.size(); ci++)
        {
            for (int cj = 0; cj < map[ci].size(); cj++)
            {
                if (map[ci][cj] != '@')
                {
                    continue;
                }
                int adjacent = count_adjacent(ci, cj, map);

                if (adjacent < 4)
                {
                    map[ci][cj] = 'x';
                    cycle++;
                }
            }
        }
        count += cycle;
        for (auto& row : map)
        {
            for (auto& chara : row)
            {
                if (chara == 'x')
                    chara = '.';
            }
        }
    }

    for (auto& row : map)
    {
        std::cout << row << std::endl;
    }

    std::cout << count << std::endl;
}