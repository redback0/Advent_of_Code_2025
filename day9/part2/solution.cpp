
#include <set>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
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

bool any_edge_intersects(Tile& corner1, Tile& corner2, std::vector<Tile> tiles)
{
    Tile topleft = {std::min(corner1.x, corner2.x),
        std::min(corner1.y, corner2.y)};
    Tile botright = {std::max(corner1.x, corner2.x),
        std::max(corner1.y, corner2.y)};

    auto edge1 = tiles.begin();
    auto edge2 = tiles.end() - 1;

    for (; edge1 != tiles.end(); edge2 = edge1++)
    {
        if (edge1->x > topleft.x && edge1->x < botright.x)
        {
            if (edge1->y > topleft.y != edge2->y > topleft.y ||
                edge1->y < botright.y != edge2->y < botright.y)
            {
                return true;
            }
        }
        if (edge1->y > topleft.y && edge1->y < botright.y)
        {
            if (edge1->x > topleft.x != edge2->x > topleft.x ||
                edge1->x < botright.x != edge2->x < botright.x)
            {
                return true;
            }
        }
    }
    return false;
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

    Tile rightmost = {0, 0};

    // read file, take note of biggest values
    while (std::getline(input, line))
    {
        auto comma = line.find(',');

        int x = std::atoi(line.c_str());
        int y = std::atoi(line.substr(comma+1).c_str());
        tiles.push_back({x, y});
        all_x.insert(x);
        all_y.insert(y);
        if (x > rightmost.x)
            rightmost = {x, y};
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


    auto from = tiles.end();
    from--;
    auto to = tiles.begin();

    // mark lines in map
    for (; to != tiles.end(); from = to++)
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

    auto rightmost_iter = std::find_if(tiles.begin(), tiles.end(), 
        [rightmost](auto first){
            return first.x == rightmost.x && first.y == rightmost.y;
        });
    auto rightmost_iter2 = rightmost_iter + 1;

    if (rightmost_iter->y > rightmost_iter2->y)
    {
        std::reverse(tiles.begin(), tiles.end());
    }

    // find rectangles
    int64_t biggest = 0;

    int64_t edge_biggest = 0;
    Tile edge_big_corn1;
    Tile edge_big_corn2;

    int64_t map_biggest = 0;
    Tile map_big_corn1;
    Tile map_big_corn2;

    auto corner1 = tiles.begin();
    for (; corner1 != tiles.end(); corner1++)
    {
        auto corner2 = tiles.begin();
        // // for inside detection
        // auto prev_corner = corner1;
        // prev_corner--;
        // auto next_corner = corner2;

        for (; corner2 != tiles.end(); corner2++)
        {
            // get rectangle size
            int64_t area = 
                (std::abs(corner1->x - corner2->x) + 1) *
                (std::abs(corner1->y - corner2->y) + 1);

            // std::cout << "Checking:" << std::endl;
            // std::cout << corner1->x << ", " << corner1->y << std::endl;
            // std::cout << corner2->x << ", " << corner2->y << std::endl;


            // check if rectagle is bigger and lands on inside;

            if (area == 1473551379)
                std::cout << "THIS ONE" << std::endl;

            if (area > biggest)
                biggest = area;
            Tile check = *corner1;
            if (area > edge_biggest &&
                !any_edge_intersects(*corner1, *corner2, tiles))
            {
                // std::cout << "new bigger" << std::endl;
                edge_biggest = area;
                edge_big_corn1 = *corner1;
                edge_big_corn2 = *corner2;
            }
            if (area > map_biggest &&
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
                map_biggest = area;
                map_big_corn1 = *corner1;
                map_big_corn2 = *corner2;
            }
        }
    }

    // print map
    edge_big_corn1 = adj_tile(edge_big_corn1, all_x, all_y);
    edge_big_corn2 = adj_tile(edge_big_corn2, all_x, all_y);
    map[edge_big_corn1.y][edge_big_corn1.x] = 'E';
    map[edge_big_corn2.y][edge_big_corn2.x] = 'E';

    map_big_corn1 = adj_tile(map_big_corn1, all_x, all_y);
    map_big_corn2 = adj_tile(map_big_corn2, all_x, all_y);
    map[map_big_corn1.y][map_big_corn1.x] = 'M';
    map[map_big_corn2.y][map_big_corn2.x] = 'M';

    Tile first = adj_tile(*tiles.begin(), all_x, all_y);
    Tile last = adj_tile(tiles.back(), all_x, all_y);
    map[first.y][first.x] = 'F';
    map[last.y][last.x] = 'L';

    for (int row = 0; row < mapy; row++)
        std::cout << map[row] << std::endl;

    std::cout << "biggest: " << biggest << std::endl;
    std::cout << "edge biggest: " << edge_biggest << std::endl;
    std::cout << "map biggest: " << map_biggest << std::endl;
}