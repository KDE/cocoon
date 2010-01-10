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

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Git/Repo.h"
#include "OpenRepositoryDialog.h"

#include <KAction>
#include <KActionCollection>
#include <KApplication>
#include <KStandardAction>



MainWindow::MainWindow(QWidget *parent)
	: KXmlGuiWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	setupActions();

	connect(this, SIGNAL(repositoryChanged(Git::Repo*)), ui->commandLineWidget, SLOT(setRepository(Git::Repo*)));
	connect(this, SIGNAL(repositoryChanged(Git::Repo*)), ui->compareWidget, SLOT(setRepository(Git::Repo*)));
	connect(this, SIGNAL(repositoryChanged(Git::Repo*)), ui->historyWidget, SLOT(setRepository(Git::Repo*)));
	connect(this, SIGNAL(repositoryChanged(Git::Repo*)), ui->stageWidget, SLOT(setRepository(Git::Repo*)));

	// a call to KXmlGuiWindow::setupGUI() populates the GUI
	// with actions, using KXMLGUI.
	// It also applies the saved mainwindow settings, if any, and ask the
	// mainwindow to automatically save settings if changed: window size,
	// toolbar position, icon size, etc.
	setupGUI();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void MainWindow::open()
{
	OpenRepositoryDialog openRepoDialog(this);
	openRepoDialog.exec();
	if (openRepoDialog.result() == QDialog::Accepted) {
		QString path = openRepoDialog.selectedRepositoryPath();
		setRepository(path);
	}
}

void MainWindow::reload()
{
//	ui->historyWidget->reload();
	ui->stageWidget->reload();
//	ui->compareWidget->reload();
}

void MainWindow::setRepository(const QString &repoPath)
{
	m_repo = new Git::Repo(repoPath, this);
	emit repositoryChanged(m_repo);
}

void MainWindow::setupActions()
{
	KStandardAction::quit(KApplication::instance(), SLOT(quit()), actionCollection());
/*
			# commit
				commit_action = KDE::Action.new(self) do |a|
					a.text = i18n("Commit")
					a.icon = KDE::Icon.new('git-commit')
					# TODO: make setting shortcuts work
					#a.shortcut = Qt::CTRL + Qt::Key_Return
				end
				actionCollection.add_action('commit', commit_action)
				connect(commit_action, SIGNAL('triggered(bool)'), @ui.stageWidget, SLOT('commit()'))

				stage_file_action = KDE::Action.new(self) do |a|
					a.text = i18n("Stage File to Commit")
					a.icon = KDE::Icon.new('list-add')
					# TODO: make setting shortcuts work
					#a.shortcut = Qt::CTRL + Qt::Key_S
				end
				actionCollection.add_action('file_stage', stage_file_action)
				connect(stage_file_action, SIGNAL('triggered(bool)'), @ui.stageWidget, SLOT('stageFile()'))

				unstage_file_action = KDE::Action.new(self) do |a|
					a.text = i18n("Unstage File from Commit")
					a.icon = KDE::Icon.new('list-remove')
					# TODO: make setting shortcuts work
					#a.shortcut = Qt::CTRL + Qt::Key_U
				end
				actionCollection.add_action('file_unstage', unstage_file_action)
				connect(unstage_file_action, SIGNAL('triggered(bool)'), @ui.stageWidget, SLOT('unstageFile()'))
*/

// repository
	KAction *openRepoAction = actionCollection()->addAction("repository_open", this, SLOT(open()));
	openRepoAction->setText(i18n("Open repository"));
	openRepoAction->setIcon(KIcon("folder-open"));
	openRepoAction->setShortcut(Qt::CTRL + Qt::Key_O);

	KAction *reloadRepoAction = actionCollection()->addAction("repository_reload", this, SLOT(reload()));
	reloadRepoAction->setText(i18n("Reload repository"));
	reloadRepoAction->setIcon(KIcon("view-refresh"));
	reloadRepoAction->setShortcut(Qt::Key_F5);
}

#include "MainWindow.moc"
