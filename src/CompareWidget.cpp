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

#include "CompareWidget.h"
#include "ui_CompareWidget.h"

#include <Git/Commit.h>
#include <Git/Ref.h>
#include <Git/Repo.h>

#include "GitBranchesModel.h"
#include "GitHistoryModel.h"



CompareWidget::CompareWidget(QWidget *parent)
	: QWidget(parent)
	, m_commitAId()
	, m_commitBId()
	, m_repo(0)
	, ui(new Ui::CompareWidget)
{
	ui->setupUi(this);
}

CompareWidget::~CompareWidget()
{
	delete ui;
}

void CompareWidget::loadModels()
{
	m_branchesModel = new GitBranchesModel(*m_repo, this);
	m_historyAModel = new GitHistoryModel(*m_repo, this);
	m_historyBModel = new GitHistoryModel(*m_repo, this);

	ui->branchAComboBox->setModel(m_branchesModel);
	ui->branchBComboBox->setModel(m_branchesModel);

	ui->historyAView->setModel(m_historyAModel);
	ui->historyBView->setModel(m_historyBModel);

	QModelIndex currentHistoryIndex = m_historyAModel->index(0, 0);
	ui->historyAView->setCurrentIndex(currentHistoryIndex);
	ui->historyBView->setCurrentIndex(currentHistoryIndex);
	on_historyAView_clicked(currentHistoryIndex);
	on_historyBView_clicked(currentHistoryIndex);

	showCurrentBranch();
}

void CompareWidget::on_branchAComboBox_currentIndexChanged(const QString &branchName)
{
	m_historyAModel->setBranch(m_repo->ref(branchName).name());
}

void CompareWidget::on_branchBComboBox_currentIndexChanged(const QString &branchName)
{
	m_historyBModel->setBranch(m_repo->ref(branchName).name());
}

void CompareWidget::on_historyAView_clicked(const QModelIndex &index)
{
	m_commitAId = m_historyAModel->mapToCommit(index).id();
	updateComparison();
}

void CompareWidget::on_historyBView_clicked(const QModelIndex &index)
{
	m_commitBId = m_historyBModel->mapToCommit(index).id();
	updateComparison();
}

void CompareWidget::setRepository(Git::Repo *repo)
{
	m_repo = repo;
	loadModels();
}

void CompareWidget::showCurrentBranch()
{
	int currentBranchIndex = ui->branchAComboBox->findText(m_repo->currentHead().name());
	ui->branchAComboBox->setCurrentIndex(currentBranchIndex);
	ui->branchBComboBox->setCurrentIndex(currentBranchIndex);
	updateComparison();
}

void CompareWidget::updateComparison()
{
	if (!m_commitAId.isValid()) {
		m_commitAId = m_repo->commits(m_repo->currentHead().name()).first().id();
	}
	if (!m_commitBId.isValid()) {
		m_commitBId = m_repo->commits(m_repo->currentHead().name()).first().id();
	}

	Git::Commit commitA = m_commitAId.object().toCommit();
	Git::Commit commitB = m_commitBId.object().toCommit();

	ui->diffWidget->setDiff(m_repo->diff(commitA, commitB));
}

#include "CompareWidget.moc"
