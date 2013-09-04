# Copyright (c) 2011-2013, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

# Set default repositories
if(NOT DEFINED VPM_REPOSITORIES)
  set(VPM_REPOSITORIES
    "bzr checkout lp:<name>/<version><dashvar> <destination>"
    "bzr checkout lp:vpm.<name>/<version><dashvar> <destination>")
endif()

# Prevent CMake from issuing a warning about unused variable
set(_forceUsed ${VPM_REPOSITORIES})

function(vpm_download_package _outDir _name _version _variant)
  if("${_variant}" STREQUAL "")
    set(_dashvar "")
    set(_slashvar "")
  else()
    set(_dashvar "-${_variant}")
    set(_slashvar "/${_variant}")
  endif()
  list(GET VPM_PACKAGE_ROOTS 0 _root)
  set(_destination "${_root}/${_name}/${_version}${_dashvar}")
  if(EXISTS "${_destination}")
    message(FATAL_ERROR "Package already exists: ${_destination}")
  endif()
  if(NOT EXISTS "${_root}/${_name}")
    message(STATUS "Making directory ${_root}/${_name}")
    file(MAKE_DIRECTORY "${_root}/${_name}")
  endif()
  foreach(_repo ${VPM_REPOSITORIES})
    string(REPLACE "<name>" ${_name} _cmd ${_repo})
    string(REPLACE "<version>" ${_version} _cmd ${_cmd})
    string(REPLACE "<variant>" "${_variant}" _cmd ${_cmd})
    string(REPLACE "<dashvar>" "${_dashvar}" _cmd ${_cmd})
    string(REPLACE "<slashvar>" "${_slashvar}" _cmd ${_cmd})
    string(REPLACE "<destination>" ${_destination} _cmd ${_cmd})
    message(STATUS "Downloading '${_name}' version '${_version}${_dashvar}' from '${_cmd}'")
    string(REPLACE " " ";" _cmd ${_cmd})
    execute_process(
      COMMAND ${_cmd}
      RESULT_VARIABLE _result)
    if(${_result} EQUAL 0)
      set(${_outDir} ${_destination} PARENT_SCOPE)
      return()
    endif()
  endforeach()
  message(FATAL_ERROR "Failed to download package '${_name}' version '${_version}${_dashvar}'")
endfunction()

function(vpm_try_find_package _dirOut _name _version _variant)
  set(_found FALSE)
  if(IS_ABSOLUTE ${_version})
    file(TO_CMAKE_PATH "${_version}" _path)
    if (EXISTS ${_path})
      set(_dir ${_path})
      set(_found TRUE)
    else()
      message(FATAL_ERROR "Unable to find existing package '${_version}'")
    endif()
  else()
    if("${_variant}" STREQUAL "")
      set(_varver ${_version})
    else()
      set(_varver "${_version}-${_variant}")
    endif()

    foreach(_d ${VPM_PACKAGE_ROOTS})
      set(_dir "${_d}/${_name}/${_varver}")
      if(EXISTS ${_dir})
        set(_found TRUE)
        break()
      endif()
     endforeach()
   endif()

  if(${_found})
    set(${_dirOut} ${_dir} PARENT_SCOPE)
  else()
    set(${_dirOut} NOTFOUND PARENT_SCOPE)
  endif()
endfunction()

function(vpm_find_package _dirOut _name)
  vpm_get_version(_version ${_name})
  vpm_get_variant(_variant ${_name})
  vpm_try_find_package(_dir ${_name} ${_version} "${_variant}")
  if(${_dir} STREQUAL NOTFOUND)
    vpm_download_package(_dir ${_name} ${_version} "${_variant}")
  endif()
  if(NOT (EXISTS ${_dir}/configure.cmake OR EXISTS ${_dir}/CMakeLists.txt))
    message(FATAL_ERROR "${_dir} does not appear to be a valid package directory. Contains neither configure.cmake nor CMakeLists.txt")
  endif()
  set(${_dirOut} ${_dir} PARENT_SCOPE)
endfunction()
