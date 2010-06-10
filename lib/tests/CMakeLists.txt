# Used by kde4_add_unit_test to set the full path to test executables
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

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

# Storage and Objects
	LooseStorageTest
	LooseStorageCachingTest
	LooseStorageListingTest
	PackedStorageTest
	RawObjectTest
	RawObjectHeaderExtractionTest
	RawObjectInstantiationTest
	RawObjectTypeTest

# Object tests
	BlobTest
	CommitListingTest
	CommitMergeDetectionTest
	CommitPopulationTest
	CommitPopulationErrorsTest
	TreeTest

# Status tests
	StatusDeletedFileTest
	StatusModifiedAddedFileTest
	StatusModifiedFileTest
	StatusNewFileTest
	StatusNewlyAddedFileTest
	StatusRemodifiedFileTest
	StatusRemovedFileTest
	StatusUpdatedFileTest
)