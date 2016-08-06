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

#include "OpenRepositoryDialog.h"
#include "ui_OpenRepositoryDialog.h"

#include "CloneRepositoryDialog.h"
#include "Git/Repo.h"

#include <KIcon>
#include <KMessageBox>
#include <KSharedConfig>
#include <KConfigGroup>
#include <KGlobal>

#include <QFileDialog>
#include <QListWidgetItem>
#include <QPainter>



OpenRepositoryDialog::OpenRepositoryDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::OpenRepositoryDialog)
{
	ui->setupUi(this);

	setWindowIcon(KIcon("git-repo"));

	ui->repositorySearchLine->setListWidget(ui->repositoriesListWidget);

	createActions();

	loadRepositories();
}

OpenRepositoryDialog::~OpenRepositoryDialog()
{
	delete ui;
}



void OpenRepositoryDialog::accept()
{
	saveRepositories();

	QDialog::accept();
}

void OpenRepositoryDialog::addRepository(const QString &repoPath)
{
	QListWidget *repoList = ui->repositoriesListWidget;

	QList<QListWidgetItem*> foundItems = repoList->findItems(repoPath, Qt::MatchExactly);

	// make sure we only add a repo once
	if (foundItems.isEmpty()) {
		QListWidgetItem *item = new QListWidgetItem(repoPath, repoList);
		if (Git::Repo::containsRepository(repoPath)) {
			item->setIcon(KIcon("git-repo"));
		} else {
			item->setForeground(QBrush(Qt::red));
			if (QDir(repoPath).exists()) {
				item->setIcon(KIcon("folder"));
				item->setToolTip(i18n("This directory does not contain a repository."));
			} else {
				QPixmap iconPix = KIcon("folder").pixmap(16);
				QPainter painter(&iconPix);

				QPixmap emblemPix = KIcon("emblem-important").pixmap(8);
				painter.drawPixmap(0, iconPix.height() - emblemPix.height(), emblemPix);

				item->setIcon(iconPix);
				item->setToolTip(i18n("This directory does not exist anymore."));
			}
		}
		repoList->addItem(item);
	}

	selectRepository(repoPath);
}

void OpenRepositoryDialog::createActions()
{
	ui->addButton->setIcon(KIcon("list-add"));
	ui->cloneButton->setIcon(KIcon("git-repo-clone"));
	ui->newButton->setIcon(KIcon("git-repo-new"));
	ui->removeButton->setIcon(KIcon("list-remove"));

	ui->openButton->setIcon(KIcon("git-repo-open"));

	QPushButton *quitButton = ui->quitButton;
	if (parent()) {
		quitButton->setIcon(KIcon("dialog-close"));
		quitButton->setText(i18n("Cancel"));
	} else {
		quitButton->setIcon(KIcon("application-exit"));
		quitButton->setText(i18n("Exit"));
	}
}

void OpenRepositoryDialog::loadRepositories()
{
	KSharedConfigPtr config = KGlobal::config();
	KConfigGroup cg = config->group("Repositories");

	QStringList repoPaths = cg.readPathEntry("List", QStringList());
	foreach (const QString &path, repoPaths) {
		addRepository(path);
		/** @todo check whether repo dirs still contain repos */
	}

	QString lastSelectedPath = cg.readPathEntry("Last", 0);
	if (lastSelectedPath.isNull()) {
		selectRepository(0);
	} else {
		selectRepository(lastSelectedPath);
	}
}

void OpenRepositoryDialog::on_addButton_clicked()
{
	QString path = QFileDialog::getExistingDirectory(this, i18n("Select Git repository location"));

	// make sure the user has not cancelled the file dialog
	if (!path.isEmpty()) {
		addRepository(path);
	}
}

void OpenRepositoryDialog::on_cloneButton_clicked()
{
	CloneRepositoryDialog cloneDialog(this);
	cloneDialog.exec();
	if (cloneDialog.result() == QDialog::Accepted) {
		addRepository(cloneDialog.repositoryPath());
	}
}

void OpenRepositoryDialog::on_newButton_clicked()
{
	QString path = QFileDialog::getExistingDirectory(this, i18n("Select a location for the new Git repository"));

	// make sure the user has not cancelled the file dialog
	if (!path.isEmpty()) {
		// make sure the directory exists and is empty
		if (QDir(path).entryList(QDir::NoDotAndDotDot|QDir::Hidden).isEmpty()) {
			Git::Repo::init(path);
			addRepository(path);
		} else {
			KMessageBox::sorry(this, i18n("Can not create a Git repository in %1.\nThe directory is not empty.", path));
		}
	}
}

void OpenRepositoryDialog::on_removeButton_clicked()
{
	QListWidget *repoList = ui->repositoriesListWidget;

	int row = repoList->currentRow();
	repoList->takeItem(row);
	selectRepository(row-1);
}

void OpenRepositoryDialog::on_repositoriesListWidget_currentTextChanged(const QString &text)
{
	ui->removeButton->setEnabled(!text.isEmpty());
	ui->openButton->setEnabled(Git::Repo::containsRepository(text));
}

void OpenRepositoryDialog::reject()
{
	saveRepositories();

	QDialog::reject();
}

void OpenRepositoryDialog::saveRepositories()
{
	QListWidget *repoList = ui->repositoriesListWidget;

	KSharedConfigPtr config = KGlobal::config();
	KConfigGroup cg = config->group("Repositories");

	QStringList repoPaths;
	for(int row=0; row < repoList->count(); ++row) {
		repoPaths.append(repoList->item(row)->text());
	}

	cg.writePathEntry("List", repoPaths);

	cg.writePathEntry("Last", selectedRepositoryPath());

	config->sync();
}

QString OpenRepositoryDialog::selectedRepositoryPath() const
{
	QListWidgetItem *currentItem = ui->repositoriesListWidget->currentItem();

	if (currentItem) {
		return currentItem->text();
	}

	return QString();
}

void OpenRepositoryDialog::selectRepository(const int row)
{
	ui->repositoriesListWidget->setCurrentRow(row);
}

void OpenRepositoryDialog::selectRepository(const QString &repoPath)
{
	QListWidget *repoList = ui->repositoriesListWidget;

	QList<QListWidgetItem*> foundItems = repoList->findItems(repoPath, Qt::MatchExactly);
	if (!foundItems.isEmpty()) {
		repoList->setCurrentItem(foundItems[0]);
		//repoList->setCurrentRow(repoList->row(foundItems[0]));
	}
}

#include "OpenRepositoryDialog.moc"
