/*
 * cocoonview.cpp
 *
 * Copyright (C) 2008 %{AUTHOR} <%{EMAIL}>
 */
#include "cocoonview.h"
#include "settings.h"

#include <klocale.h>
#include <QtGui/QLabel>

CocoonView::CocoonView(QWidget *)
{
    ui_cocoonview_base.setupUi(this);
    settingsChanged();
    setAutoFillBackground(true);
}

CocoonView::~CocoonView()
{

}

void CocoonView::switchColors()
{
    // switch the foreground/background colors of the label
    QColor color = Settings::col_background();
    Settings::setCol_background( Settings::col_foreground() );
    Settings::setCol_foreground( color );

    settingsChanged();
}

void CocoonView::settingsChanged()
{
    QPalette pal;
    pal.setColor( QPalette::Window, Settings::col_background());
    pal.setColor( QPalette::WindowText, Settings::col_foreground());
    ui_cocoonview_base.kcfg_sillyLabel->setPalette( pal );

    // i18n : internationalization
    ui_cocoonview_base.kcfg_sillyLabel->setText( i18n("This project is %1 days old",Settings::val_time()) );
    emit signalChangeStatusbar( i18n("Settings changed") );
}

#include "cocoonview.moc"
