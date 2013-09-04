# Copyright (c) 2011, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

# Variables used in the build
#
# VPM_BUILD_PACKAGES
#   the list of root packages to build
# 
# <package>_VERSION
#   version to use for a particular package
#
# VPM_PACKAGE_ROOTS
#   list of directories to search for packages
#   default is CMAKE_CURRENT_LIST_DIR/../../
#

# Need 2.8.5 for string(FIND)
cmake_minimum_required(VERSION 2.8.5)

# If no user configure script and BUILD_PACKAGES not provided through command line,
# build the vpm command line tool itself
if(NOT EXISTS "${CMAKE_BINARY_DIR}/configure.cmake" AND NOT DEFINED VPM_BUILD_PACKAGES)
  include(self_build.cmake)
else()
  include(package_build.cmake)
endif()
