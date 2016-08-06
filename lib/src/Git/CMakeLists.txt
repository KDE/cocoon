
add_subdirectory( 3rdparty )

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty
)

set(cocoon_git_LIB_SRCS
	3rdparty/dvcsjob.cpp
	3rdparty/gitrunner.cpp
	Blob.cpp
	CloneRepositoryProcess.cpp
	Commit.cpp
	Id.cpp
	LooseStorage.cpp
	ObjectStorage.cpp
	PackedStorage.cpp
	PackedStorageObject.cpp
	RawObject.cpp
	Ref.cpp
	Repo.cpp
	Status.cpp
	Tree.cpp
)

add_library(CocoonGit
	SHARED ${cocoon_git_LIB_SRCS}
)

target_link_libraries(CocoonGit
    KF5::Archive
    KF5::KDELibs4Support
    Qt5::Core
)

set_target_properties(CocoonGit PROPERTIES VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH} SOVERSION ${LIB_VERSION_MAJOR})

install(TARGETS CocoonGit ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

if(INSTALL_DEVEL_FILES)
# install header files
	install( FILES
		Blob.h
		CloneRepositoryProcess.h
		Commit.h
		Id.h
		ObjectStorage.h
		RawObject.h
		Ref.h
		Repo.h
		Status.h
		Tree.h
		DESTINATION ${INCLUDE_INSTALL_DIR}/Git COMPONENT Devel
	)
endif()
