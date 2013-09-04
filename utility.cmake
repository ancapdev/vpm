# Copyright (c) 2013, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

# Process shorthand format <name>[@<version>][?<variant>]
# Sets version and variant if present
# Returns name
function(vpm_process_package_shorthand _outName _input)
  if(${_input} MATCHES "^([^?@]+)@([^?@]+)\\?([^?@]+)$")
    set(${_outName} ${CMAKE_MATCH_1} PARENT_SCOPE)
    vpm_set_version(${CMAKE_MATCH_1} ${CMAKE_MATCH_2})
    vpm_set_variant(${CMAKE_MATCH_1} ${CMAKE_MATCH_3})
  elseif(${_input} MATCHES "^([^?@]+)\\?([^?@]+)@([^?@]+)$")
    set(${_outName} ${CMAKE_MATCH_1} PARENT_SCOPE)
    vpm_set_variant(${CMAKE_MATCH_1} ${CMAKE_MATCH_2})
    vpm_set_version(${CMAKE_MATCH_1} ${CMAKE_MATCH_3})
  elseif(${_input} MATCHES "^([^?']+)@([^?@]+)$")
    set(${_outName} ${CMAKE_MATCH_1} PARENT_SCOPE)
    vpm_set_version(${CMAKE_MATCH_1} ${CMAKE_MATCH_2})
  elseif(${_input} MATCHES "^([^?@]+)\\?([^?@]+)$")
    set(${_outName} ${CMAKE_MATCH_1} PARENT_SCOPE)
    vpm_set_variant(${CMAKE_MATCH_1} ${CMAKE_MATCH_2})
  elseif(${_input} MATCHES "^([^?@]+)$")
    set(${_outName} ${CMAKE_MATCH_1} PARENT_SCOPE)
  else()
    message(FATAL_ERROR "Invalid package name: ${_p}")
  endif()
endfunction()

# Get package name from CMAKE_CURRENT_LIST_DIR. Must be called from a valid package directory.
function(vpm_get_current_list_dir_package _outDir)
  get_filename_component(_parentPath ${CMAKE_CURRENT_LIST_DIR} PATH)
  get_filename_component(_parentName ${_parentPath} NAME)
  set(${_outDir} ${_parentName} PARENT_SCOPE)
endfunction()

# Get binary directory for a given package
function(vpm_get_package_binary_dir _outBinDir _name)
  vpm_get_version(_version ${_name})
  vpm_get_variant(_variant ${_name})

  # convert to cmake path aka path which is guaranteed to have '/' as delimiter
  file(TO_CMAKE_PATH ${_version} _subdir)
  # strip trailing slashes
  string(REGEX REPLACE "/+$" "" ${_subdir} "${_subdir}")
  get_filename_component(_subdir ${_subdir} NAME)
  
  if(NOT "${_variant}" STREQUAL "")
    set(_subdir "${_subdir}-${_variant}")
  endif()

  set(${_outBinDir} "${CMAKE_BINARY_DIR}/${_name}/${_subdir}" PARENT_SCOPE)
endfunction()

