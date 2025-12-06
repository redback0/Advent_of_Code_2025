
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>
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

    std::vector<std::pair<int64_t, int64_t>> add_mults;

    
    for (std::getline(input, line);
        input.good() && !line.empty();
        std::getline(input, line))
    {
        if (!std::isdigit(line[0]))
            break;

        std::istringstream buffer(line);
        std::vector<std::string> split((
            std::istream_iterator<std::string>(buffer)),
            std::istream_iterator<std::string>());

        if (add_mults.empty())
        {
            add_mults.resize(split.size());
            for (auto& ele : add_mults)
            {
                ele.second = 1;
            }
        }

        auto iter_num = split.begin();
        auto add_mult = add_mults.begin();

        for (; iter_num != split.end() && add_mult != add_mults.end();
            iter_num++, add_mult++)
        {
            int64_t num = std::atoll(iter_num->c_str());
            
            add_mult->first += num;
            add_mult->second *= num;
        }
    }

    std::istringstream operations(line);

    int64_t total = 0;

    std::string operation;
    auto add_mult = add_mults.begin();
    for (; add_mult != add_mults.end(); add_mult++)
    {
        operations >> operation;

        switch (operation[0])
        {
            case '+':
                total += add_mult->first;
            break;
            case '*':
                total += add_mult->second;
            break;
            default:
                std::cout << "Error in reading operations" << std::endl;
                return 1;
        }
    }

    std::cout << total << std::endl;
}