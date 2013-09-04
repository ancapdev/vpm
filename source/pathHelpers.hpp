#ifndef PATH_HELPERS_HPP
#define PATH_HELPERS_HPP

#include <string>

#if defined (_WIN32)
#include <shlobj.h>
#endif

extern std::string const PATH_SEP_CHAR;

#if defined (_WIN32)
std::string GetKnownFolderPath(REFKNOWNFOLDERID knownFolderId);
#endif

bool IsFilePresent(const std::string& filePath);

#endif