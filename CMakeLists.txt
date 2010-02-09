project(CocoonGit)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(KDE4 REQUIRED)
include (KDE4Defaults)

include_directories(
	${KDE4_INCLUDES}
	${QT_INCLUDES}
)

add_subdirectory( doc )
add_subdirectory( src )
add_subdirectory( tests )
