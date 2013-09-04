# Copyright (c) 2013, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

macro(vpm_set_variant _name _variant)
  if(VPM_VERBOSE)
    message(STATUS "Setting variant for '${_name}' to '${_variant}'")
  endif()

  set_property(GLOBAL PROPERTY "${_name}_VARIANT" ${_variant})
endmacro()

macro(vpm_set_variants _name _variant)
  vpm_set_variant(${_name} ${_variant})
  if(${ARGC} GREATER 3)
    vpm_set_variants(${ARGN})
  endif()
endmacro()

function(vpm_get_variant _variantOut _name)
  get_property(_variant GLOBAL PROPERTY "${_name}_VARIANT")
  set(${_variantOut} ${_variant} PARENT_SCOPE)
endfunction()

# Set variant if not already set
function(vpm_set_default_variant _name _variant)
  get_property(_hasVariant GLOBAL PROPERTY "${_name}_VARIANT" SET)
  if(NOT ${_hasVariant})
    vpm_set_variant(${_name} ${_variant})
  endif()
endfunction()

# Set variants if not already set
macro(vpm_set_default_variants _name _variant)
  vpm_set_default_variant(${_name} ${_variant})
  if(${ARGC} GREATER 3)
    vpm_set_default_variants(${ARGN})
  endif()
endmacro()
