# Copyright (c) 2011-2013, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

# Add dependencies on other packages. Variable number of arguments
macro(vpm_depend)
  foreach(_d ${ARGN})
    # Find packages
    vpm_find_package(_dir ${_d})

    # Add package to list of package to build
    get_property(_p GLOBAL PROPERTY packages)
    list(FIND _p ${_d} _index)
    if(${_index} EQUAL "-1")
      set_property(GLOBAL APPEND PROPERTY packages ${_d})
    endif()

    # Include package configuration (at most once per CMakeLists.txt)
    if(NOT "${_d}_CONFIGURED")
      vpm_trace("Adding dependency on ${_d}")
      set("${_d}_CONFIGURED" TRUE)
      if(EXISTS "${_dir}/configure.cmake")
        # !! All variables may be overwritten after this point
        include("${_dir}/configure.cmake")
      endif()
    endif()
  endforeach()
endmacro()

# Include current package's own configure.cmake
macro(vpm_depend_self)
  vpm_trace("${VPM_CURRENT_PACKAGE} depending on self")
  if(NOT "${VPM_CURRENT_PACKAGE}_CONFIGURED")
    set("${VPM_CURRENT_PACKAGE}_CONFIGURED" TRUE)
    if(NOT EXISTS "${VPM_CURRENT_PACKAGE_DIR}/configure.cmake")
      message(FATAL_ERROR "configure.cmake file not found in ${VPM_CURRENT_PACKAGE_DIR}")
    endif()
    include("${VPM_CURRENT_PACKAGE_DIR}/configure.cmake")
  endif()
endmacro()
