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

#include "CommandLineWidget.h"

#include <klibloader.h>
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
	, m_url(KUrl())
	, m_terminal(0)
	, m_terminalWidget(0)
{
	m_layout = new QVBoxLayout(this);
	m_layout->setMargin(0);
	showTerminal();
}

CommandLineWidget::~CommandLineWidget()
{
}

void CommandLineWidget::setUrl(const KUrl& url)
{
	if (!url.isValid() || (url == m_url)) {
		return;
	}

	m_url = url;

	const bool sendInput = (m_terminal != 0)
		&& (m_terminal->foregroundProcessId() == -1)
		&& isVisible();

	if (sendInput) {
		cdUrl(url);
	}
}

void CommandLineWidget::showEvent(QShowEvent* event)
{
		if (event->spontaneous()) {
			QWidget::showEvent(event);
			return;
		}

		showTerminal();

		QWidget::showEvent(event);
}

void CommandLineWidget::showTerminal()
{
		if (m_terminal == 0) {
			KPluginFactory* factory = KPluginLoader("libkonsolepart").factory();
			KParts::ReadOnlyPart* part = factory ? (factory->create<KParts::ReadOnlyPart>(this)) : 0;
			if (part != 0) {
				connect(part, SIGNAL(destroyed(QObject*)), this, SLOT(terminalExited()));
				m_terminalWidget = part->widget();
				m_layout->addWidget(m_terminalWidget);
				m_terminal = qobject_cast<TerminalInterfaceV2 *>(part);
			}
		}

		if (m_terminal != 0) {
			m_terminal->showShellInDir(m_url.toLocalFile());
			cdUrl(m_url);
			//m_terminal->sendInput("clear\n");
			m_terminalWidget->setFocus();
		}
}

QSize CommandLineWidget::sizeHint() const
{
		QSize size = QWidget::sizeHint();
		size.setHeight(200);
		return size;
}

void CommandLineWidget::terminalExited()
{
	m_terminal = 0;
	showTerminal();
}

void CommandLineWidget::cdUrl(const KUrl& url)
{
		if (url.isLocalFile()) {
			cdDirectory(url.toLocalFile());
		}
}

void CommandLineWidget::cdDirectory(const QString& dir)
{
	m_terminal->sendInput("cd " + KShell::quoteArg(dir) + '\n');
}

#include "CommandLineWidget.moc"
