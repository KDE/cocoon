/*
 * cocoonview.h
 *
 * Copyright (C) 2007 %{AUTHOR} <%{EMAIL}>
 */
#ifndef COCOONVIEW_H
#define COCOONVIEW_H

#include <QtGui/QWidget>

#include "ui_cocoonview_base.h"

class QPainter;
class KUrl;

/**
 * This is the main view class for Cocoon.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 *
 * @short Main view
 * @author %{AUTHOR} <%{EMAIL}>
 * @version %{VERSION}
 */

class CocoonView : public QWidget, public Ui::cocoonview_base
{
    Q_OBJECT
public:
    /**
     * Default constructor
     */
    CocoonView(QWidget *parent);

    /**
     * Destructor
     */
    virtual ~CocoonView();

private:
    Ui::cocoonview_base ui_cocoonview_base;

signals:
    /**
     * Use this signal to change the content of the statusbar
     */
    void signalChangeStatusbar(const QString& text);

    /**
     * Use this signal to change the content of the caption
     */
    void signalChangeCaption(const QString& text);

private slots:
    void switchColors();
    void settingsChanged();
};

#endif // CocoonVIEW_H
