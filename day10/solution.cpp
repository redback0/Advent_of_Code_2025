
#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

void printIntVector(std::vector<int> vec, std::string brackets = "()")
{
    std::cout << brackets[0];
    for (auto num : vec)
        std::cout << num << ", ";
    std::cout << "\b\b" << brackets[1] << std::endl;
}

class Machine
{
private:
    std::vector<std::vector<int>> _buttons;
    std::string _toggle_req;
    std::vector<int> _add_req;

    enum GroupCompare {
        GROUP_MATCH,
        GROUP_A_IN_B,
        GROUP_B_IN_A,
        GROUP_UNIQUE,
        GROUP_MIXED
    };

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

    GroupCompare compareGroups(
        const std::vector<int>& a,
        const std::vector<int>& b)
    {
        bool a_in_b = true;
        bool b_in_a = true;
        bool related = false;

        if (a.size() < b.size())
        {
            b_in_a = false;
            for (auto aVal : a)
            {
                if (std::find(b.begin(), b.end(), aVal) == b.end())
                    a_in_b = false;
                else
                    related = true;
            }
        }
        else
        {
            if (a.size() != b.size())
                a_in_b = false;
            for (auto bVal : b)
            {
                if (std::find(a.begin(), a.end(), bVal) == a.end())
                {
                    a_in_b = false;
                    b_in_a = false;
                }
                else
                    related = true;
            }
        }

        if (a_in_b && b_in_a)
            return GROUP_MATCH;
        if (a_in_b)
            return GROUP_A_IN_B;
        if (b_in_a)
            return GROUP_B_IN_A;
        if (related)
            return GROUP_MIXED;
        return GROUP_UNIQUE;
    }

    void removeGroup(
        std::pair<int, std::vector<int>>& remove,
        std::pair<int, std::vector<int>>& from)
    {
        from.first -= remove.first;

        for (auto button : remove.second)
        {
            from.second.erase(
                std::find(from.second.begin(), from.second.end(), button));
        }
    }

    std::vector<std::pair<int, std::vector<int>>> calcAddGroups()
    {
        std::vector<std::pair<int, std::vector<int>>> groups(_add_req.size());

        for (int i = 0; i < _add_req.size(); i++)
            groups[i].first = _add_req[i];

        for (int i = 0; i < _buttons.size(); i++)
        {
            for (int num : _buttons[i])
            {
                groups[num].second.push_back(i);
            }
        }
        
        int num_changes = 0;
        do
        {
            num_changes = 0;
            for (int a = 0; a < groups.size() - 1; a++)
            {
                for (int b = a + 1; b < groups.size(); b++)
                {
                    switch (compareGroups(groups[a].second, groups[b].second))
                    {
                        case GROUP_A_IN_B:
                            removeGroup(groups[a], groups[b]);
                            num_changes++;
                        break;
                        case GROUP_B_IN_A:
                            removeGroup(groups[b], groups[a]);
                            num_changes++;
                        break;
                        default:
                        break;
                    }
                }
            }
        } while (num_changes > 0);

        // print groups
        for (auto& group : groups)
        {
            std::cout << group.first << " ";
            printIntVector(group.second, "[]");
        }
        

        return groups;
    }

    int getMin(
        std::vector<int> presses,
        std::vector<std::pair<int, std::vector<int>>> groups,
        int button)
    {
        int min = 0;
        for (auto& group : groups)
        {
            if (*std::max_element(group.second.begin(), group.second.end()) == button)
            {
                int group_total = 0;
                for (auto group_button : group.second)
                {
                    if (button == group_button)
                        continue;
                    group_button += presses[group_button];
                }
                min = std::max(group.first - group_total, min);
                // std::cout << "got min for: " << group.first << " ";
                // printIntVector(group.second, "[]");
                // std::cout << " as: " << group.first - group_total << std::endl;
            }
        }
        return min;
    }

    int getMax(
        std::vector<int> presses,
        std::vector<std::pair<int, std::vector<int>>> groups,
        int button)
    {
        int max = INT_MAX;
        for (auto& group : groups)
        {
            if (std::find(
                    group.second.begin(),
                    group.second.end(),
                    button
                ) != group.second.end())
            {
                int group_total = 0;
                for (auto group_button : group.second)
                {
                    if (button == group_button)
                        continue;
                    group_button += presses[group_button];
                }
                max = std::min(group.first - group_total, max);
            }
        }
        return max;
    }

    int getMinAddSolve()
    {
        std::vector<int> solve(_add_req.size());
        std::vector<int> presses(_buttons.size());
        std::vector<std::pair<int, std::vector<int>>> groups = calcAddGroups();
        std::cout << "Starting add search..." << std::endl;

        std::vector<int> init_mins(presses.size());

        for (auto& group : groups)
        {
            if (group.second.size() == 1)
            {
                init_mins[group.second[0]] = group.first;
            }
        }

        for (int i = 0; i < _buttons.size(); i++)
        {
            pressAddButtonN(solve, _buttons[i], presses[i], init_mins[i]);
        }

        int lowest_solve = INT_MAX;
        int checks = 0;
        int i = _buttons.size();
        while (true)
        {
            if (i < 0)
                break;
            else if (i >= _buttons.size())
            {
                i--;
                int j = 0;
                for (; j < _add_req.size(); j++)
                    if (solve[j] < _add_req[j]) break;
                int times_to_press =
                    _add_req[_buttons[i][j]] - solve[_buttons[i][j]];
                pressAddButtonN(solve, _buttons[i], presses[i], times_to_press);
                int isSolved = isAddSolved(solve);
                checks++;

                // std::cout << "BChecking: " << i << ", ";
                // printIntVector(presses, "[]");
                // printIntVector(solve);

                if (isSolved == 0)
                {
                    int total_presses = std::accumulate(
                        presses.begin(), presses.end(), 0);
                    if (total_presses < lowest_solve)
                    {
                        lowest_solve = total_presses;
                    }
                    std::cout << "solve found: " << total_presses << std::endl;
                }
                resetAddButton(solve, _buttons[i], presses[i]);
                i--;
                if (i >= 0) pressAddButton(solve, _buttons[i], presses[i]);
            }
            else if ((presses[i] > getMax(presses, groups, i) ||
                std::accumulate(presses.begin(), presses.end(), 0) >= lowest_solve))
            {
                resetAddButton(solve, _buttons[i], presses[i]);
                pressAddButtonN(solve, _buttons[i], presses[i], init_mins[i]);
                i--;
                if (i >= 0) pressAddButton(solve, _buttons[i], presses[i]);
            }
            else
            {
                int isSolved = isAddSolved(solve);
                checks++;

                // std::cout << "AChecking: " << i << ", ";
                // printIntVector(presses, "[]");
                // printIntVector(solve);

                if (isSolved < 0)
                {
                    i = _buttons.size();
                }
                else if (isSolved > 0)
                {
                    resetAddButton(solve, _buttons[i], presses[i]);
                    pressAddButtonN(solve, _buttons[i], presses[i], init_mins[i]);
                    i--;
                    if (i >= 0) pressAddButton(solve, _buttons[i], presses[i]);
                }
                else
                {
                    int total_presses = std::accumulate(
                        presses.begin(), presses.end(), 0);
                    if (total_presses < lowest_solve)
                        lowest_solve = total_presses;
                    resetAddButton(solve, _buttons[i], presses[i]);
                    pressAddButtonN(solve, _buttons[i], presses[i], init_mins[i]);
                    i--;
                    if (i >= 0) pressAddButton(solve, _buttons[i], presses[i]);
                    std::cout << "solve found: " << total_presses << std::endl;
                }
            }
        }
        std::cout << "lowest: " << lowest_solve
            << " with " << checks << " checks\n" << std::endl;
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
        count_add_presses += machine.getMinAddSolve();
    }

    std::cout << "toggle total: " << count_toggle_presses << std::endl;
    std::cout << "add total: " << count_add_presses << std::endl;
}