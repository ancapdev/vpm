#include "pathHelpers.hpp"

#include <fstream>

#if defined (_WIN32)
std::string const PATH_SEP_CHAR("\\");
#else
std::string const PATH_SEP_CHAR("/");
#endif

#if defined (_WIN32)

std::string GetKnownFolderPath(REFKNOWNFOLDERID knownFolderId)
{
	wchar_t* folderPath = nullptr;
	if (SUCCEEDED(SHGetKnownFolderPath(knownFolderId, 0, NULL, &folderPath)))
	{
		std::wstring wpath(folderPath);
		CoTaskMemFree(folderPath);
		return std::string(wpath.begin(), wpath.end());
	}
	return std::string();
}

#endif

bool IsFilePresent(const std::string& filePath)
{
	std::ifstream f(filePath.c_str());
	return f.is_open();
}
