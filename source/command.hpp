// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "configuration.hpp"
#include "options.hpp"

#include <string>
#include <vector>

struct ICommand
{
    virtual int Run(const Configuration& configuration, int argc, char** argv) const = 0;
    virtual std::string GetName() const = 0;
    virtual std::string GetDescription() const = 0;
    virtual std::string GetUsage() const = 0;

    virtual OptionsDesc const* GetOptions() const
    {
        return nullptr;
    }

    virtual ~ICommand() {}
};

void RegisterCommand(ICommand const& command);
std::vector<ICommand const*> const& GetCommands();
ICommand const& GetCommand(std::string const& name);

template<typename T>
struct AutoRegisterCommand
{
    AutoRegisterCommand()
    {
        RegisterCommand(command);
    }

    T command;
};

#endif
