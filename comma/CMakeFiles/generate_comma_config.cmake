# The install-only section is empty for the build tree.
SET(comma_CONFIG_INSTALL_ONLY)

# The "use" file.
SET(comma_USE_FILE ${comma_BINARY_DIR}/CMakeFiles/use_comma.cmake)

# Library directory.
SET(comma_LIBRARY_DIRS_CONFIG ${LIBRARY_OUTPUT_PATH})

# Runtime library directory.
SET(comma_RUNTIME_LIBRARY_DIRS_CONFIG ${LIBRARY_OUTPUT_PATH})

# Binary executable directory.
SET(comma_EXECUTABLE_DIRS_CONFIG ${EXECUTABLE_OUTPUT_PATH})

#-----------------------------------------------------------------------------
# Configure comma_config.cmake for the build tree.

CONFIGURE_FILE( ${comma_SOURCE_DIR}/CMakeFiles/comma_config.cmake.in
                ${comma_BINARY_DIR}/CMakeFiles/comma_config.cmake @ONLY IMMEDIATE )

#-----------------------------------------------------------------------------
# Settings specific to the install tree.

# The "use" file.
SET(comma_USE_FILE \${comma_INSTALL_PREFIX}/${comma_INSTALL_PACKAGE_DIR}/use_comma.cmake)

# Include directories.
SET(comma_INCLUDE_DIRS_CONFIG
  \${comma_INSTALL_PREFIX}/include
  ${comma_EXTERNAL_INCLUDES}
)


# Link directories.
IF(CYGWIN AND BUILD_SHARED_LIBS)
  # In Cygwin programs directly link to the .dll files.
  SET(comma_LIBRARY_DIRS_CONFIG \${comma_INSTALL_PREFIX}/${comma_INSTALL_BIN_DIR})
ELSE(CYGWIN AND BUILD_SHARED_LIBS)
  SET(comma_LIBRARY_DIRS_CONFIG \${comma_INSTALL_PREFIX}/${comma_INSTALL_LIB_DIR})
ENDIF(CYGWIN AND BUILD_SHARED_LIBS)

# Executable and runtime library directories.
IF(WIN32)
  SET(comma_EXECUTABLE_DIRS_CONFIG \${comma_INSTALL_PREFIX}/${comma_INSTALL_BIN_DIR})
  SET(comma_RUNTIME_LIBRARY_DIRS_CONFIG \${comma_INSTALL_PREFIX}/${comma_INSTALL_BIN_DIR})
ELSE(WIN32)
  SET(comma_EXECUTABLE_DIRS_CONFIG \${comma_INSTALL_PREFIX}/${comma_INSTALL_BIN_DIR})
  SET(comma_RUNTIME_LIBRARY_DIRS_CONFIG \${comma_INSTALL_PREFIX}/${comma_INSTALL_LIB_DIR})
ENDIF(WIN32)

IF(WIN32)
  SET(comma_EXE_EXT ".exe")
ENDIF(WIN32)

#-----------------------------------------------------------------------------
# Configure comma_config.cmake for the install tree.

# Construct the proper number of GET_FILENAME_COMPONENT(... PATH)
# calls to compute the installation prefix from comma_DIR.
#STRING(REGEX REPLACE "/" ";" comma_INSTALL_PACKAGE_DIR_COUNT
#  "${comma_INSTALL_PACKAGE_DIR}")
SET(comma_CONFIG_INSTALL_ONLY "
# Compute the installation prefix from comma_DIR.
SET(comma_INSTALL_PREFIX \"\${comma_DIR}/..\")
")
#FOREACH(p ${comma_INSTALL_PACKAGE_DIR_COUNT})
#  SET(comma_CONFIG_INSTALL_ONLY
#    "${comma_CONFIG_INSTALL_ONLY}GET_FILENAME_COMPONENT(comma_INSTALL_PREFIX \"\${comma_INSTALL_PREFIX}\" PATH)\n"
#    )
#ENDFOREACH(p)

# The install tree only has one configuration.
SET(comma_CONFIGURATION_TYPES_CONFIG)

IF(CMAKE_CONFIGURATION_TYPES)
  # There are multiple build configurations.  Configure one
  # QConfig.cmake for each configuration.
  FOREACH(config ${CMAKE_CONFIGURATION_TYPES})
    SET(comma_BUILD_TYPE_CONFIG ${config})
    CONFIGURE_FILE(${comma_SOURCE_DIR}/CMakeFiles/comma_config.cmake.in
                   ${comma_BINARY_DIR}/Utilities/${config}/comma_config.cmake
                   @ONLY IMMEDIATE)
  ENDFOREACH(config)

  # Install the config file corresponding to the build configuration
  # specified when building the install target.  The BUILD_TYPE variable
  # will be set while CMake is processing the install files.
  INSTALL(
    FILES
      "${comma_BINARY_DIR}/Utilities/\${BUILD_TYPE}/comma_config.cmake"
    DESTINATION ${comma_INSTALL_PACKAGE_DIR}
  )

ELSE(CMAKE_CONFIGURATION_TYPES)
  # There is only one build configuration.  Configure one QConfig.cmake.
  SET(comma_BUILD_TYPE_CONFIG ${CMAKE_BUILD_TYPE})
  CONFIGURE_FILE(${comma_SOURCE_DIR}/CMakeFiles/comma_config.cmake.in
                 ${comma_BINARY_DIR}/Utilities/comma_config.cmake @ONLY IMMEDIATE)

  # Setup an install rule for the config file.
  INSTALL(
    FILES
      "${comma_BINARY_DIR}/Utilities/comma_config.cmake"
    DESTINATION ${comma_INSTALL_PACKAGE_DIR}
  )
ENDIF(CMAKE_CONFIGURATION_TYPES)
