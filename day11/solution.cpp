
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

class Device
{
private:
    std::string _name;
    std::vector<std::string> _outputNames;
    std::vector<Device*> _outputs;
    bool _has_out;
    std::pair<int64_t, std::pair<int64_t, int64_t>> _paths_to_out;

public:
    Device(std::string details)
        : _name()
        , _outputNames()
        , _outputs()
        , _has_out(false)
        , _paths_to_out({-1, {-1, -1}})
    {
        _name = details.substr(0, 3);
        details = details.substr(5);

        while (!details.empty())
        {
            _outputNames.push_back(details.substr(0, 3));
            details = details.substr(std::min(4, (int)details.size()));
        }
    }
    Device()
        : _name()
        , _outputNames()
        , _outputs()
        , _has_out(false)
        , _paths_to_out({-1, {-1, -1}})
    {}

    void initOutputs(std::map<std::string, Device>& devices)
    {
        for (auto& name : _outputNames)
        {
            try
            {
                Device& connect = devices.at(name);
                // connect._inputs.push_back(this);
                this->_outputs.push_back(&connect);
            }
            catch (std::out_of_range e)
            {
                if (name == "out")
                {
                    _has_out = true;
                }
            }
        }
    }

    void reset()
    {
        _paths_to_out = {-1, {-1, -1}};
    }

    const std::string& getName()
    {
        return _name;
    }

    int64_t countPaths()
    {
        if (_paths_to_out.first >= 0)
            return _paths_to_out.first;

        int64_t paths = 0;

        if (_has_out)
        {
            paths = 1;
        }

        for (auto& output : _outputs)
        {
            paths += output->countPaths();
        }
        _paths_to_out.first = paths;
        return paths;
    }

    int64_t countPaths(std::string& passthrough, int cache_index)
    {
        int64_t& cache = cache_index ? 
                _paths_to_out.second.first :
                _paths_to_out.second.second;
        if (cache >= 0)
            return cache;

        int64_t paths = 0;

        if (_has_out)
        {
            paths = 1;
        }

        if (_name != passthrough)
        {
            // continue as normal
            for (auto& output : _outputs)
            {
                paths += output->countPaths(passthrough, cache_index);
            }
        }

        cache = paths;
        return paths;
    }

    int64_t countPaths(std::pair<std::string, std::string>& passthrough)
    {
        if (_paths_to_out.first >= 0)
            return _paths_to_out.first;

        int64_t paths = 0;
        
        if (_has_out)
        {
            paths = 1;
        }

        if (_name == passthrough.first)
        {
            for (auto& output : _outputs)
            {
                paths += output->countPaths(passthrough.second, 0);
            }
        }
        else if (_name == passthrough.second)
        {
            for (auto& output : _outputs)
            {
                paths += output->countPaths(passthrough.first, 1);
            }
        }
        else
        {
            // continue as normal
            for (auto& output : _outputs)
            {
                paths += output->countPaths(passthrough);
            }
        }
        _paths_to_out.first = paths;
        return paths;
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
    std::map<std::string, Device> devices;
    
    while (std::getline(input, line))
    {
        Device device(line);
        
        devices[device.getName()] = device;
    }
    for (auto& device : devices)
        device.second.initOutputs(devices);

    // std::cout << "you -> out: " << devices["you"].countPaths() << std::endl;

    // for (auto& device : devices)
    //     device.second.reset();

    int64_t total_paths = devices["svr"].countPaths();
    std::cout << "svr -> out: " << total_paths << std::endl;

    for (auto& device : devices)
        device.second.reset();

    std::pair<std::string, std::string> passthrough{"dac", "fft"};
    int64_t not_paths = devices["svr"].countPaths(passthrough);
    total_paths -= not_paths;

    std::cout << "svr -> !(dac && fft) -> out: "
        << not_paths << std::endl;

    std::cout << "svr -> (dac && fft) -> out: "
        << total_paths << std::endl;
}