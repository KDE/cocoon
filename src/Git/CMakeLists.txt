
add_subdirectory( tests )

set(cocoon_git_LIB_SRCS
	Commit.cpp
	dvcsjob.cpp
	gitrunner.cpp
	Repo.cpp
	Status.cpp
)

kde4_add_library(CocoonGit
	STATIC ${cocoon_git_LIB_SRCS}
)

target_link_libraries(CocoonGit
	${QT_LIBRARIES}
)

set_target_properties(CocoonGit PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})

install(TARGETS CocoonGit ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

# install header files
