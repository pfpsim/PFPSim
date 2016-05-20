# - Find SystemC
# This module finds if SystemC is installed and determines where the
# include files and libraries are. This code sets the following
# variables: (from kernel/sc_ver.h)
#
#  SystemC_VERSION_MAJOR      = The major version of the package found.
#  SystemC_VERSION_MINOR      = The minor version of the package found.
#  SystemC_VERSION_REV        = The patch version of the package found.
#  SystemC_VERSION            = This is set to: $major.$minor.$rev
#
# The minimum required version of SystemC can be specified using the
# standard CMake syntax, e.g. FIND_PACKAGE(SystemC 2.2)
#
# For these components the following variables are set:
#
#  SystemC_INCLUDE_DIRS             - Full paths to all include dirs.
#  SystemC_LIBRARIES                - Full paths to all libraries.
#
# Example Usages:
#  FIND_PACKAGE(SystemC)
#  FIND_PACKAGE(SystemC 2.3)
#

#=============================================================================
# Copyright 2012 GreenSocs
# Modified by Kamil Saigol for PFPSIM
#=============================================================================

message(STATUS "Searching for SystemC")

# The HINTS option should only be used for values computed from the system.
SET(_SYSTEMC_HINTS
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\SystemC\\2.2;SystemcHome]/include"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\SystemC\\2.3;SystemcHome]/include"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\SystemC\\2.3.0;SystemcHome]/include"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\SystemC\\2.3.1;SystemcHome]/include"
  ${SYSTEMC_PREFIX}/include
  ${SYSTEMC_PREFIX}/lib
  ${SYSTEMC_PREFIX}/lib-linux
  ${SYSTEMC_PREFIX}/lib-linux64
  ${SYSTEMC_PREFIX}/lib-macos
  $ENV{SYSTEMC_PREFIX}/include
  $ENV{SYSTEMC_PREFIX}/lib
  $ENV{SYSTEMC_PREFIX}/lib-linux
  $ENV{SYSTEMC_PREFIX}/lib-linux64
  $ENV{SYSTEMC_PREFIX}/lib-macos
  ${CMAKE_INSTALL_PREFIX}/include
  ${CMAKE_INSTALL_PREFIX}/lib
  ${CMAKE_INSTALL_PREFIX}/lib-linux
  ${CMAKE_INSTALL_PREFIX}/lib-linux64
  ${CMAKE_INSTALL_PREFIX}/lib-macos
  )
# Hard-coded guesses should still go in PATHS. This ensures that the user
# environment can always override hard guesses.
# Kamil: I updated these paths from those provided by GreenSocs
SET(_SYSTEMC_PATHS
  /usr/include/systemc
  /usr/lib
  /usr/lib-linux
  /usr/lib-linux64
  /usr/lib-macos
  /usr/local/lib
  /usr/local/lib-linux
  /usr/local/lib-linux64
  /usr/local/lib-macos
  /usr/local/systemc
  /usr/local/systemc/include
  /usr/local/systemc/lib
  /usr/local/systemc/lib-linux
  /usr/local/systemc/lib-linux64
  )
FIND_FILE(_SYSTEMC_VERSION_FILE
  NAMES sc_ver.h
  HINTS ${_SYSTEMC_HINTS}
  PATHS ${_SYSTEMC_PATHS}
  PATH_SUFFIXES sysc/kernel
)

EXEC_PROGRAM("cat ${_SYSTEMC_VERSION_FILE} |grep '#define SC_API_VERSION_STRING' | cut -d '_' -f 7 "
             OUTPUT_VARIABLE SystemC_MAJOR)
EXEC_PROGRAM("cat ${_SYSTEMC_VERSION_FILE} |grep '#define SC_API_VERSION_STRING' | cut -d '_' -f 8 "
             OUTPUT_VARIABLE SystemC_MINOR)
EXEC_PROGRAM("cat ${_SYSTEMC_VERSION_FILE} |grep '#define SC_API_VERSION_STRING' | cut -d '_' -f 9 "
             OUTPUT_VARIABLE SystemC_REV)

# Kamil: I added the lines below to account for differences in the sc_ver.h file
# between SystemC <= 2.3.0 and SystemC > 2.3.0
if("${SystemC_MAJOR}" MATCHES "")
  EXEC_PROGRAM("cat ${_SYSTEMC_VERSION_FILE} | grep '#define SC_VERSION_MAJOR' | egrep -o '[0-9]' "
                OUTPUT_VARIABLE SystemC_MAJOR)
endif()
if("${SystemC_MINOR}" MATCHES "")
  EXEC_PROGRAM("cat ${_SYSTEMC_VERSION_FILE} | grep '#define SC_VERSION_MINOR' | egrep -o '[0-9]' "
                OUTPUT_VARIABLE SystemC_MINOR)
endif()
if("${SystemC_REV}" MATCHES "")
  EXEC_PROGRAM("cat ${_SYSTEMC_VERSION_FILE} | grep '#define SC_VERSION_PATCH' | egrep -o '[0-9]' "
                OUTPUT_VARIABLE SystemC_REV)
endif()

set(SystemC_VERSION ${SystemC_MAJOR}.${SystemC_MINOR}.${SystemC_REV})

# Kamil: The original code (commented out) is lazy
#if("${SystemC_MAJOR}" MATCHES "2")
#  set(SystemC_FOUND TRUE)
#endif("${SystemC_MAJOR}" MATCHES "2")
if(NOT "${SystemC_MAJOR}" MATCHES "")
  set(SystemC_FOUND TRUE)
endif()

message(STATUS "SystemC version = ${SystemC_VERSION}")

FIND_PATH(SystemC_INCLUDE_DIRS
  NAMES systemc.h
  HINTS ${_SYSTEMC_HINTS}
  PATHS ${_SYSTEMC_PATHS}
)

FIND_PATH(SystemC_LIBRARY_DIRS
  NAMES libsystemc.a
  HINTS ${_SYSTEMC_HINTS}
  PATHS ${_SYSTEMC_PATHS}
)

set(SystemC_LIBRARIES ${SystemC_LIBRARY_DIRS}/libsystemc.a)

message(STATUS "SystemC library = ${SystemC_LIBRARIES}")
