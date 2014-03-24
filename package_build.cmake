# Copyright (c) 2011-2014, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

include(debug.cmake)
include(variant.cmake)
include(version.cmake)

#
# Platform properties
#
if(WIN32)
  set(VPM_PLATFORM_NAME "win32")
elseif(APPLE)
  set(VPM_PLATFORM_NAME "darwin")
elseif(UNIX)
  set(VPM_PLATFORM_NAME "linux")
else()
  message(FATAL_ERROR "Unknown platform")
endif()

if(NOT DEFINED VPM_BITS)
  if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set(VPM_BITS 64)
  else()
    set(VPM_BITS 32)
  endif()
endif()

#
# Install configuration
#
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/stage/")
set(CMAKE_DEBUG_POSTFIX "_d")
set(CMAKE_RELEASE_POSTFIX "_r")
set(CMAKE_RELWITHDEBINFO_POSTFIX "_rd")
set(CMAKE_MINSIZEREL_POSTFIX "_rs")

#
# Set up utility macros for adding libraries, executables, dependencies, and include directories
#
macro(vpm_add_library _name)
  add_library(${_name} ${ARGN})
  install(TARGETS ${_name} DESTINATION "lib${VPM_BITS}")
endmacro()

macro(vpm_add_executable _name)
  add_executable(${_name} ${ARGN})
endmacro()

macro(vpm_add_link_dependencies _name)
  get_target_property(_type ${_name} TYPE)
  if (${_type} STREQUAL "STATIC_LIBRARY")
    target_link_libraries(${_name} INTERFACE ${ARGN})
  else()
    target_link_libraries(${_name} PUBLIC ${ARGN})
  endif()
endmacro()

macro(vpm_add_build_dependencies _name)
  add_dependencies(${_name} ${ARGN})
endmacro()

macro(vpm_add_build_and_link_dependencies _name)
  target_link_libraries(${_name} PUBLIC ${ARGN})
endmacro()

macro(vpm_include_directories)
  # Forward to cmake built-in
  include_directories(${ARGN})
  # Add to install target
  foreach(_dir ${ARGN})
    # Normalize path (will have all forward slashes and no trailing spaces)
    get_filename_component(_path ${_dir} ABSOLUTE)
    list(FIND VPM_INSTALLED_DIRECTORIES ${_path} _index)
    if(${_index} EQUAL -1)
      install(DIRECTORY "${_path}/" DESTINATION "include")
      list(APPEND VPM_INSTALLED_DIRECTORIES ${_path})
    endif()
  endforeach()
  set(VPM_INSTALLED_DIRECTORIES ${VPM_INSTALLED_DIRECTORIES} PARENT_SCOPE)
endmacro()

#
# Load user config
#
if(EXISTS "${CMAKE_BINARY_DIR}/configure.cmake")
  message(STATUS "Loading user configuration file: ${CMAKE_BINARY_DIR}/configure.cmake")
  include("${CMAKE_BINARY_DIR}/configure.cmake")
endif()

#
# Set output paths for shared library builds
#
if(${BUILD_SHARED_LIBS})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
endif()

#
# Set package roots
#
if(NOT DEFINED VPM_PACKAGE_ROOTS)
  get_filename_component(_root "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
  set(VPM_PACKAGE_ROOTS ${_root})
endif()

#
# Dependency machinery. Must be included only after package roots are set.
#
include(find.cmake)
include(depend.cmake)
include(utility.cmake)

#
# Load config package, to set up compiler flags etc
#
if(NOT DEFINED VPM_CONFIG_PACKAGE)
  set(VPM_CONFIG_PACKAGE vpm.config)
endif()

vpm_process_package_shorthand(VPM_CONFIG_PACKAGE_NAME ${VPM_CONFIG_PACKAGE})
vpm_set_default_version(${VPM_CONFIG_PACKAGE_NAME} master)
vpm_get_version(_configVersion ${VPM_CONFIG_PACKAGE_NAME})

vpm_find_package(_configDir ${VPM_CONFIG_PACKAGE_NAME})
vpm_trace("Adding version '${_configVersion}' of '${VPM_CONFIG_PACKAGE_NAME}' from ${_configDir}")
include("${_configDir}/configure.cmake")

#
# Get versions and variants from VPM_BUILD_PACKAGES
#
unset(_buildPackages)
foreach(_p ${VPM_BUILD_PACKAGES})
  vpm_process_package_shorthand(_name ${_p})
  message(STATUS "Adding package: ${_name}")
  list(APPEND _buildPackages ${_name})
endforeach()
set(VPM_BUILD_PACKAGES ${_buildPackages})

#
# Iterate over packages and run their CMake scripts
#
set(VPM_LAST_PACKAGE_ITERATION FALSE)
set_property(GLOBAL PROPERTY packages ${VPM_BUILD_PACKAGES})
set(VPM_CURRENT_PACKAGE_IS_ROOT TRUE)
while(TRUE)
  get_property(VPM_BUILD_PACKAGES GLOBAL PROPERTY packages)
  foreach(VPM_CURRENT_PACKAGE ${VPM_BUILD_PACKAGES})
    if(NOT "${VPM_CURRENT_PACKAGE}_INCLUDED")
      vpm_find_package(VPM_CURRENT_PACKAGE_DIR ${VPM_CURRENT_PACKAGE})

      if(NOT ${VPM_LAST_PACKAGE_ITERATION} AND "${VPM_CURRENT_PACKAGE}_INCLUDE_LAST")
        # Already deferred, do nothing (handled separate to avoid redundant file system checks)
      elseif(NOT ${VPM_LAST_PACKAGE_ITERATION} AND EXISTS "${VPM_CURRENT_PACKAGE_DIR}/vpm_include_last")
        vpm_trace("Deferring ${VPM_CURRENT_PACKAGE} include to last")
        set("${VPM_CURRENT_PACKAGE}_INCLUDE_LAST" TRUE)
      else()
        # Add package
        vpm_trace("Adding ${VPM_CURRENT_PACKAGE} from ${VPM_CURRENT_PACKAGE_DIR}")
        if(EXISTS "${VPM_CURRENT_PACKAGE_DIR}/CMakeLists.txt")
          vpm_get_package_binary_dir(_bindir ${VPM_CURRENT_PACKAGE})
          add_subdirectory(${VPM_CURRENT_PACKAGE_DIR} ${_bindir})
        endif()
        set("${VPM_CURRENT_PACKAGE}_INCLUDED" TRUE)
      endif()
    endif()
  endforeach()

  list(LENGTH VPM_BUILD_PACKAGES _len1)
  get_property(VPM_BUILD_PACKAGES GLOBAL PROPERTY packages)
  list(LENGTH VPM_BUILD_PACKAGES _len2)
  if(${_len1} EQUAL ${_len2})
    if(NOT VPM_LAST_PACKAGE_ITERATION)
      set(VPM_LAST_PACKAGE_ITERATION TRUE)
    else()
      break()
    endif()
  endif()

  set(VPM_CURRENT_PACKAGE_IS_ROOT FALSE)
endwhile()


#
# Display build info
#
macro(__format_summary _message _first)
  message(STATUS "${_message} ${_first}")
  foreach(_i ${ARGN})
    message(STATUS "                                   ${_i}")
  endforeach()
endmacro()

macro(__pad _string _toLength)
  string(LENGTH ${${_string}} _len)
  while(${_len} LESS ${_toLength})
    set(${_string} "${${_string}} ")
    string(LENGTH ${${_string}} _len)
  endwhile()
endmacro()

list(SORT VPM_BUILD_PACKAGES)
foreach(_p ${VPM_BUILD_PACKAGES})
  vpm_get_version(_version ${_p})
  vpm_get_variant(_variant ${_p})
  __pad(_p 30)
  list(APPEND packageInfos "${_p} ${_version} ${_variant}")
endforeach()

set(_configName ${VPM_CONFIG_PACKAGE_NAME})
__pad(_configName 30)

message(STATUS "Build info:")
message(STATUS "    Arch bits:                     ${VPM_BITS}")
message(STATUS "    Build Type:                    ${CMAKE_BUILD_TYPE}")
message(STATUS "    Compiler flags:               ${CMAKE_CXX_FLAGS}") # Seems to always start with a space
message(STATUS "    Compiler flags Debug:          ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "    Compiler flags Release:        ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "    Compiler flags RelWithDebInfo: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
message(STATUS "    Compiler flags MinSizeRel:     ${CMAKE_CXX_FLAGS_MINSIZEREL}")
__format_summary("    Package roots:                " ${VPM_PACKAGE_ROOTS})
message(STATUS "    Config package:                ${_configName} ${_configVersion}")
__format_summary("    Packages:                     " ${packageInfos})

