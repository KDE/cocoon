/*
	Cocoon - A GUI for Git.
	Copyright (C) 2009-2012  Riyad Preukschas <riyad@informatik.uni-bremen.de>

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

#include "CommandLineWidget.h"

#include <QGit2/QGit2>

#include <klibloader.h>
#include <kservice.h>
#include <kde_terminal_interface_v2.h>
#include <KDebug>
#include <KIO/Job>
#include <KIO/JobUiDelegate>
#include <kparts/part.h>
#include <KShell>

#include <QBoxLayout>
#include <QShowEvent>

CommandLineWidget::CommandLineWidget(QWidget *parent)
	: QWidget(parent)
	, m_layout(0)
	, m_repo(0)
	, m_terminal(0)
	, m_terminalWidget(0)
{
	m_layout = new QVBoxLayout(this);
	m_layout->setMargin(0);
	createTerminal();
}

CommandLineWidget::~CommandLineWidget()
{
}

void CommandLineWidget::createTerminal()
{
        KPluginFactory* factory = 0;
        KService::Ptr service = KService::serviceByDesktopName("konsolepart");
        if (service) {
            factory = KPluginLoader(service->library()).factory();
        }
	KParts::ReadOnlyPart* part = factory ? (factory->create<KParts::ReadOnlyPart>(this)) : 0;
	if (part != 0) {
		connect(part, SIGNAL(destroyed(QObject*)), this, SLOT(terminalExited()));
		m_terminalWidget = part->widget();
		m_layout->addWidget(m_terminalWidget);
		m_terminal = qobject_cast<TerminalInterfaceV2 *>(part);
	}
}

void CommandLineWidget::setRepository(QGitRepository *repo)
{
	const bool sendInput = (m_terminal != 0)
		&& (m_terminal->foregroundProcessId() == -1)
		&& isVisible();

	m_repo = repo;

	if (sendInput) {
		changeDirectory(m_repo->workDirPath());
	}
}

void CommandLineWidget::showEvent(QShowEvent* event)
{
		if (event->spontaneous()) {
			QWidget::showEvent(event);
			return;
		}

		updateTerminal();

		QWidget::showEvent(event);
}

void CommandLineWidget::terminalExited()
{
	m_terminal = 0;
	createTerminal();
}

void CommandLineWidget::updateTerminal()
{
	m_terminal->showShellInDir(m_repo->workDirPath());
	m_terminalWidget->setFocus();
}

void CommandLineWidget::changeDirectory(const QString& dir)
{
	m_terminal->sendInput("cd " + KShell::quoteArg(dir) + '\n');
}

#include "CommandLineWidget.moc"
