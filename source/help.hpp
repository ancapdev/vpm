// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef HELP_HPP
#define HELP_HPP

#include "command.hpp"

class Help : public ICommand
{
public:
    virtual int Run(const Configuration& configuration, int argc, char** argv) const;
    virtual std::string GetName() const;
    virtual std::string GetDescription() const;
    virtual std::string GetUsage() const;
};

void PrintOptions(OptionsDesc const& descriptor);

#endif
