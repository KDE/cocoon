# Used by kde4_add_unit_test to set the full path to test executables
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${ktutorial_SOURCE_DIR}/src ${KDE4_INCLUDES})
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_BINARY_DIR}/src
)

MACRO(UNIT_TESTS)
	FOREACH(_className ${ARGN})
		set(_testName ${_className}Test)
		kde4_add_unit_test(${_testName}
			TESTNAME cocoon-git-${_testName}
			NOGUI ${_testName}.cpp
		)
		target_link_libraries(${_testName}
			CocoonGit
			${KDE4_KDEUI_LIBS}
			${QT_QTTEST_LIBRARY}
		)
	ENDFOREACH(_className)
ENDMACRO(UNIT_TESTS)

unit_tests(
	NewFileStatus
	NewlyAddedFileStatus
	ModifiedFileStatus
)
