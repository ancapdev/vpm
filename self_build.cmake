# Copyright (c) 2011, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

project(vpm)

include(version.cmake)

# Enable C++11 mode on unix compilers, excluding solaris
if(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
  add_definitions("-Dnullptr=0")
elseif(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-deprecated-declarations")
endif()

# Enable parallel compilation on msvc
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

# Add yaml-cpp
add_subdirectory(thirdparty/yaml-cpp-0.3.0)
include_directories(thirdparty/yaml-cpp-0.3.0/include)

# Embed current paths in source, for use in the vpm tool
configure_file(source/paths.cx_ paths.cxx)

# Embed version information
configure_File(source/version.cx_ version.cxx)

#
# Add main tool binary
#

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)

add_executable(vpm
  source/main.cpp
  source/command.cpp source/command.hpp
  source/configuration.cpp source/configuration.hpp
  source/help.cpp source/help.hpp
  source/info.cpp source/info.hpp
  source/metabuild.cpp source/metabuild.hpp
  source/options.cpp source/options.hpp
  source/pathHelpers.cpp source/pathHelpers.hpp
  source/paths.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/paths.cxx
  source/version.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/version.cxx)

target_link_libraries(vpm yaml-cpp)

install(TARGETS vpm DESTINATION bin)

#
# Install framework
#

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/ DESTINATION framework PATTERN ".bzr" EXCLUDE)
  
#
# CPack configuration
#

set(CPACK_PACKAGE_NAME vpm)
set(CPACK_PACKAGE_VERSION 0.0.1)
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 1)

include(CPack)