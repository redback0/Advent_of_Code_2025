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

    // getline each line, check first character, atoi number
    while (input.good())
    {
        std::getline(input, line);

        if (line.empty())
            break;

        move = std::atoi(line.c_str() + 1);

        if (line[0] == 'R')
            pos += move;
        else if (line[0] == 'L')
            pos -= move;
        else
        {
            std::cout << "not a direction::" << line << "::" << std::endl;
            return 1;
        }

        if (pos % 100 == 0)
            count++;
    }

    std::cout << count << std::endl;

    input.close();
}