# Find the comma installation or build tree.
# If comma is not found, comma_FOUND is set to false.
# The following variables are set if comma is found.
#  comma_FOUND         - Set to true when comma is found.
#  comma_USE_FILE      - CMake file to use comma.
#  comma_MAJOR_VERSION - The comma major version number.
#  comma_MINOR_VERSION - The comma minor version number
#                       (odd non-release).
#  comma_BUILD_VERSION - The comma patch level
#                       (meaningless for odd minor).
#  comma_INCLUDE_DIRS  - Include directories for comma
#  comma_LIBRARY_DIRS  - Link directories for comma libraries
# The following cache entries must be set by the user to locate comma:
#  comma_DIR  - The directory where comma was installed
#            eg c:\program files\comma, c:\lib\comma
#            it is the directory containing subdirs (include, bin, lib, CMakeFiles)
#           This can also be the root of the build tree - the dir where the device makefiles/project files exist
#            eg c:\build\comma\

# Construct consitent error messages for use below.
SET(comma_DIR_DESCRIPTION "comma installation directory - this is either the root of the build tree, or the installed location")
SET(comma_DIR_MESSAGE "comma not found.  Set the comma_DIR cmake cache entry to the ${comma_DIR_DESCRIPTION}")

#If the location is not already known, then search.
IF ( NOT comma_DIR )
  # Get the system search path as a list.
  IF(UNIX)
    STRING(REGEX MATCHALL "[^:]+" comma_DIR_SEARKH1 "$ENV{PATH}")
  ELSE(UNIX)
    STRING(REGEX REPLACE "\\\\" "/" comma_DIR_SEARKH1 "$ENV{PATH}")
  ENDIF(UNIX)
  IF ( WIN32 )
    # Assume this path exists.
    SET ( comma_DIR_SEARKH1
      ${comma_DIR_SEARKH1}
      "C:/Program Files/comma"
    )
  ENDIF ( WIN32 )
  STRING(REGEX REPLACE "/;" ";" comma_DIR_SEARCH2 "${comma_DIR_SEARCH1}")

  SET ( comma_DIR_SEARCH "${comma_DIR_SEARCH2}" } )

  #
  # Look for an installation or build tree.
  #
  FIND_PATH(comma_DIR NAMES use_comma.cmake PATH_SUFFIXES ${SUFFIX_FOR_PATH} PATHS

     # Look for an environment variable comma_DIR.
      $ENV{comma_DIR}

      # Look in places relative to the system executable search path.
      ${comma_DIR_SEARCH}

      # Look in standard UNIX install locations.
      /usr/local/CMakeFiles
      /usr/local/lib
      /usr/lib

      # Help the user find it if we cannot.
      DOC "The ${comma_DIR_DESCRIPTION}"
  )
ENDIF ( NOT comma_DIR )
# If comma was found, load the configuration file to get the rest of the
# settings.
IF(comma_DIR)
  # Make sure the comma_config.cmake file exists in the directory provided.
  IF(EXISTS ${comma_DIR}/comma_config.cmake)
    # We found comma.  Load the settings.
    SET(comma_FOUND 1)
    INCLUDE(${comma_DIR}/comma_config.cmake)
  ENDIF(EXISTS ${comma_DIR}/comma_config.cmake)
ELSE(comma_DIR)
  # We did not find comma_.
  SET(comma_FOUND 0)
ENDIF(comma_DIR)

IF ( comma_FOUND )
  IF ( EXISTS ${comma_USE_FILE} )
    INCLUDE(${comma_USE_FILE})
  ELSE ( EXISTS ${comma_USE_FILE} )
    SET(comma_FOUND 0)
  ENDIF ( EXISTS ${comma_USE_FILE} )
ENDIF ( comma_FOUND )

IF( NOT comma_FOUND )
  MESSAGE(FATAL_ERROR ${comma_DIR_MESSAGE})
ENDIF( NOT comma_FOUND )

