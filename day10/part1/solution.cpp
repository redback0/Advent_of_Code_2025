
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

class Machine
{
private:
    std::vector<std::vector<int>> _buttons;
    std::string _requirements;

public:
    Machine(std::string details)
        : _buttons()
        , _requirements()
    {
        _requirements = details.substr(1, details.find(']') - 1);
        details = details.substr(details.find('('));

        while (details[0] == '(')
        {
            std::vector<int> button;

            while (details[0] != ')')
            {
                int number = std::atoi(details.c_str() + 1);
                button.push_back(number);
                details = details.substr(details.find_first_of(",)", 1));
            }
            details = details.substr(details.find_first_of("({"));
            _buttons.push_back(button);
        }

        // ehehe skip the end for now
    }

    static void pressButton(std::string& solve, const std::vector<int>& button)
    {
        for (auto toggle : button)
        {
            solve[toggle] = solve[toggle] == '.' ? '#' : '.';
        }
    }

    int getMinSolve()
    {
        int lowest_solve = _buttons.size();

        for (int bits = 1;
            bits < std::pow(2, _buttons.size()) && lowest_solve > 1;
            bits++)
        {
            std::string solve(_requirements.size(), '.');
            int solve_presses = 0;
            for (int i = 0; bits >> i; i++)
            {
                if ((bits >> i) & 1)
                {
                    solve_presses++;
                    pressButton(solve, _buttons[i]);
                }
            }

            if (solve == _requirements && solve_presses < lowest_solve)
                lowest_solve = solve_presses;
        }

        return lowest_solve;
    }
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

    int count_presses = 0;

    while (std::getline(input, line))
    {
        Machine machine(line);

        count_presses += machine.getMinSolve();
    }

    std::cout << count_presses << std::endl;
}