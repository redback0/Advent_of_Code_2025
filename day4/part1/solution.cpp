#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <array>

int count_adjacent(int c, std::array<std::string, 3> map)
{
    int adjacent = 0;

    for (int i = 0; i < map.size(); i++)
    {
        if (map[i].empty())
            continue;
        for (int j = std::max(0, c - 1); j <= std::min((int)map[1].size(), c + 1); j++)
        {
            if (i == 1 && j == c)
                continue;
            if (map[i][j] == '@')
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

    std::array<std::string, 3> map;

    std::getline(input, map[1]);
    std::getline(input, map[2]);

    int count = 0;

    while (!map[1].empty())
    {
        for (int c = 0; c < map[1].size(); c++)
        {
            if (map[1][c] != '@')
            {
                std::cout << map[1][c];
                continue;
            }
            int adjacent = count_adjacent(c, map);

            if (adjacent < 4)
            {
                count++;
            }
            std::cout << adjacent;
        }
        std::cout << std::endl;

        map[0] = map[1];
        map[1] = map[2];
        if (input.good())
            std::getline(input, map[2]);
    }

    std::cout << count << std::endl;
}