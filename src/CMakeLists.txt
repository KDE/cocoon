set(cocoon_SRCS
   cocoon.cpp
   main.cpp
   cocoonview.cpp
 )

kde4_add_ui_files(cocoon_SRCS cocoonview_base.ui prefs_base.ui)

kde4_add_kcfg_files(cocoon_SRCS settings.kcfgc )

kde4_add_executable(cocoon ${cocoon_SRCS})

target_link_libraries(cocoon ${KDE4_KDEUI_LIBS} )

install(TARGETS cocoon ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( FILES cocoon.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES cocoon.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES cocoonui.rc  DESTINATION  ${DATA_INSTALL_DIR}/cocoon )
