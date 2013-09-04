# Copyright (c) 2011-2013, Christian Rorvik
# Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

macro(vpm_trace _msg)
  if(VPM_VERBOSE)
    message(STATUS ${_msg})
  endif()
endmacro()
