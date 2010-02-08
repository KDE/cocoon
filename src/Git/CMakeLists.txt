
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_BINARY_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty
)

add_subdirectory( 3rdparty )

set(cocoon_git_LIB_SRCS
	3rdparty/dvcsjob.cpp
	3rdparty/gitrunner.cpp
	CloneRepositoryProcess.cpp
	Commit.cpp
	Head.cpp
	LooseStorage.cpp
	ObjectStorage.cpp
	RawObject.cpp
	Ref.cpp
	Repo.cpp
	Status.cpp
)

kde4_add_library(CocoonGit
	STATIC ${cocoon_git_LIB_SRCS}
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
	CloneRepositoryProcess.h
	Commit.h
	Head.h
	RawObject.h
	Ref.h
	Repo.h
	Status.h
	DESTINATION ${INCLUDE_INSTALL_DIR}/Git COMPONENT Devel
)
