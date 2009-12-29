
set(git_SRCS
	dvcsjob.cpp
	gitrunner.cpp
	Repo.cpp
)

automoc4(CocoonGit git_SRCS)
add_library(CocoonGit
	STATIC ${git_SRCS})

target_link_libraries(CocoonGit
	${QT_LIBRARIES})
