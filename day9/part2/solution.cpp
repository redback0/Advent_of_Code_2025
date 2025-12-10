
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <vector>

struct Tile
{
    int64_t x;
    int64_t y;
};

Tile adj_tile(
    const Tile& og,
    const std::set<int>& all_x,
    const std::set<int>& all_y)
{
    Tile adj;
    adj.x = 2 * std::distance(all_x.begin(), all_x.find(og.x));
    adj.y = 2 * std::distance(all_y.begin(), all_y.find(og.y));

    return adj;
}

int adj_coord(
    int og,
    const std::set<int>& coord_set)
{
    return 2 * std::distance(coord_set.begin(), coord_set.find(og));
}

void flood_fill(
    char **map,
    int mapX,
    int mapY,
    char wall,
    char fill,
    int x,
    int y)
{
    map[y][x] = fill;
    if (y + 1 < mapY && map[y + 1][x] != wall && map[y + 1][x] != fill)
        flood_fill(map, mapX, mapY, wall, fill, x, y + 1);
    if (y - 1 >= 0 && map[y - 1][x] != wall && map[y - 1][x] != fill)
        flood_fill(map, mapX, mapY, wall, fill, x, y - 1);
    if (x + 1 < mapX && map[y][x + 1] != wall && map[y][x + 1] != fill)
        flood_fill(map, mapX, mapY, wall, fill, x + 1, y);
    if (x - 1 >= 0 && map[y][x - 1] != wall && map[y][x - 1] != fill)
        flood_fill(map, mapX, mapY, wall, fill, x - 1, y);
}

void flood_fill(
    char **map,
    int mapX,
    int mapY,
    char wall,
    char fill)
{
    map[0][0] = fill;

    if (1 < mapY && map[1][0] != wall)
        flood_fill(map, mapX, mapY, wall, fill, 0, 1);
    if (1 < mapX && map[0][1] != wall)
        flood_fill(map, mapX, mapY, wall, fill, 1, 0);
}

bool check_line(char **map, Tile start, Tile end, Tile step, char oob)
{
    while ((step.x && start.x != end.x) || (step.y && start.y != end.y))
    {
        if (map[start.y][start.x] == oob)
        {
            return false;
        }
        start.x += step.x;
        start.y += step.y;
    }
    return true;
}

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

    std::set<int> all_x;
    std::set<int> all_y;

    all_x.insert(0);
    all_y.insert(0);

    // read file, take note of biggest values
    while (std::getline(input, line))
    {
        auto comma = line.find(',');

        int x = std::atoi(line.c_str());
        int y = std::atoi(line.substr(comma+1).c_str());
        tiles.push_back({x, y});
        all_x.insert(x);
        all_y.insert(y);
    }

    // create empty map
    int mapx = all_x.size()*2 + 2;
    int mapy = all_y.size()*2 + 1;
    char map[mapy][mapx];
    char *map_ptr[mapy];
    for (int row = 0; row < mapy; row++)
    {
        std::memset(map[row], 'x', mapx);
        map[row][mapx - 1] = '\0';
        map_ptr[row] = map[row];
    }

    auto from = tiles.end() - 1;
    auto to = tiles.begin();

    // mark lines in map
    for (; to < tiles.end(); from = to++)
    {
        Tile adj_to = adj_tile(*to, all_x, all_y);
        Tile line = adj_tile(*from, all_x, all_y);
        Tile step = {to->x - from->x, to->y - from->y};

        if (step.x != 0) step.x /= std::abs(step.x);
        if (step.y != 0) step.y /= std::abs(step.y);

        while (line.x != adj_to.x || line.y != adj_to.y)
        {
            map[line.y][line.x] = 'B';

            line.x += step.x;
            line.y += step.y;
        }
    }

    //fill OOB
    flood_fill(map_ptr, mapx - 1, mapy, 'B', ' ');

    // find rectangles
    int64_t biggest_area = 0;
    Tile biggest_corner1;
    Tile biggest_corner2;
    auto corner1 = tiles.begin();
    for (; corner1 != tiles.end() - 1; corner1++)
    {
        auto corner2 = corner1 + 1;
        for (; corner2 != tiles.end(); corner2++)
        {
            // std::cout << std::endl;

            // get rectangle size
            int64_t area = std::abs((
                corner1->x - corner2->x + 1) *
                (corner1->y - corner2->y + 1));

            // check if rectagle is bigger and lands on inside;
            Tile check = *corner1;
            if (area > biggest_area &&
                check_line(map_ptr,
                    adj_tile({std::min(corner1->x, corner2->x), corner1->y}, all_x, all_y),
                    adj_tile({std::max(corner1->x, corner2->x), corner1->y}, all_x, all_y),
                    {1, 0}, ' ') &&
                check_line(map_ptr,
                    adj_tile({std::min(corner1->x, corner2->x), corner2->y}, all_x, all_y),
                    adj_tile({std::max(corner1->x, corner2->x), corner2->y}, all_x, all_y),
                    {1, 0}, ' ') &&
                check_line(map_ptr,
                    adj_tile({corner1->x, std::min(corner1->y, corner2->y)}, all_x, all_y),
                    adj_tile({corner1->x, std::max(corner1->y, corner2->y)}, all_x, all_y),
                    {0, 1}, ' ') &&
                check_line(map_ptr,
                    adj_tile({corner2->x, std::min(corner1->y, corner2->y)}, all_x, all_y),
                    adj_tile({corner2->x, std::max(corner1->y, corner2->y)}, all_x, all_y),
                    {0, 1}, ' '))
            {
                // std::cout << "new bigger" << std::endl;
                biggest_area = area;
                biggest_corner1 = *corner1;
                biggest_corner2 = *corner2;
            }
        }
    }

    std::cout << biggest_corner1.x << ", " << biggest_corner1.y << std::endl;
    std::cout << biggest_corner2.x << ", " << biggest_corner2.y << std::endl;

    biggest_corner1 = adj_tile(biggest_corner1, all_x, all_y);
    biggest_corner2 = adj_tile(biggest_corner2, all_x, all_y);
    map[biggest_corner1.y][biggest_corner1.x] = 'C';
    map[biggest_corner2.y][biggest_corner2.x] = 'C';
    for (int row = 0; row < mapy; row++)
        std::cout << map[row] << std::endl;

    std::cout << biggest_area << std::endl;
}