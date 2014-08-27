# Copyright (c) 2011-2014, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

set(VPM_FRAMEWORK_VERSION 1.08.00)

macro(vpm_set_version _name _version)
  if(VPM_VERBOSE)
    message(STATUS "Setting version for '${_name}' to '${_version}'")
  endif()

  set_property(GLOBAL PROPERTY "${_name}_VERSION" ${_version})
endmacro()

macro(vpm_set_versions _name _version)
  vpm_set_version(${_name} ${_version})
  if(${ARGC} GREATER 3)
    vpm_set_versions(${ARGN})
  endif()
endmacro()

function(vpm_get_version _versionOut _name)
  get_property(_version GLOBAL PROPERTY "${_name}_VERSION")
  if("${_version}" STREQUAL "")
    message(FATAL_ERROR "No version for package '${_name}'")
  endif()
  set(${_versionOut} ${_version} PARENT_SCOPE)
endfunction()

# Set version if not already set
function(vpm_set_default_version _name _version)
  get_property(_hasVersion GLOBAL PROPERTY "${_name}_VERSION" SET)
  if(NOT ${_hasVersion})
    vpm_set_version(${_name} ${_version})
  endif()
endfunction()

# Set versions if not already set
macro(vpm_set_default_versions _name _version)
  vpm_set_default_version(${_name} ${_version})
  if(${ARGC} GREATER 3)
    vpm_set_default_versions(${ARGN})
  endif()
endmacro()


macro(vpm_minimum_version _name _version)
  get_property(_hasChecked GLOBAL PROPERTY "${_name}-${_version}-checked" SET)
  if(NOT ${_hasChecked})
    set_property(GLOBAL PROPERTY "${_name}-${_version}-checked" TRUE)
    vpm_get_version(_foundVersion ${_name})
    if(${_foundVersion} MATCHES "^[0-9]+(\\.[0-9]+)*$")
      if(${_foundVersion} VERSION_LESS ${_version})
        message(FATAL_ERROR "Requires ${_name} version ${_version}. Found version ${_foundVersion}")
      endif()
    else()
      message(WARNING "Ignoring ambiguous version check for ${_name}. Requires minimum version ${_version}. Found version ${_foundVersion}")
    endif()
  endif()
endmacro()

macro(vpm_minimum_framework_version _version)
  if(${VPM_FRAMEWORK_VERSION} VERSION_LESS ${_version})
    message(FATAL_ERROR "Requires vpm framework version ${_version}. Found version ${VPM_FRAMEWORK_VERSION}")
  endif()
endmacro()