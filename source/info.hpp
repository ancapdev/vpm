// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef INFO_HPP
#define INFO_HPP

#include "command.hpp"

class Info : public ICommand
{
public:
    virtual int Run(const Configuration& configuration, int argc, char** argv) const;
    virtual std::string GetName() const;
    virtual std::string GetDescription() const;
    virtual std::string GetUsage() const;
};

void PrintVersion();
void PrintCopyright();
void PrintCommands();

#endif
