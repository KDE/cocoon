# Used by kde4_add_unit_test to set the full path to test executables
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_BINARY_DIR}/src
)

MACRO(UNIT_TESTS)
	FOREACH(_className ${ARGN})
		set(_testName ${_className})
		kde4_add_unit_test(${_testName}
			TESTNAME cocoon-git-${_testName}
			NOGUI ${_testName}.cpp GitTestBase.cpp
		)
		target_link_libraries(${_testName}
			CocoonGit
			${QT_QTTEST_LIBRARY}
		)
	ENDFOREACH(_className)
ENDMACRO(UNIT_TESTS)

unit_tests(
# Repo
	RepoCommitsCachingTest
	RepoHeadsCachingTest
	RepoStatusCachingTest

# Ref tests
	HeadTest

# Commit tests
	CommitListingTest
	CommitMergeDetectionTest
	CommitPopulationTest
	CommitPopulationErrorsTest

# Status tests
	StatusDeletedFileTest
	StatusModifiedAddedFileTest
	StatusModifiedFileTest
	StatusNewFileTest
	StatusNewlyAddedFileTest
	StatusRemodifiedFileTest
	StatusRemovedFileTest
	StatusUpdatedFileTest

# Storage and Objects
	LooseStorageTest
	LooseStorageCachingTest
	LooseStorageListingTest
	RawObjectTest
	RawObjectHeaderExtractionTest
	RawObjectInstantiationTest
	RawObjectTypeTest
)
