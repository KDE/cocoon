
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
	Head.cpp
	LooseStorage.cpp
	ObjectStorage.cpp
	PackedStorage.cpp
	RawObject.cpp
	Ref.cpp
	Repo.cpp
	Status.cpp
	Tree.cpp
)

kde4_add_library(CocoonGit
	SHARED ${cocoon_git_LIB_SRCS}
)

target_link_libraries(CocoonGit
	${KDE4_KFILE_LIBS}
	${QT_LIBRARIES}
)

set_target_properties(CocoonGit PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})

install(TARGETS CocoonGit ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

# install header files
install( FILES
	Blob.h
	CloneRepositoryProcess.h
	Commit.h
	Head.h
	RawObject.h
	Ref.h
	Ref_p.h
	Repo.h
	Status.h
	Tree.h
	DESTINATION ${INCLUDE_INSTALL_DIR}/Git COMPONENT Devel
)