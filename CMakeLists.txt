project(Cocoon)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

find_package(KDE4 REQUIRED)
include (KDE4Defaults)

find_package(CocoonGit REQUIRED)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/lib/src
	${KDE4_INCLUDES}
	${QT_INCLUDES}
)

add_subdirectory( doc )
add_subdirectory( lib )
add_subdirectory( src )
