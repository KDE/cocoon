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

#include <QWidget>

namespace Git {
	class Repo;
}

class TerminalInterfaceV2;

class QVBoxLayout;



class CommandLineWidget : public QWidget
{
	Q_OBJECT
	public:
		CommandLineWidget(QWidget *parent = 0);
		~CommandLineWidget();

	public slots:
		void setRepository(const Git::Repo *repo);
		void terminalExited();

	protected:
		/** @see QWidget::showEvent() */
		virtual void showEvent(QShowEvent* event);

	private:
		void changeDirectory(const QString& path);
		void showTerminal();

	private:
		QVBoxLayout* m_layout;
		const Git::Repo *m_repo;
		TerminalInterfaceV2* m_terminal;
		QWidget* m_terminalWidget;
};

#endif // COMMANDLINEWIDGET_H
