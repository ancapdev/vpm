// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "help.hpp"
#include "info.hpp"

#include <iomanip>
#include <iostream>

namespace
{
    AutoRegisterCommand<Help> autoRegistrator;
}

void PrintOptions(OptionsDesc const& descriptor)
{
    std::cout << "Options:" << std::endl;
    descriptor.ForEach([] (OptionsDesc::Desc const& desc)
    {
        int const padWidth = 30;
        if (desc.type == OptionsDesc::TYPE_FLAG)
            std::cout << "    -" << std::setw(padWidth) << std::left << desc.name << desc.description << std::endl;
        else
            std::cout << "    -" << std::setw(padWidth) << std::left << desc.name + "=<value>" << desc.description << std::endl;

        if (desc.type == OptionsDesc::TYPE_ENUM)
        {
            std::cout << "     " << std::setw(padWidth) << " " << "Valid values: ";
            for (auto it = desc.values.begin(); it != desc.values.end(); ++it)
            {
                if (it != desc.values.begin())
                    std::cout << ", ";
                std::cout << *it;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    });
}

int Help::Run(const Configuration& configuration, int argc, char** argv) const
{
    if (argc != 1)
    {
        std::cout << "Invalid number of arguments" << std::endl;
        std::cout << std::endl;
        std::cout << "Help usage: vpm help <command>" << std::endl;
        std::cout << std::endl;
        PrintCommands();
        std::cout << std::endl;
        return 1;
    }

    ICommand const& command = GetCommand(argv[0]);

    std::cout << "Name: " << command.GetName() << std::endl;
    std::cout << std::endl;
    std::cout << "Description: " << command.GetDescription() << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << command.GetUsage() << std::endl;
    std::cout << std::endl;

    if (OptionsDesc const* optionsDesc = command.GetOptions())
        PrintOptions(*optionsDesc);

    std::cout << std::endl;

    return 0;
}

std::string Help::GetName() const
{
    return "help";
}

std::string Help::GetDescription() const
{
    return "Get help about a command";
}

std::string Help::GetUsage() const
{
    return "vpm help <command>";
}
