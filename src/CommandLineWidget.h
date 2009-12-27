/*
	Cocoon - A GUI for Git.
	Copyright (C) 2009  Riyad Preukschas <riyad@informatik.uni-bremen.de>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef COMMANDLINEWIDGET_H
#define COMMANDLINEWIDGET_H

#include <KUrl>

#include <QWidget>

class QVBoxLayout;

class KJob;

class TerminalInterfaceV2;

class CommandLineWidget : public QWidget
{
	Q_OBJECT
	public:
		CommandLineWidget(QWidget *parent = 0);
		~CommandLineWidget();

		const KUrl& url() const;
		virtual QSize sizeHint() const;

	public slots:
		virtual void setUrl(const KUrl& url);
		void terminalExited();

	protected:
		/** @see QWidget::showEvent() */
		virtual void showEvent(QShowEvent* event);

//	private slots:
//		void slotMostLocalUrlResult(KJob* job);

	private:
		void cdDirectory(const QString& path);
		void cdUrl(const KUrl& url);
		void showTerminal();

	private:
		QVBoxLayout* m_layout;
		KUrl m_url;
		TerminalInterfaceV2* m_terminal;
		QWidget* m_terminalWidget;
};

#endif // COMMANDLINEWIDGET_H
