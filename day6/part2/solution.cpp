
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
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

    std::vector<std::string> whole_file;

    for (std::getline(input, line);
        std::isdigit(line[0]);
        std::getline(input, line))
    {
        whole_file.push_back(line);
    }

    int64_t total = 0;
    while (!line.empty())
    {
        char operation = line[0];

        auto next = std::find_if(line.begin() + 1, line.end(),
            [](char c){return !std::isspace(c);});

        bool is_last = next >= line.end();

        int length = std::distance(line.begin(), next);
        line = std::string(next, line.end());

        std::vector<std::string> column_numbers;

        for (auto& row : whole_file)
        {
            if (!is_last)
                column_numbers.push_back(row.substr(0, length - 1));
            else
                column_numbers.push_back(row.substr(0, length));

            row = row.substr(length);
        }

        int64_t column_total;
        switch (operation)
        {
            case '+':
                column_total = 0;
                break;
            case '*':
                column_total = 1;
                break;
            default:
                std::cout << "Error in reading operations" << std::endl;
                return 1;
        }

        for (int i = 0; i < column_numbers[0].size(); i++)
        {
            std::string number;

            for (int row = 0; row < column_numbers.size(); row++)
            {
                number += column_numbers[row][i];
            }

            switch (operation)
            {
                case '+':
                    column_total += std::atoll(number.c_str());
                    break;
                case '*':
                    column_total *= std::atoll(number.c_str());
                    break;
            }
        }

        std::cout << operation << column_total << std::endl;
        total += column_total;
    }

    std::cout << '\n' << total << std::endl;
}