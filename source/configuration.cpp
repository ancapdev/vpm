// Copyright (c) 2012-2014, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "configuration.hpp"
#include "paths.hpp"
#include "pathHelpers.hpp"

#include <yaml-cpp/yaml.h>

#include <cstdlib>
#include <istream>
#include <fstream>
#include <stdexcept>

namespace
{
	bool IsVpmConfigFilePresent(const std::string& folderPath)
	{
		if (folderPath.empty())
			return false;
		std::string configPath = folderPath + PATH_SEP_CHAR + VPM_CONFIG_FILE_NAME;
		return IsFilePresent(configPath);
	}

    std::string GetHomeDirectory()
    {
        if (char const* vpmHome = std::getenv("VPM_HOME"))
	    return vpmHome;

        if (char const* home = std::getenv("HOME"))
            return home;

#ifndef _WIN32
        return "~";
#else
        // Search LocalAppData and User Profile path. Prefer LocalAppData as
	// it is non-roaming.
	char const* localAppDir = GetKnownFolderPath(FOLDERID_LocalAppData);
	if (IsVpmConfigFilePresent(localAppDir))
	    return localAppDir;

	char const* profileDir = GetKnownFolderPath(FOLDERID_Profile);
	if (IsVpmConfigFilePresent(profileDir))
	    return profileDir;

	return !localAppDir.empty() ? localAppDir : "C:";
#endif
    }
}

Configuration::Configuration()
: homeDirectory(GetHomeDirectory())
, configFileName(homeDirectory + PATH_SEP_CHAR + VPM_CONFIG_FILE_NAME)
, frameworkDirectory(VPM_DIR)
, defaultGenerator(DEFAULT_GENERATOR)
, defaultConfigPackage("vpm.config")
, defaultBits(sizeof(void*) * 8)
{
}

void UpdateConfigurationFromYaml(Configuration& config, std::istream& input)
{
    YAML::Parser parser(input);
    YAML::Node doc;
    if (parser.GetNextDocument(doc))
    {
        if (YAML::Node const* node = doc.FindValue("framework_path"))
            *node >> config.frameworkDirectory;

        if (YAML::Node const* node = doc.FindValue("default_generator"))
            *node >> config.defaultGenerator;

        if (YAML::Node const* node = doc.FindValue("default_config_package"))
            *node >> config.defaultConfigPackage;

        if (YAML::Node const* node = doc.FindValue("default_bits"))
            *node >> config.defaultBits;

        if (YAML::Node const* roots = doc.FindValue("package_roots"))
        {
            for (std::size_t i = 0; i < roots->size(); ++i)
            {
                std::string root;
                (*roots)[i] >> root;
                config.packageRoots.push_back(root);
            }
        }

        if (parser.GetNextDocument(doc))
            throw std::runtime_error("More than 1 yaml documents in stream");
    }
}
