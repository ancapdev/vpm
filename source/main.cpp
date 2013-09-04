// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "help.hpp"
#include "info.hpp"
#include "configuration.hpp"

#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

int PrintError(const std::string& error)
{
    PrintVersion();
    std::cout << std::endl;
    std::cout << error << std::endl;
    std::cout << std::endl;
    PrintCommands();
    std::cout << std::endl;
    return 1;
}

int main(int argc, char** argv)
{
    Configuration config;
    std::ifstream configFile(config.configFileName.c_str());
    if (configFile)
    {
        try
        {
            UpdateConfigurationFromYaml(config, configFile);
        }
        catch (std::exception const& e)
        {
            return PrintError("Error parsing " + config.configFileName + ": " + e.what());
        }
    }

    if (argc < 2)
        return PrintError("Invalid number of arguments");

    if (std::strcmp(argv[1], "--version") == 0)
        return Info().Run(config, argc, argv);

    try
    {
        ICommand const& command = GetCommand(argv[1]);

        try
        {
            command.Run(config, argc - 2, argv + 2);
        }
        catch (std::exception const& e)
        {
            PrintVersion();
            std::cout << std::endl;
            std::cout << e.what() << std::endl;
            std::cout << std::endl;
            std::cout << "Usage: " << command.GetUsage() << std::endl;
            std::cout << std::endl;
            if (OptionsDesc const* optionsDesc = command.GetOptions())
                PrintOptions(*optionsDesc);

            return 1;
        }
    }
    catch (std::exception const& e)
    {
        return PrintError(e.what());
    }
}
