#.rst:
# FindBISON
# ---------
#
# Find ``bison`` executable and provide a macro to generate custom build rules.
#
# The module defines the following variables:
#
# ``BISONCPP_EXECUTABLE``
#   path to the ``bison`` program
#
# ``BISONCPP_VERSION``
#   version of ``bison``
#
# ``BISONCPP_FOUND``
#   true if the program was found
#
# The minimum required version of ``bison`` can be specified using the
# standard CMake syntax, e.g.  ``find_package(BISON 2.1.3)``.
#
# If ``bison`` is found, the module defines the macro::
#
#   BISONCPP_TARGET(<Name> <YaccInput> <CodeOutput>
#                [COMPILE_FLAGS <flags>]
#                [VERBOSE <file>]
#                )
#
# which will create a custom rule to generate a parser.  ``<YaccInput>`` is
# the path to a yacc file.  ``<CodeOutput>`` is the name of the source file
# generated by bison.  A header file is also be generated, and contains
# the token list.
#
# The options are:
#
# ``COMPILE_FLAGS <flags>``
#   Specify flags to be added to the ``bison`` command line.
#
# ``VERBOSE <file>``
#   Tell ``bison`` to write verbose descriptions of the grammar and
#   parser to the given ``<file>``.
#
# The macro defines the following variables:
#
# ``BISONCPP_<Name>_DEFINED``
#   true is the macro ran successfully
#
# ``BISONCPP_<Name>_INPUT``
#   The input source file, an alias for <YaccInput>
#
# ``BISONCPP_<Name>_OUTPUT_SOURCE``
#   The source file generated by bison
#
# ``BISONCPP_<Name>_OUTPUT_HEADER``
#   The header file generated by bison
#
# ``BISONCPP_<Name>_OUTPUTS``
#   The sources files generated by bison
#
# ``BISONCPP_<Name>_COMPILE_FLAGS``
#   Options used in the ``bison`` command line
#
# Example usage:
#
# .. code-block:: cmake
#
#   find_package(BISON)
#   BISONCPP_TARGET(MyParser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
#                DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.h)
#   add_executable(Foo main.cpp ${BISONCPP_MyParser_OUTPUTS})

#=============================================================================
# Copyright 2009 Kitware, Inc.
# Copyright 2006 Tristan Carel
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

find_program(BISONCPP_EXECUTABLE NAMES bisonc++ DOC "path to the bison executable")
mark_as_advanced(BISONCPP_EXECUTABLE)

include(CMakeParseArguments)

if(BISONCPP_EXECUTABLE)
  # the bison commands should be executed with the C locale, otherwise
  # the message (which are parsed) may be translated
  set(_Bison_SAVED_LC_ALL "$ENV{LC_ALL}")
  set(ENV{LC_ALL} C)

  execute_process(COMMAND ${BISONCPP_EXECUTABLE} --version
    OUTPUT_VARIABLE BISONCPP_version_output
    ERROR_VARIABLE BISONCPP_version_error
    RESULT_VARIABLE BISONCPP_version_result
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  set(ENV{LC_ALL} ${_Bison_SAVED_LC_ALL})

  if(NOT ${BISONCPP_version_result} EQUAL 0)
    message(SEND_ERROR "Command \"${BISONCPP_EXECUTABLE} --version\" failed with output:\n${BISONCPP_version_error}")
  else()
    if("${BISONCPP_version_output}" MATCHES "^bisonc\\+\\+ V([^,]+)")
      set(BISONCPP_VERSION "${CMAKE_MATCH_1}")
    endif()
  endif()

  # internal macro
  macro(BISONCPP_TARGET_option_verbose Name BisonOutput filename)
    list(APPEND BISONCPP_TARGET_cmdopt "--verbose")
    get_filename_component(BISONCPP_TARGET_output_path "${BisonOutput}" PATH)
    get_filename_component(BISONCPP_TARGET_output_name "${BisonOutput}" NAME_WE)
    add_custom_command(OUTPUT ${filename}
      COMMAND ${CMAKE_COMMAND}
      ARGS -E copy
      "${BISONCPP_TARGET_output_path}/${BISONCPP_TARGET_output_name}.output"
      "${filename}"
      DEPENDS
      "${BISONCPP_TARGET_output_path}/${BISONCPP_TARGET_output_name}.output"
      COMMENT "[BISONC++][${Name}] Copying bison verbose table to ${filename}"
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    set(BISONCPP_${Name}_VERBOSE_FILE ${filename})
    list(APPEND BISONCPP_TARGET_extraoutputs
      "${BISONCPP_TARGET_output_path}/${BISONCPP_TARGET_output_name}.output")
  endmacro()

  # internal macro
  macro(BISONCPP_TARGET_option_extraopts Options)
    set(BISONCPP_TARGET_extraopts "${Options}")
    separate_arguments(BISONCPP_TARGET_extraopts)
    list(APPEND BISONCPP_TARGET_cmdopt ${BISONCPP_TARGET_extraopts})
  endmacro()

  #============================================================
  # BISONCPP_TARGET (public macro)
  #============================================================
  #
  macro(BISONCPP_TARGET Name BisonInput BisonOutput BisonClass)
    set(BISONCPP_TARGET_output_header "${CMAKE_CURRENT_SOURCE_DIR}/${BisonClass}base.h")
    set(BISONCPP_TARGET_cmdopt "")
    set(BISONCPP_TARGET_outputs "${CMAKE_CURRENT_SOURCE_DIR}/${BisonOutput}")

    # Parsing parameters
    set(BISONCPP_TARGET_PARAM_OPTIONS)
    set(BISONCPP_TARGET_PARAM_ONE_VALUE_KEYWORDS
      VERBOSE
      COMPILE_FLAGS
      )
    set(BISONCPP_TARGET_PARAM_MULTI_VALUE_KEYWORDS)
    cmake_parse_arguments(
        BISONCPP_TARGET_ARG
        "${BISONCPP_TARGET_PARAM_OPTIONS}"
        "${BISONCPP_TARGET_PARAM_ONE_VALUE_KEYWORDS}"
        "${BISONCPP_TARGET_PARAM_MULTI_VALUE_KEYWORDS}"
        ${ARGN}
    )

    if(NOT "${BISONCPP_TARGET_ARG_UNPARSED_ARGUMENTS}" STREQUAL "")
      message(SEND_ERROR "Usage")
    else()
      if(NOT "${BISONCPP_TARGET_ARG_VERBOSE}" STREQUAL "")
        BISONCPP_TARGET_option_verbose(${Name} ${BisonOutput} "${BISONCPP_TARGET_ARG_VERBOSE}")
      endif()
      if(NOT "${BISONCPP_TARGET_ARG_COMPILE_FLAGS}" STREQUAL "")
        BISONCPP_TARGET_option_extraopts("${BISONCPP_TARGET_ARG_COMPILE_FLAGS}")
      endif()

      list(APPEND BISONCPP_TARGET_cmdopt "--class-name=${BisonClass}")
      list(APPEND BISONCPP_TARGET_outputs "${BISONCPP_TARGET_output_header}")

      add_custom_command(OUTPUT ${BISONCPP_TARGET_outputs}
        ${BISONCPP_TARGET_extraoutputs}
        COMMAND ${BISONCPP_EXECUTABLE}
        ARGS ${BISONCPP_TARGET_cmdopt} --no-lines --parsefun-source=${BisonOutput} ${BisonInput}
        DEPENDS ${BisonInput}
        COMMENT "[BISONC++][${Name}] Building parser with bisonc++ ${BISONCPP_VERSION}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

      # define target variables
      set(BISONCPP_${Name}_DEFINED TRUE)
      set(BISONCPP_${Name}_INPUT ${BisonInput})
      set(BISONCPP_${Name}_OUTPUTS ${BISONCPP_TARGET_outputs})
      set(BISONCPP_${Name}_COMPILE_FLAGS ${BISONCPP_TARGET_cmdopt})
      set(BISONCPP_${Name}_OUTPUT_SOURCE "${BisonOutput}")
      set(BISONCPP_${Name}_OUTPUT_HEADER "${BISONCPP_TARGET_output_header}")

    endif()
  endmacro()
  #
  #============================================================

endif()

find_package(PackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(BISON REQUIRED_VARS  BISONCPP_EXECUTABLE
                                        VERSION_VAR BISONCPP_VERSION)

# FindBISON.cmake ends here
