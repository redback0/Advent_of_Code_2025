
#include <cstring>
#include <fstream>
#include <iostream>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

void printVector(const std::vector<std::string>& vec)
{
    for (auto& str : vec)
    {
        std::cout << str << std::endl;
    }
}

class Shape
{
private:
    std::vector<std::string> _template[4];
    int _size;

    void spinShape(int orient)
    {
        int vec_size = _template[orient-1].size();
        int line_size = _template[orient-1][0].size();
        _template[orient].resize(line_size);
        for (int i = 0; i < line_size; i++)
        {
            _template[orient][i].resize(vec_size);
            _template[orient][i][vec_size-1] = '\0';
            for (int j = 0; j < vec_size; j++)
            {
                _template[orient][i][j] = _template[orient-1][j][line_size - i - 1];
            }
        }
    }
public:
    Shape() = default;

    Shape(std::ifstream& file_input)
        : _template()
        , _size(0)
    {
        std::string line;
        while (std::getline(file_input, line) && !line.empty())
            _template[0].push_back(line);

        for (int i = 0; i < _template[0].size(); i++)
        {
            for (int j = 0; j < _template[0][i].size(); j++)
            {
                if (_template[0][i][j] == '#')
                    _size++;
            }
        }

        for (int i = 1; i < 4; i++)
            spinShape(i);
    }

    const std::vector<std::string>& operator[](int i)
    {
        if (i >= 0 && i < 4)
            return _template[i];
        else
            throw std::out_of_range("erm");
    }

    int size()
    {
        return _size;
    }
};

void removeShape(
    std::vector<std::string>& map,
    int x,
    int y,
    Shape shape,
    int orientation)
{
    auto& templ = shape[orientation];
    for (int i = 0; i < templ.size(); i++)
    {
        for (int j = 0; j < templ[i].length(); j++)
        {
            if (templ[i][j] == '#')
                map[i+y][j+x] = '.';
        }
    }
}

bool putShape(
    std::vector<std::string>& map,
    int x,
    int y,
    Shape shape,
    int orientation)
{
    auto& templ = shape[orientation];

    if (
        map.size() - y < templ.size() ||
        map[0].size() - x < templ[0].length()
    )
        return false;

    for (int i = 0; i < templ.size(); i++)
    {
        for (int j = 0; j < templ[i].length(); j++)
        {
            if (templ[i][j] == '#' &&
                map[i+y][j+x] != '.')
                return false;
        }
    }
    for (int i = 0; i < templ.size(); i++)
    {
        for (int j = 0; j < templ[i].length(); j++)
        {
            if (templ[i][j] == '#')
                map[i+y][j+x] = '#';
        }
    }
    return true;
}

struct InMap
{
    int x;
    int y;
    int shapeID;
    int orientation;
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
    std::vector<Shape> shapes;

    while (std::getline(input, line) && !line.empty())
    {
        if (line.back() != ':')
            break;
        shapes.push_back(Shape(input));
        // std::cout << shapes.back().size() << std::endl;
    }

    std::stack<InMap> in_map;
    int definite = 0;
    int maybe = 0;
    int nope = 0;
    while (!line.empty())
    {
        std::cout << line << std::endl;

        std::vector<std::string> map;

        int width = std::atoi(line.c_str());
        int height = std::atoi(line.substr(line.find('x') + 1).c_str());

        std::vector<int> reqs;

        int find = 0;
        while (!line.empty() && (find = line.find(' ', 1)) != line.npos)
        {
            line = line.substr(line.find(' ', 1));
            reqs.push_back(std::atoi(line.c_str()));
        }

        // std::cout << reqs.size() << " [";
        // for (auto req : reqs)
        //     std::cout << req << ", ";
        // std::cout << "\b\b]" << std::endl;

        int total_blocks = 0;
        int total_area = 0;
        for (int i = 0; i < reqs.size(); i++)
        {
            total_blocks += reqs[i];
            total_area += reqs[i] * shapes[i].size();
        }

        int area = width * height;

        std::cout << total_blocks << " (" << (total_blocks * 9) << "), "
            << total_area << ", " << area << std::endl;

        if (area >= total_blocks * 9)
            definite++;
        else if (area >= total_area)
            maybe++;
        else
            nope++;

        // map.resize(height);
        // for (auto& line : map)
        // {
        //     line.resize(width);
        //     std::fill(line.begin(), line.end(), '.');
        // }

        // int i = 0;
        // int j = 0;
        // int k = 0;
        // do
        // {
        //     for (int i = 0; i < map.size(); i++)
        //     {
        //         for (int j = 0; j < map[i].size(); j++)
        //         {
        //             for (; k < shapes.size() * 4; k++)
        //             {
        //                 if (putShape(map, j, i, shapes[k / 4], k % 4))
        //                 {
        //                     in_map.push({j, i, k / 4, k % 4});
        //                     break;
        //                 }
        //                 else
        //                     k++;
        //             }
        //             k = 0;
        //         }
        //         j = 0;
        //     }
        //     i = in_map.top().y;
        //     j = in_map.top().x;
        //     k = in_map.top().shapeID * 4 + in_map.top().orientation;
        // } while (!in_map.empty());

        // printVector(map);
        // std::cout << std::endl;
        std::getline(input, line);
    }

    std::cout << "Definite: " << definite << std::endl;
    std::cout << "Maybe: " << maybe << std::endl;
    std::cout << "Nope: " << nope << std::endl;
    std::cout << "Total: " << definite + maybe << std::endl;
}