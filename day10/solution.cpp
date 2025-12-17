
#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <vector>

template<typename T>
void printIntVector(std::vector<T>& vec, std::string brackets = "()")
{
    std::cout << brackets[0];
    auto num = vec.begin();
    
    if (vec.size() > 0)
    {
        if (*num == 0)
            *num = 0;
        std::cout << *num;
    }
    for (num++; num < vec.end(); num++)
    {
        if (*num == 0)
            *num = 0;
        std::cout << ", " << *num;
    }
    std::cout << brackets[1] << std::endl;
}

void printGroups(std::vector<std::pair<double, std::vector<double>>>& groups)
{
    for (auto& group : groups)
    {
        auto before = std::cout.width(8);
        std::cout << group.first << " ";
        std::cout.width(before);
        printIntVector(group.second, "[]");
    }
    std::cout << std::endl;
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

    typedef std::pair<double, std::vector<double>> t_group_pair;
    typedef std::vector<std::pair<double, std::vector<double>>> t_group;

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
        t_group_pair& from,
        t_group_pair& remove)
    {
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

    void orderGroups(t_group& groups)
    {
        std::sort(groups.rbegin(), groups.rend(),
            [](auto& g1, auto& g2){
                for (int i = 0; i < g1.second.size(); i++)
                {
                    if (g1.second[i] != 0 && g1.second[i] == g2.second[i])
                        return g1.second[i] > g2.second[i];
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

    void multGroup(
        t_group_pair& group,
        double mult)
    {
        group.first *= mult;
        for (auto& num : group.second)
        {
            num *= mult;
        }
    }

    void removeGroup(
        t_group_pair& from,
        t_group_pair& remove)
    {
        int i = 0;
        for (; i < from.second.size(); i++)
        {
            if (from.second[i] != 0 && remove.second[i] != 0)
                break;
        }

        double mult = from.second[i];
        multGroup(from, remove.second[i]);

        from.first -= remove.first * mult;

        for (int j = 0; j < from.second.size(); j++)
        {
            from.second[j] -= remove.second[j] * mult;
        }
    }

    void simplifyGroup(t_group_pair& group)
    {
        int i = 0;
        for (; i < group.second.size(); i++)
        {
            if (group.second[i] != 0)
                break;
        }
        if (i >= group.second.size())
            return;

        auto div = group.second[i];
        group.first /= group.second[i];
        for (auto& num : group.second)
        {
            num /= div;
        }
    }

    t_group calcAddGroups()
    {
        t_group groups(_add_req.size());

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
        gaussianElimination(groups);
        return groups;
    }

    void gaussianElimination(t_group& groups)
    {
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
                // if (groups[a].first < 0)
                //     simplifyGroup(groups[a]);
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
        for (auto& group : groups)
        {
            simplifyGroup(group);
            // total += group.first;
            // std::cout.width(6);
            // std::cout << group.first << " ";
            // std::cout.width();
            // printIntVector(group.second, "[]");
        }
        // std::cout << std::endl;

        // std::cout << total << std::endl;
    }

    std::vector<int> backSubstitute(t_group& groups)
    {
        std::vector<int> values(_buttons.size(), 0);

        for (int group = groups.size() - 1; group >= 0; group--)
        {
            int i = 0;
            for (; i < groups[group].second.size(); i++)
            {
                if (groups[group].second[i] != 0)
                    break;
            }
            if (i >= groups[group].second.size())
                continue;

            double prevalue = groups[group].first * groups[group].second[i];
            values[i] = prevalue + (prevalue > 0 ? 0.001 : -0.001);

            for (int j = 1; j + i < groups[group].second.size(); j++)
            {
                values[i] -= values[i + j] * groups[group].second[i + j];
            }
        }

        return values;
    }

    bool checkPostive(const std::vector<int>& presses)
    {
        for (auto press : presses)
            if (press < 0)
                return false;
        return true;
    }

    bool checkPressesSolve(const std::vector<int>& presses)
    {
        std::vector<int> solve(_add_req.size());
        int temp = 0;
        for (int i = 0; i < presses.size(); i++)
        {
            pressAddButtonN(solve, _buttons[i], temp, presses[i]);
            if (presses[i] < 0)
                std::cout << "o7" << std::endl;
        }
        return isAddSolved(solve) == 0;
    }

    int getMinAddSolve()
    {
        t_group groups = calcAddGroups();
        // printGroups(groups);
        int temp = 0;

        std::map<int, int> unknowns;

        // find unknowns
        for (auto& group : groups)
        {
            int i = 0;
            for (; i < group.second.size(); i++)
                if (group.second[i] != 0)
                    break;

            for (i++; i < group.second.size(); i++)
            {
                if (group.second[i] != 0)
                    unknowns.insert({i, INT_MAX});
            }
        }

        t_group extra_groups;

        // create groups for unknowns
        for (auto& unknown : unknowns)
        {
            std::vector<double> new_group(_buttons.size());

            new_group[unknown.first] = 1;
            extra_groups.push_back({0, new_group});

            for (auto req : _buttons[unknown.first])
            {
                unknown.second = std::min(_add_req[req], unknown.second);
            }
        }

        std::vector<int> presses;
        std::vector<int> best_presses;
        auto groups_save = groups;

        // iterate through all possible values of unknowns
        auto unknown = unknowns.begin();
        auto ex_group = extra_groups.begin();
        int lowest_solve = INT_MAX;
        if (unknown != unknowns.end())
        {
            for (;;)
            {
                while (ex_group->first <= unknown->second)
                {
                    groups.insert(groups.end(), extra_groups.begin(), extra_groups.end());
                    gaussianElimination(groups);
                    presses = backSubstitute(groups);
                    int total_presses = std::accumulate(presses.begin(), presses.end(), 0);
                    if (total_presses < lowest_solve &&
                        checkPostive(presses) &&
                        checkPressesSolve(presses))
                    {
                        best_presses = presses;
                        lowest_solve = total_presses;
                        // printGroups(groups);
                    }
                    groups = groups_save;
                    ex_group->first++;
                }

                ex_group->first = 0;

                unknown++;
                ex_group++;

                while (ex_group < extra_groups.end() &&
                    ex_group->first >= unknown->second)
                {
                    ex_group->first = 0;
                    unknown++;
                    ex_group++;
                }
                if (ex_group >= extra_groups.end())
                    break;
                ex_group->first++;
                unknown = unknowns.begin();
                ex_group = extra_groups.begin();
            }
        }
        else
        {
            best_presses = backSubstitute(groups);
            lowest_solve = std::accumulate(
                best_presses.begin(), best_presses.end(), 0);
        }

        std::vector<int> solve(_add_req.size());
        for (int i = 0; i < best_presses.size(); i++)
        {
            pressAddButtonN(solve, _buttons[i], temp, best_presses[i]);
            if (best_presses[i] < 0)
                std::cout << "o7" << std::endl;
        }

        if (isAddSolved(solve) != 0)
            std::cout << "uh oh" << std::endl;

        // std::cout << "Presses: ";
        // printIntVector(best_presses, "{}");

        // std::cout << "Solve: ";
        // printIntVector(solve);

        // std::cout << "Req:   ";
        // printIntVector(_add_req);

        // std::cout << "lowest: " << lowest_solve << std::endl;
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

    int i = 1;

    while (std::getline(input, line))
    {
        // std::cout << "Machine " << i++ << std::endl;
        Machine machine(line);

        count_toggle_presses += machine.getMinToggleSolve();
        count_add_presses += machine.getMinAddSolve();
        // std::cout << std::endl;
    }

    std::cout << "toggle total: " << count_toggle_presses << std::endl;
    std::cout << "add total: " << count_add_presses << std::endl;
}