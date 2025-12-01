#include <cstdlib>
#include <fstream>
#include <iostream>


int main(int ac, char** av)
{
    // open file av[1]
    if (ac < 2)
        return 1;

    std::ifstream input(av[1]);
    std::string line;
    int pos = 50;
    int move = 0;
    int count = 0;
    int start_pos;

    // getline each line, check first character, atoi number
    while (input.good())
    {
        std::getline(input, line);

        if (line.empty())
            break;

        start_pos = pos;

        move = std::atoi(line.c_str() + 1);
        int add_count = 0;

        if (line[0] == 'R')
            pos += move;
        else if (line[0] == 'L')
        {
            if (pos == 0)
                add_count--;
            pos -= move;
            if (pos % 100 == 0)
                add_count++;
        }
        else
        {
            std::cout << "not a direction::" << line << "::" << std::endl;
            return 1;
        }

        while (pos >= 100)
        {
            add_count++;
            pos -= 100;
        }
        while (pos < 0)
        {
            add_count++;
            pos += 100;
        }
        count += add_count;

        std::cout << line << " " << start_pos << " -> " << pos << " +" << add_count << std::endl;
    }

    std::cout << count << std::endl;

    input.close();
}