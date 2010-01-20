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

#include "StageWidget.h"
#include "ui_StageWidget.h"

#include "CommitWidget.h"
#include "FileStatusWidget.h"

#include "Git/Repo.h"
#include "Git/Status.h"

#include "GitFileStatusModel.h"

#include <QSortFilterProxyModel>
#include <QModelIndex>



StageWidget::StageWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::StageWidget)
{
	ui->setupUi(this);

	m_stagedFilesProxyModel = new QSortFilterProxyModel(this);
	m_unstagedFilesProxyModel = new QSortFilterProxyModel(this);

	ui->stagedChangesView->setModel(m_stagedFilesProxyModel);
	ui->unstagedChangesView->setModel(m_unstagedFilesProxyModel);
}

StageWidget::~StageWidget()
{
	delete ui;
}

void StageWidget::commit()
{
	ui->commitWidget->commit();
}

void StageWidget::loadModels()
{
	m_stagedFilesModel = new GitFileStatusModel(Git::Staged, m_repo, this);
	m_unstagedFilesModel = new GitFileStatusModel(Git::Unstaged, m_repo, this);

	m_stagedFilesProxyModel->setSourceModel(m_stagedFilesModel);
	m_unstagedFilesProxyModel->setSourceModel(m_unstagedFilesModel);

	ui->stagedChangesView->header()->setSortIndicator(0, Qt::AscendingOrder);
	ui->unstagedChangesView->header()->setSortIndicator(0, Qt::AscendingOrder);

	ui->fileStatusWidget->setRepository(m_repo);
	ui->commitWidget->setRepository(m_repo);
}

void StageWidget::on_stagedChangesView_clicked(const QModelIndex &index)
{
	ui->unstagedChangesView->clearSelection();
	const Git::StatusFile *file = m_stagedFilesModel->mapToStatusFile(m_stagedFilesProxyModel->mapToSource(index));
	ui->fileStatusWidget->setFile(*file);
}

void StageWidget::on_stagedChangesView_doubleClicked(const QModelIndex &index)
{
	Q_UNUSED(index);

	unstageFile();
}

void StageWidget::on_unstagedChangesView_clicked(const QModelIndex &index)
{
	ui->stagedChangesView->clearSelection();
	const Git::StatusFile *file = m_unstagedFilesModel->mapToStatusFile(m_unstagedFilesProxyModel->mapToSource(index));
	ui->fileStatusWidget->setFile(*file);
}

void StageWidget::on_unstagedChangesView_doubleClicked(const QModelIndex &index)
{
	Q_UNUSED(index);

	stageFile();
}

void StageWidget::reload()
{
	m_stagedFilesModel->reset();
	m_unstagedFilesModel->reset();
	ui->commitWidget->clear();
	ui->fileStatusWidget->clear();
}

void StageWidget::setRepository(Git::Repo *repo)
{
	m_repo = repo;
	loadModels();
}

void StageWidget::stageFile()
{
	QModelIndexList indexes = ui->unstagedChangesView->selectionModel()->selectedIndexes();
	if (!indexes.isEmpty()) {
		QModelIndex index = indexes.first();
		const Git::StatusFile *statusFile = m_unstagedFilesModel->mapToStatusFile(m_unstagedFilesProxyModel->mapToSource(index));
		m_repo->stageFiles(QStringList() << statusFile->path());

		// set selection on staged file
		QModelIndex newIndex = m_stagedFilesProxyModel->mapFromSource(m_stagedFilesModel->mapToIndex(*statusFile));
		ui->stagedChangesView->setCurrentIndex(newIndex);

		// update file status view
		const Git::StatusFile *newStatusFile = m_stagedFilesModel->mapToStatusFile(m_stagedFilesProxyModel->mapToSource(newIndex));
		ui->fileStatusWidget->setFile(*newStatusFile);
	}
}

void StageWidget::unstageFile()
{
	QModelIndexList indexes = ui->stagedChangesView->selectionModel()->selectedIndexes();
	if (!indexes.isEmpty()) {
		QModelIndex index = indexes.first();
		const Git::StatusFile *statusFile = m_stagedFilesModel->mapToStatusFile(m_stagedFilesProxyModel->mapToSource(index));
		m_repo->unstageFiles(QStringList() << statusFile->path());

		// set selection on unstaged file
		QModelIndex newIndex = m_unstagedFilesProxyModel->mapFromSource(m_unstagedFilesModel->mapToIndex(*statusFile));
		ui->unstagedChangesView->setCurrentIndex(newIndex);

		// update file status view
		const Git::StatusFile *newStatusFile = m_unstagedFilesModel->mapToStatusFile(m_unstagedFilesProxyModel->mapToSource(newIndex));
		ui->fileStatusWidget->setFile(*newStatusFile);
	}
}

#include "StageWidget.moc"
