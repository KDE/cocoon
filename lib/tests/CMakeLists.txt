# Used by kde4_add_unit_test to set the full path to test executables
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

MACRO(UNIT_TESTS)
	FOREACH(_className ${ARGN})
		set(_testName ${_className})
		kde4_add_unit_test(${_testName}
			TESTNAME ${_testName}
			NOGUI ${_testName}.cpp GitTestBase.cpp
		)
		target_link_libraries(${_testName}
			CocoonGit
			${QT_QTTEST_LIBRARY}
			${QT_QTCORE_LIBRARY}
			${KDE4_KDEUI_LIBS}
		)
	ENDFOREACH(_className)
ENDMACRO(UNIT_TESTS)

unit_tests(
	IdTest

# Repo
	RepoCommitsCachingTest
	RepoHeadsCachingTest
	RepoStatusCachingTest
	RepoFileStagingTest
	RepoCommitIndexTest

# Ref tests
	HeadTest
	RepoRefsTest

# Storage and Objects
	LooseStorageTest
	LooseStorageCachingTest
	LooseStorageListingTest
	PackedStorageTest
	PackedStorageCachingTest
	PackedStorageNormalExtractionTest
	PackedStorageDeltifiedExtractionTest
	RawObjectTest
	RawObjectConversionTest
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
