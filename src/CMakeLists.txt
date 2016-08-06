
set(cocoon_SRCS
	main.cpp
	CloneRepositoryDialog.cpp
	CommandLineWidget.cpp
	CommitInfoWidget.cpp
	CommitWidget.cpp
	CompareWidget.cpp
	DiffWidget.cpp
	FileStatusWidget.cpp
	GitBranchesModel.cpp
	GitFileStatusModel.cpp
	GitHistoryModel.cpp
	HistoryWidget.cpp
	MainWindow.cpp
	OpenRepositoryDialog.cpp
	StageWidget.cpp
)

kde4_add_ui_files(cocoon_SRCS
	CloneRepositoryDialog.ui
	CommitInfoWidget.ui
	CommitWidget.ui
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
	${KDE4_SOLID_LIBS}
	${KDE4_STRIGI_LIBS}
	${STRIGI_STREAMANALYZER_LIBRARY}
	${STRIGI_STREAMS_LIBRARY}
	CocoonGit
)


########### install files ###############

install(TARGETS cocoon ${INSTALL_TARGETS_DEFAULT_ARGS})

install( PROGRAMS cocoon.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES cocoon.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES cocoonui.rc  DESTINATION  ${DATA_INSTALL_DIR}/cocoon )
