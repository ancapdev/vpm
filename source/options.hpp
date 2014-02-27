// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

class OptionsDesc
{
public:
    enum Type
    {
        TYPE_FLAG,
        TYPE_STRING,
        TYPE_ENUM
    };

    struct Desc
    {
        Type type;
        std::string name;
        std::string description;
        std::set<std::string> values; // Only applies for enums
    };

    void AddFlag(std::string const& name, std::string const& description);

    void AddString(std::string const& name, std::string const& description);

    /// \values null terminated
    void AddEnum(std::string const& name, std::string const& description, std::set<std::string> const& values);


    Desc const* Find(std::string const& nameOrPrefix) const;

    Desc const* FindForValue(std::string const& value) const;

    template<typename F>
    void ForEach(F f) const
    {
        for (DescriptorMap::const_iterator it = mDescriptors.begin(); it != mDescriptors.end(); ++it)
            f(it->second);
    }

private:
    typedef std::map<std::string, Desc> DescriptorMap;

    void Add(Desc const& descriptor);

    DescriptorMap mDescriptors;
};

class Options
{
public:
    Options(OptionsDesc const& desc, int argc, char** argv);

    std::string const* GetValue(std::string const& name) const;
    std::vector<char const*> const& GetUnnamedValues() const;

private:
    std::map<std::string, std::string> mNamed;
    std::vector<char const*> mUnnamed;
};

#endif
