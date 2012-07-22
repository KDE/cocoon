# - Try to find libgit2
# Once done this will define
#
#  GIT2_FOUND - system has libgit2
#  GIT2_INCLUDE_DIR - the libgit2 include directory
#  GIT2_LIBRARY


FIND_PATH(GIT2_INCLUDE_DIR NAMES git2 PATH_SUFFIXES git2)

FIND_LIBRARY(GIT2_LIBRARY NAMES git2)

IF (GIT2_INCLUDE_DIR AND GIT2_LIBRARY)
  SET(GIT2_FOUND TRUE)
ENDIF (GIT2_INCLUDE_DIR AND GIT2_LIBRARY)


IF (GIT2_FOUND)
  IF (NOT GIT2_FIND_QUIETLY)
    MESSAGE(STATUS "Found libgit2: ${GIT2_LIBRARY}")
  ENDIF (NOT GIT2_FIND_QUIETLY)
ELSE (GIT2_FOUND)
  IF (GIT2_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find libgit2 <http://libgit2.github.com/>")
  ENDIF (GIT2_FIND_REQUIRED)
ENDIF (GIT2_FOUND)