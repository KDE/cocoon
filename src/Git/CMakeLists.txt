
set(git_SRCS
	Commit.cpp
	dvcsjob.cpp
	gitrunner.cpp
	Repo.cpp
	Status.cpp
)

automoc4(CocoonGit git_SRCS)
add_library(CocoonGit
	STATIC ${git_SRCS})

target_link_libraries(CocoonGit
	${QT_LIBRARIES})
