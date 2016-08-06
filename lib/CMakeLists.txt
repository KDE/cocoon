project(libCocoonGit)

set(LIB_VERSION_MAJOR 0)
set(LIB_VERSION_MINOR 1)
set(LIB_VERSION_PATCH 0)

add_subdirectory( src )

if(BUILD_TESTS)
	add_subdirectory( tests )
endif(BUILD_TESTS)
