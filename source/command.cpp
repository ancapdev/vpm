// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "command.hpp"

#include <algorithm>
#include <stdexcept>

namespace 
{
    std::vector<ICommand const*>& GetCommandList()
    {
        static std::vector<ICommand const*> commands;
        return commands;
    }
}

void RegisterCommand(ICommand const& command)
{
    GetCommandList().push_back(&command);
}

std::vector<ICommand const*> const& GetCommands()
{
    return GetCommandList();
}

ICommand const& GetCommand(std::string const& name)
{
    auto commands = GetCommands();
    auto it = std::find_if(commands.begin(), commands.end(), [&] (ICommand const* command) { return command->GetName() == name; });
    if (it == commands.end())
        throw std::runtime_error("Invalid command: " + name);

    return *(*it);
}


