
add_subdirectory( Git )

set(cocoon_SRCS
	main.cpp
	CommandLineWidget.cpp
	CommitInfoWidget.cpp
	CompareWidget.cpp
	DiffWidget.cpp
	FileStatusWidget.cpp
	GitBranchesModel.cpp
	GitHistoryModel.cpp
	GitStagedFilesModel.cpp
	GitUnstagedFilesModel.cpp
	HistoryWidget.cpp
	MainWindow.cpp
	OpenRepositoryDialog.cpp
	StageWidget.cpp
)

kde4_add_ui_files(cocoon_SRCS
	CommitInfoWidget.ui
	CompareWidget.ui
	FileStatusWidget.ui
	HistoryWidget.ui
	MainWindow.ui
	OpenRepositoryDialog.ui
	StageWidget.ui
)

kde4_add_kcfg_files(cocoon_SRCS settings.kcfgc)

kde4_add_executable(cocoon ${cocoon_SRCS})

target_link_libraries(cocoon
	${KDE4_KDEUI_LIBS}
	${KDE4_KFILE_LIBS}
	${KDE4_KPARTS_LIBS}
	CocoonGit
)

install(TARGETS cocoon ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install( FILES cocoon.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES cocoon.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES cocoonui.rc  DESTINATION  ${DATA_INSTALL_DIR}/cocoon )
