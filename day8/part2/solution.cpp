
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

class Position
{
private:
    double _x, _y, _z;

public:
    Position()
        : _x()
        , _y()
        , _z()
    {}
    Position(double x, double y, double z)
        : _x(x)
        , _y(y)
        , _z(z)
    {}

    Position operator+(const Position& other) const
    {
        return Position(_x + other._x, _y + other._y, _z + other._z);
    }

    Position operator-(const Position& other) const
    {
        return Position(_x - other._x, _y - other._y, _z - other._z);
    }

    double getX() const
    {
        return _x;
    }

    double getDistSqr(const Position& other) const
    {
        const Position rela = *this - other;

        return (std::pow(rela._x, 2) +
                std::pow(rela._y, 2) +
                std::pow(rela._z, 2));
    }

    bool inCube(const Position& centre, double dist) const
    {
        const Position rela = *this - centre;

        return (rela._x < dist && rela._x > -dist &&
                rela._y < dist && rela._y > -dist &&
                rela._z < dist && rela._z > -dist);
    }

    friend
    std::ostream& operator<<(std::ostream& out, const Position& pos)
    {
        out << pos._x << ", " << pos._y << ", " << pos._z;
        return out;
    }
};

class Junction;

class Network
{
private:
    std::vector<Junction*> _nodes;

public:
    Network()
        : _nodes()
    {}
    Network(Junction* node)
        : _nodes()
    {
        _nodes.push_back(node);
    }

    void join(Network& other);

    auto getNumNodes()
    {
        return _nodes.size();
    }

    bool inNetwork(Junction* node)
    {
        return std::find(_nodes.begin(), _nodes.end(), node) != _nodes.end();
    }
};

class Junction
{
private:
    Position _position;
    std::vector<Junction*> _connections;
    std::shared_ptr<Network> _network;

public:
    Junction()
        : _position()
        , _connections()
        , _network()
    {}
    Junction(Position position)
        : _position(position)
        , _connections()
        , _network()
    {}

    const Position& getPosition() const
    {
        return _position;
    }

    void setNetwork(std::shared_ptr<Network>& network)
    {
        _network = network;
    }

    std::shared_ptr<Network>& getNetwork()
    {
        if (!_network)
            _network = std::shared_ptr<Network>(new Network(this));
        return _network;
    }

    void initNetwork()
    {
        if (!_network)
            _network = std::shared_ptr<Network>(new Network(this));
    }

    bool isConnected(Junction& other)
    {
        return std::find(_connections.begin(), _connections.end(), &other) != _connections.end();
    }

    void addConnection(Junction& to_connect)
    {
        _connections.push_back(&to_connect);
        to_connect._connections.push_back(this);
        _network->join(*to_connect._network);
    }

    friend
    std::ostream& operator<<(std::ostream& out, const Junction& junc)
    {
        out << junc._position;
        return out;
    }
};

void Network::join(Network& other)
{
    if (&other == this)
        return;

    std::shared_ptr<Network>& this_shared_ptr =
        ((*this->_nodes.begin())->getNetwork());

    for (auto node : other._nodes)
    {
        node->setNetwork(this_shared_ptr);
        if (std::find(_nodes.begin(), _nodes.end(), node) == _nodes.end())
            _nodes.push_back(node);
    }
}


int main(int ac, char** av)
{
    if (ac < 3)
    {
        std::cout << "Please provide input" << std::endl;
        return 1;
    }

    std::ifstream input(av[1]);
    std::string line;

    int num_connections = std::atoi(av[2]);

    std::vector<Junction> junctions;

    while (std::getline(input, line))
    {
        auto comma1 = line.find(',');
        auto comma2 = line.find(',', comma1 + 1);

        double x = std::atoi(line.c_str());
        double y = std::atoi(line.substr(comma1+1).c_str());
        double z = std::atoi(line.substr(comma2+1).c_str());
        junctions.push_back(Position(x, y, z));
    }

    for (auto& junc : junctions)
    {
        junc.initNetwork();
        std::cout << junc << std::endl;
    }

    std::map<double, std::pair<Junction*, Junction*>>
        closest_pairs;
    std::pair<Junction*, Junction*> last_connection;

    std::shared_ptr<Network> network;
    do
    {
        for (auto first = junctions.begin();
            first != junctions.end() - 1; first++)
        {
            for (auto second = first + 1;
                second != junctions.end(); second++)
            {
                if (first->isConnected(*second))
                    continue;

                double distSqr = first->getPosition()
                    .getDistSqr(second->getPosition());

                closest_pairs.emplace(
                    distSqr, std::pair(&*first, &*second)
                );

                if (closest_pairs.size() > num_connections)
                {
                    double biggest = closest_pairs.begin()->first;

                    for (auto& pair : closest_pairs)
                    {
                        if (pair.first > biggest)
                            biggest = pair.first;
                    }

                    closest_pairs.extract(biggest);
                }
            }

        };

        for (auto& pair : closest_pairs)
        {
            std::cout << '[' << *pair.second.first << "] [" << *pair.second.second << ']' << std::endl;
            pair.second.first->addConnection(*pair.second.second);
            last_connection = pair.second;

            if (junctions[0].getNetwork()->getNumNodes() >= junctions.size())
                break;
        }

        closest_pairs.clear();

        network = junctions[0].getNetwork();

        std::cout << network->getNumNodes() << std::endl;
    } while (network->getNumNodes() < junctions.size());

    std::set<std::shared_ptr<Network>> networks;

    for (auto& junction : junctions)
    {
        networks.insert(junction.getNetwork());
    }

    std::cout << "\n" << 
        (int64_t)last_connection.first->getPosition().getX() * 
        (int64_t)last_connection.second->getPosition().getX() << std::endl;
}