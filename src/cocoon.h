/*
 * cocoon.h
 *
 * Copyright (C) 2008 %{AUTHOR} <%{EMAIL}>
 */
#ifndef COCOON_H
#define COCOON_H


#include <kxmlguiwindow.h>

#include "ui_prefs_base.h"

class CocoonView;
class QPrinter;
class KToggleAction;
class KUrl;

/**
 * This class serves as the main window for Cocoon.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author %{AUTHOR} <%{EMAIL}>
 * @version %{VERSION}
 */
class Cocoon : public KXmlGuiWindow
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    Cocoon();

    /**
     * Default Destructor
     */
    virtual ~Cocoon();

private slots:
    void fileNew();
    void optionsPreferences();

private:
    void setupActions();

private:
    Ui::prefs_base ui_prefs_base ;
    CocoonView *m_view;

    QPrinter   *m_printer;
    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;
};

#endif // _COCOON_H_
