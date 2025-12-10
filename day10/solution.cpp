
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

class Machine
{
private:
    std::vector<std::vector<int>> _buttons;
    std::string _toggle_req;
    std::vector<int> _add_req;

public:
    Machine(std::string details)
        : _buttons()
        , _toggle_req()
        , _add_req()
    {
        _toggle_req = details.substr(1, details.find(']') - 1);
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

        while (details[0] != '}')
        {
            int number = std::atoi(details.c_str() + 1);
            _add_req.push_back(number);
            details = details.substr(details.find_first_of(",}", 1));
        }
    }

    static void toggleButton(std::string& solve, const std::vector<int>& button)
    {
        for (auto toggle : button)
        {
            solve[toggle] = solve[toggle] == '.' ? '#' : '.';
        }
    }

    int getMinToggleSolve()
    {
        int lowest_solve = _buttons.size();

        for (int bits = 1;
            bits < std::pow(2, _buttons.size()) && lowest_solve > 1;
            bits++)
        {
            std::string solve(_toggle_req.size(), '.');
            int solve_presses = 0;
            for (int i = 0; bits >> i; i++)
            {
                if ((bits >> i) & 1)
                {
                    solve_presses++;
                    toggleButton(solve, _buttons[i]);
                }
            }

            if (solve == _toggle_req && solve_presses < lowest_solve)
                lowest_solve = solve_presses;
        }

        return lowest_solve;
    }

    static void pressAddButton(
        std::vector<int>& solve,
        std::vector<int> button,
        int& count)
    {
        for (auto add : button)
        {
            solve[add]++;
        }
        count++;
    }

    static void pressAddButtonN(
        std::vector<int>& solve,
        std::vector<int> button,
        int& count,
        int n)
    {
        for (auto add : button)
        {
            solve[add] += n;
        }
        count += n;
    }

    static void resetAddButton(
        std::vector<int>& solve,
        std::vector<int> button,
        int& count)
    {
        for (auto unadd : button)
        {
            solve[unadd] -= count;
        }
        count = 0;
    }

    int isAddSolved(const std::vector<int>& solve)
    {
        int isSolved = 0;
        for (int i = 0; i < _add_req.size(); i++)
        {
            if (solve[i] < _add_req[i])
                isSolved = -1;
            else if (solve[i] > _add_req[i])
                return 1;
        }
        return isSolved;
    }

    int getMinAddSolve()
    {
        std::vector<int> solve(_add_req.size());
        std::vector<int> presses(_buttons.size());
        int lowest_solve = INT_MAX;
        int i = 0;
        while (true)
        {
            if (i < 0)
                break;
            else if (i < _buttons.size())
            {
                pressAddButton(solve, _buttons[i], presses[i]);
                int isSolved = isAddSolved(solve);
                if (isSolved < 0)
                    i++;
                else if (isSolved > 0)
                {
                    resetAddButton(solve, _buttons[i], presses[i]);
                    i--;
                }
                else
                {
                    int total_presses = std::accumulate(
                        presses.begin(), presses.end(), 0);
                    if (total_presses < lowest_solve)
                        lowest_solve = total_presses;
                    resetAddButton(solve, _buttons[i], presses[i]);
                    i--;
                }
            }
            else
            {
                i--;
                int times_to_press =
                    _add_req[_buttons[i][0]] - solve[_buttons[i][0]];
                int isSolved = isAddSolved(solve);
                if (isSolved == 0)
                {
                    int total_presses = std::accumulate(
                        presses.begin(), presses.end(), 0);
                    if (total_presses < lowest_solve)
                        lowest_solve = total_presses;
                }
                resetAddButton(solve, _buttons[i], presses[i]);
                i--;
            }
        }
        std::cout << lowest_solve << std::endl;
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

    int count_toggle_presses = 0;
    int64_t count_add_presses = 0;

    while (std::getline(input, line))
    {
        Machine machine(line);

        count_toggle_presses += machine.getMinToggleSolve();
        // count_add_presses += machine.getMinAddSolve();
    }

    std::cout << count_toggle_presses << std::endl;
    std::cout << count_add_presses << std::endl;
}