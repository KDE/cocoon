project(Cocoon)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

find_package(KDE4 REQUIRED)
include (KDE4Defaults)

include_directories(
	${KDE4_INCLUDES}
	${QT_INCLUDES}
)

add_subdirectory( doc )
add_subdirectory( lib )
