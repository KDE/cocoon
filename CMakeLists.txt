project(Cocoon)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

find_package(KDE4 REQUIRED)
include (KDE4Defaults)

find_package(CocoonGit REQUIRED)

include_directories(
	${KDE4_INCLUDES}
	${QT_INCLUDES}
	/usr/local/include
)

add_subdirectory( doc )
add_subdirectory( lib )
add_subdirectory( src )
