// Copyright (c) 2011-2014, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "info.hpp"
#include "paths.hpp"
#include "pathHelpers.hpp"
#include "version.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>

namespace
{
    AutoRegisterCommand<Info> autoRegistrator;
}


int Info::Run(const Configuration& configuration, int argc, char** argv) const
{
    PrintVersion();
    PrintCopyright();

    std::cout << std::endl;

    std::cout << "VPM Home Dir:           " << configuration.homeDirectory << std::endl;
    std::cout << "Configuration File:     " << configuration.configFileName << (IsFilePresent(configuration.configFileName) ? "" : " (Missing)") << std::endl;
    std::cout << "Framework Dir:          " << configuration.frameworkDirectory << std::endl;
    std::cout << "Default Generator:      " << configuration.defaultGenerator << std::endl;
    std::cout << "Default Config Package: " << configuration.defaultConfigPackage << std::endl;
    std::cout << "Default bits:           " << configuration.defaultBits << std::endl;
    for (auto it = configuration.packageRoots.begin(); it != configuration.packageRoots.end(); ++it)
        std::cout << "Package root:           " << *it << std::endl;

    std::cout << std::endl;

    return 0;
}

std::string Info::GetName() const
{
    return "info";
}

std::string Info::GetDescription() const
{
    return "Get info about this vpm install";
}

std::string Info::GetUsage() const
{
    return "vpm info";
}

void PrintVersion()
{
    std::cout << "Versioned Package Make (vpm) " << VPM_FRAMEWORK_VERSION << ", Built " << __DATE__ << " " << __TIME__ << std::endl;
}

void PrintCopyright()
{
    std::cout << "Copyright (c) 2011-2014, Christian Rorvik" << std::endl;
    std::cout << "Distributed under the Simplified BSD License" << std::endl;
}

void PrintCommands()
{
    std::cout << "Commands:" << std::endl;
    auto commands = GetCommands();
    std::for_each(commands.begin(), commands.end(), [] (ICommand const* command)
    {
        std::cout << "\tvpm " << std::setw(20) << std::left << command->GetName() << command->GetDescription() << std::endl;
    });
}
