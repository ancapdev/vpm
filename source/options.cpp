// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "options.hpp"

#include <stdexcept>

void OptionsDesc::AddFlag(std::string const& name, std::string const& description)
{
    Desc const desc = { TYPE_FLAG, name, description, std::set<std::string>() };
    Add(desc);
}

void OptionsDesc::AddString(std::string const& name, std::string const& description)
{
    Desc const desc = { TYPE_STRING, name, description, std::set<std::string>() };
    Add(desc);
}

void OptionsDesc::AddEnum(std::string const& name, std::string const& description, std::set<std::string> const& values)
{
    Desc const desc = { TYPE_ENUM, name, description, values };
    Add(desc);
}

void OptionsDesc::Add(Desc const& descriptor)
{
    if (!mDescriptors.insert(std::make_pair(descriptor.name, descriptor)).second)
        throw std::runtime_error("Option " + descriptor.name + " already in descriptor");
}

OptionsDesc::Desc const* OptionsDesc::Find(std::string const& nameOrPrefix) const
{
    DescriptorMap::const_iterator foundIt = mDescriptors.find(nameOrPrefix);
    if (foundIt != mDescriptors.end())
        return &foundIt->second;

    // Search for option by shorthand prefix. Must be unique prefix
    for (DescriptorMap::const_iterator it = mDescriptors.begin(); it != mDescriptors.end(); ++it)
    {
        if (it->first.length() > nameOrPrefix.length() &&
            it->first.compare(0, nameOrPrefix.length(), nameOrPrefix, 0, nameOrPrefix.length()) == 0)
        {
            if (foundIt != mDescriptors.end())
                return nullptr; // ambiguous prefix

            foundIt = it;
        }
    }

    return foundIt == mDescriptors.end() ? nullptr : &foundIt->second;
}

OptionsDesc::Desc const* OptionsDesc::FindForValue(std::string const& value) const
{
    Desc const* found = nullptr;
    for (DescriptorMap::const_iterator it = mDescriptors.begin(); it != mDescriptors.end(); ++it)
    {
        if (it->second.type == TYPE_ENUM && it->second.values.count(value) != 0)
            if (found)
                return nullptr; // ambiguous value
            else
                found = &it->second;
    };
    return found;
}

Options::Options(OptionsDesc const& descriptors, int argc, char** argv)
{
    for (int i = 0; i < argc; ++i)
    {
        if (*argv[i] == '-')
        {
            // Option
            std::string const option(argv[i]);
            std::size_t const eqPos = option.find('=');
            if (eqPos == std::string::npos)
            {
                // Must be flag option
                std::string const optionName = option.substr(1, std::string::npos);

                OptionsDesc::Desc const* descriptor = descriptors.Find(optionName);

                if (descriptor == nullptr)
                    throw std::runtime_error("Invalid option: " + optionName);

                if (descriptor->type != OptionsDesc::TYPE_FLAG)
                    throw std::runtime_error("Malformed option: " + option);

                mNamed[descriptor->name] = "";
            }
            else
            {
                // Must be enum or string option

                std::string const optionName = option.substr(1, eqPos - 1);
                OptionsDesc::Desc const* descriptor = descriptors.Find(optionName);
                if (descriptor == nullptr)
                    throw std::runtime_error("Invalid option: " + optionName);

                std::string const optionValue = option.substr(eqPos + 1);
                if (descriptor->type == OptionsDesc::TYPE_ENUM &&
                    descriptor->values.count(optionValue) == 0)
                    throw std::runtime_error("Invalid value: " + optionValue + ", for option: " + optionName);

                if (descriptor->type == OptionsDesc::TYPE_FLAG)
                    throw std::runtime_error("Malformed option: " + option);
                    
                // Silently overrides any previously set value!
                mNamed[descriptor->name] = optionValue;
            }
        }
        else if (*argv[i] == '+')
        {
            // Implicit option value
            std::string const optionValue(argv[i] + 1);
            if (OptionsDesc::Desc const* descriptor = descriptors.FindForValue(optionValue))
            {
                // Silently overrides any previously set value!
                mNamed[descriptor->name] = optionValue;
            }
            else
            {
                throw std::runtime_error("Invalid implicit option value: " + optionValue);
            }
        }
        else
        {
            // Unnamed
            mUnnamed.push_back(argv[i]);
        }
    }
}

std::string const* Options::GetValue(std::string const& name) const
{
    std::map<std::string, std::string>::const_iterator it = mNamed.find(name);
    return it != mNamed.end() ? &it->second : nullptr;
}

std::vector<char const*> const& Options::GetUnnamedValues() const
{
    return mUnnamed;
}
