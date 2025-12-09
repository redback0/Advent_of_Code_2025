
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

struct Tile
{
    int64_t x;
    int64_t y;
};

int main(int ac, char** av)
{
    if (ac < 2)
    {
        std::cout << "Please provide input" << std::endl;
        return 1;
    }

    std::ifstream input(av[1]);
    std::string line;

    std::vector<Tile> tiles;

    while (std::getline(input, line))
    {
        auto comma = line.find(',');

        int x = std::atoi(line.c_str());
        int y = std::atoi(line.substr(comma+1).c_str());
        tiles.push_back({x, y});
    }

    int64_t biggest_area = 0;

    std::cout << tiles.size() << '\n' << std::endl;

    auto first_corner = tiles.begin();
    for (; first_corner != tiles.end() - 1; first_corner++)
    {
        auto second_corner = first_corner + 1;
        for (; second_corner != tiles.end(); second_corner++)
        {
            int64_t area = std::abs((
                first_corner->x - second_corner->x + 1) *
                (first_corner->y - second_corner->y + 1));

            if (area > biggest_area)
            {
                biggest_area = area;
            }
        }
    }

    std::cout << biggest_area << std::endl;
}