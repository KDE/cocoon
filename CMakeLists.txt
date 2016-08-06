project(Cocoon)

#Project options, to provide a selective build process
#Note that some of these are mutually exclusive or just
#do not make a whole lot of sense combined.
option(BUILD_LIB "Build libCocoonGit" ON)
option(BUILD_GUI "Build the Cocoon GUI" ON)
option(BUILD_DOC "Build the Documentation" OFF)
option(BUILD_TESTS "Build the Tests" ON)
option(INSTALL_DEVEL_FILES "Install the devel files" ON)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)

find_package(ECM 1.3.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(ECMGenerateHeaders)
include(ECMInstallIcons)
include(ECMMarkNonGuiExecutable)
include(ECMOptionalAddSubdirectory)
include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt5 5.4 CONFIG REQUIRED Core Widgets Test)
find_package(KF5 REQUIRED
    Archive
    XmlGui # For app
    KIO # For part
    Solid # For part
    KDELibs4Support
    Parts
    DocTools
    I18n
)


include_directories(
       ${CMAKE_CURRENT_SOURCE_DIR}
       ${CMAKE_SOURCE_DIR}/src
       ${CMAKE_SOURCE_DIR}/lib/src
)

########## Tests ##########
if(BUILD_TESTS)
	set(KDE4_BUILD_TESTS ON)
	add_definitions(-DCMAKE_BUILD_TYPE=debug)
endif(BUILD_TESTS)

######### Library #########
if(BUILD_LIB)
	add_subdirectory( lib )
endif()

########### GUI ###########
if(BUILD_GUI)
	add_subdirectory( src )
	add_subdirectory( icons )
endif()

###### Documentation ######
if(BUILD_DOC)
	add_subdirectory( doc )
endif()

message(STATUS)
message(STATUS "========= Cocoon Build Information =========")
message(STATUS "Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Build and install libCocoonGit (BUILD_LIB): ${BUILD_LIB}")
message(STATUS "Build and install the Cocoon GUI (BUILD_GUI): ${BUILD_GUI}")
message(STATUS "Build the unit tests (BUILD_TESTS): ${BUILD_TESTS}")
message(STATUS "Build and install the useless Documentation (BUILD_DOC): ${BUILD_DOC}")
message(STATUS "Install the development files (INSTALL_DEVEL_FILES): ${INSTALL_DEVEL_FILES}")
if(INSTALL_DEVEL_FILES AND CMAKE_INSTALL_PREFIX STREQUAL "/usr")
else(INSTALL_DEVEL_FILES AND CMAKE_INSTALL_PREFIX STREQUAL "/usr")
	message(STATUS "")
	message(STATUS "Warning:")
	message(STATUS "*.cmake files can only be installed when install prefix == /usr")
	message(STATUS "")
	message(STATUS "Please change it with -DCMAKE_INSTALL_PREFIX=/usr")
endif()
message(STATUS "")
message(STATUS "To change any of these options, override them using -D{OPTION_NAME} on the commandline.")
message(STATUS "To build and install Cocoon, run \"make\" and \"make install\"")
message(STATUS)

###### INSTALL CMAKE ######
if(INSTALL_DEVEL_FILES AND CMAKE_INSTALL_PREFIX STREQUAL "/usr")
	install(FILES
		cmake/modules/FindCocoonGit.cmake
		DESTINATION
		${CMAKE_ROOT}/Modules
	)
endif()
