// Copyright (c) 2012, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <iosfwd>
#include <string>
#include <vector>

struct Configuration
{
    Configuration();

    std::string homeDirectory;
    std::string configFileName;
    std::string frameworkDirectory;
    std::string defaultGenerator;
    std::string defaultConfigPackage;
    std::vector<std::string> packageRoots;
    std::vector<std::string> packageRepositories;
};

void UpdateConfigurationFromYaml(Configuration& config, std::istream& input);

#endif

