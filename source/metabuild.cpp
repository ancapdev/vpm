// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "metabuild.hpp"
#include "options.hpp"
#include "paths.hpp"

#include <iostream>
#include <stdexcept>
#include <sstream>

namespace
{
    AutoRegisterCommand<Metabuild> autoRegistrator;

    bool IsMultiConfigurationGenerator(std::string const& name)
    {
        return
            name.find("Visual Studio") != std::string::npos ||
            name.find("Xcode") != std::string::npos;
    }

    std::string GetGenerator(Configuration const& configuration, Options const& options, std::size_t bits)
    {
        std::string const vsArch = bits == 32 ? "" : " Win64";

        if (std::string const* generator = options.GetValue("generator"))
        {
            if (*generator == "vs9")
                return "Visual Studio 9 2008" + vsArch;
            else if (*generator == "vs10")
                return "Visual Studio 10" + vsArch;
            else if (*generator == "vs11")
                return "Visual Studio 11" + vsArch;
            else if (*generator == "make")
                return "Unix Makefiles";
            else if (*generator == "ninja")
                return "Ninja";
            else if (*generator == "xcode")
                return "Xcode";
            else
                return *generator;
        }
        else
        {
            // Fix up Visual Studio generators that include the arch bits in the generator name
            if (configuration.defaultGenerator.find("Visual Studio") != std::string::npos)
            {
                std::size_t const win64pos = configuration.defaultGenerator.find("Win64");
                if (win64pos == std::string::npos)
                {
                    return configuration.defaultGenerator + vsArch;
                }
                else
                {
                    if (bits == 64)
                        return configuration.defaultGenerator;
                    else
                        return configuration.defaultGenerator.substr(0, win64pos - 1);
                }
            }
            else
            {
                return configuration.defaultGenerator;
            }
        }
    }

    std::string EscapeBackslash(const std::string& input)
    {
        std::string result;

        for (unsigned int i = 0; i < input.size(); ++i)
        {
            if (input[i] == '\\')
            {
                result.push_back('\\');
                result.push_back('\\');
                result.push_back('\\');
            }
            result.push_back(input[i]);
        }

        return result;
    }
}

Metabuild::Metabuild()
{
    std::set<std::string> generators;
#if defined (_WIN32)
    generators.insert("vs9");
    generators.insert("vs10");
    generators.insert("vs11");
#endif
#if defined (__APPLE__)
    generators.insert("xcode");
#endif
    generators.insert("make");
    generators.insert("ninja");
    mOptionsDesc.AddEnum("generator", "CMake generator to use", generators);

    std::set<std::string> bits;
    bits.insert("32");
    bits.insert("64");
    mOptionsDesc.AddEnum("bits", "Pointer size of target architecture", bits);

    mOptionsDesc.AddString("config_package", "Configuration package to use");
    mOptionsDesc.AddString("configurations", "Configurations to build in ';' separated list");
    mOptionsDesc.AddString("cmake_args", "Arguments passed through to CMake");
    mOptionsDesc.AddFlag("whatif", "Print commands without running anything");
}

int Metabuild::Run(Configuration const& configuration, int argc, char** argv) const
{
    Options const options(mOptionsDesc, argc, argv);

    std::stringstream cmdline;

    std::vector<char const*> const& unnamed = options.GetUnnamedValues();
    if (unnamed.empty())
    {
        std::cout << "No packages specified" << std::endl;
        return 1;
    }

    cmdline << "cmake " << configuration.frameworkDirectory;

    std::string const* bitsString = options.GetValue("bits");
    std::size_t const bits = bitsString ? (*bitsString == "32" ? 32 : 64) : sizeof(void*) * 8;
    std::string const generator = GetGenerator(configuration, options, bits);

    cmdline << " -G \"" << generator << "\"";
    
    cmdline << " -DVPM_BITS=" << bits;

    if (std::string const* configurations = options.GetValue("configurations"))
    {
        if (IsMultiConfigurationGenerator(generator))
        {
            cmdline << " -DCMAKE_CONFIGURATION_TYPES=\"" << *configurations << "\"";
        }
        else
        {
            if (configurations->find(';') != std::string::npos)
                throw std::runtime_error(generator + " generator only supports 1 configuration. Given: " + *configurations);

            cmdline << " -DCMAKE_BUILD_TYPE=" << *configurations;
        }
    }

    if (std::string const* configPackage = options.GetValue("config_package"))
        cmdline << " -DVPM_CONFIG_PACKAGE=\"" << *configPackage << "\"";
    else
        cmdline << " -DVPM_CONFIG_PACKAGE=\"" << configuration.defaultConfigPackage << "\"";

    if (!configuration.packageRoots.empty())
    {
        cmdline << " -DVPM_PACKAGE_ROOTS=\"";
        auto it = configuration.packageRoots.begin();
        cmdline << *it;
        for (++it; it != configuration.packageRoots.end(); ++it)
            cmdline << ";" << *it;
        cmdline << "\"";
    }

    cmdline << " -DVPM_BUILD_PACKAGES=\"";
    for (auto it = unnamed.begin(), end = unnamed.end(); it != end; ++it)
        cmdline << (it != unnamed.begin() ? ";" : "") << EscapeBackslash(*it);
    cmdline << "\"";
    
    if (std::string const* cmakeArgs = options.GetValue("cmake_args"))
        cmdline << " " << *cmakeArgs;

    if (options.GetValue("whatif") != nullptr)
    {
        std::cout << "[WHATIF] Executing: " << cmdline.str() << std::endl;
        return 0;
    }
    else
    {
        std::cout << "Executing: " << cmdline.str() << std::endl;
        return system(cmdline.str().c_str());
    }
}

std::string Metabuild::GetName() const
{
    return "mbuild";
}

std::string Metabuild::GetDescription() const
{
    return "Execute CMake meta build";
}

std::string Metabuild::GetUsage() const
{
    return "vpm mbuild <package1>..<packageN> [options]";
}

OptionsDesc const* Metabuild::GetOptions() const
{
    return &mOptionsDesc;
}

