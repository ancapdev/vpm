// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef METABUILD_HPP
#define METABUILD_HPP

#include "command.hpp"

class Metabuild : public ICommand
{
public:
    Metabuild();

    virtual int Run(Configuration const& configuration, int argc, char** argv) const;
    virtual std::string GetName() const;
    virtual std::string GetDescription() const;
    virtual std::string GetUsage() const;
    virtual OptionsDesc const* GetOptions() const;

private:
    OptionsDesc mOptionsDesc;
};

#endif
