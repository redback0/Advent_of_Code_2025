
#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

void printIntVector(std::vector<int> vec, std::string brackets = "()")
{
    std::cout << brackets[0];
    auto num = vec.begin();
    if (vec.size() > 0)
        std::cout << *num;
    for (num++; num < vec.end(); num++)
        std::cout << ", " << *num;
    std::cout << brackets[1] << std::endl;
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

        for (int i = 0; i < a.size(); i++)
        {
            if (a[i] != 0 && b[i] == 0)
                a_in_b = false;
            else if (b[i] != 0 && a[i] == 0)
                b_in_a = false;
            else
                related = true;
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

    bool isRemoveGroup(
        std::pair<int, std::vector<int>>& from,
        std::pair<int, std::vector<int>>& remove)
    {
        // assume from < remove == false
        int i = 0;
        for (; i < from.second.size(); i++)
        {
            if (from.second[i] != 0)
                break;
        }

        if (i >= from.second.size())
            return false;

        if (remove.second[i] != 0)
            return true;

        i++;

        for (; i < from.second.size(); i++)
        {
            if (remove.second[i] != 0)
            {
                if (from.second[i] != 0)
                    return true;
                else
                    return false;
            }
        }

        return false;
    }

    void orderGroups(std::vector<std::pair<int, std::vector<int>>>& groups)
    {
        std::sort(groups.rbegin(), groups.rend(),
            [](auto& g1, auto& g2){
                for (int i = 0; i < g1.second.size(); i++)
                {
                    if (g1.second[i] != g2.second[i])
                    {
                        if (g1.second[i] == 0)
                            return true;
                        else if (g2.second[i] == 0)
                            return false;
                        return g1.second[i] < g2.second[i];
                    }
                }
                return false;
            });
    }

    void removeGroup(
        std::pair<int, std::vector<int>>& from,
        std::pair<int, std::vector<int>>& remove)
    {
        double multiplier = 0;
        for (int i = 0; i < from.second.size(); i++)
        {
            if (from.second[i] != 0 && remove.second[i] != 0)
            {
                multiplier = (double)from.second[i] / remove.second[i];
                break;
            }
        }

        from.first -= remove.first * multiplier;

        for (int i = 0; i < from.second.size(); i++)
        {
            from.second[i] -= remove.second[i] * multiplier;
        }
    }

    void simplifyGroup(std::pair<int, std::vector<int>>& group)
    {
        int i = 0;
        for (; i < group.second.size(); i++)
        {
            if (group.second[i] != 0)
                break;
        }
        if (i >= group.second.size())
            return;

        group.first /= group.second[i];
        for (int& num : group.second)
        {
            num /= group.second[i];
        }
    }

    std::vector<std::pair<int, std::vector<int>>> calcAddGroups()
    {
        std::vector<std::pair<int, std::vector<int>>> groups(_add_req.size());

        for (int i = 0; i < _add_req.size(); i++)
        {
            groups[i].first = _add_req[i];
            groups[i].second.resize(_buttons.size(), 0);
        }

        for (int i = 0; i < _buttons.size(); i++)
        {
            for (int num : _buttons[i])
            {
                groups[num].second[i] = 1;
            }
        }


        // // print groups
        // for (auto& group : groups)
        // {
        //     std::cout << group.first << " ";
        //     printIntVector(group.second, "[]");
        // }

        bool changed = 0;
        do
        {
            orderGroups(groups);
            // // print groups
            // for (auto& group : groups)
            // {
            //     std::cout << group.first << "\t";
            //     printIntVector(group.second, "[]");
            // }
            // std::cout << std::endl;
            changed = false;
            for (int a = 0; a < groups.size() - 1; a++)
            {
                for (int b = a + 1; b < groups.size(); b++)
                {
                    if (isRemoveGroup(groups[a], groups[b]))
                    {
                        removeGroup(groups[a], groups[b]);
                        changed = true;
                        break;
                    }
                }
                if (changed)
                    break;
            }

        } while (changed);

        // print groups
        // int total = 0;
        // for (auto& group : groups)
        // {
        //     simplifyGroup(group);
        //     total += group.first;
        //     std::cout << group.first << " ";
        //     printIntVector(group.second, "[]");
        // }

        // std::cout << total << std::endl;

        return groups;
    }

    int getMinAddSolve()
    {
        std::vector<std::pair<int, std::vector<int>>> groups = calcAddGroups();
        // std::cout << "Starting add search..." << std::endl;

        int lowest_solve = 0;
        for (auto& group : groups)
            lowest_solve += group.first;

        std::cout << "lowest: " << lowest_solve << std::endl;
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