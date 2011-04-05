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

#include "HistoryWidget.h"
#include "ui_HistoryWidget.h"

#include "Git/Ref.h"
#include "Git/Repo.h"

#include "GitBranchesModel.h"
#include "GitHistoryModel.h"



HistoryWidget::HistoryWidget(QWidget *parent)
	: QWidget(parent)
	, m_branchesModel(0)
	, m_historyModel(0)
	, m_repo(0)
	, ui(new Ui::HistoryWidget)
{
	ui->setupUi(this);
}

HistoryWidget::~HistoryWidget()
{
	delete ui;
}



void HistoryWidget::clear()
{
	delete m_branchesModel;
	m_branchesModel = 0;

	delete m_historyModel;
	m_historyModel = 0;

	ui->branchComboBox->clear();
	ui->historyView->setModel(0);
	ui->commitWidget->clear();
}

void HistoryWidget::loadModels()
{
	m_branchesModel = new GitBranchesModel(*m_repo, this);
	m_historyModel = new GitHistoryModel(*m_repo, this);

	ui->branchComboBox->setModel(m_branchesModel);
	ui->historyView->setModel(m_historyModel);

	QModelIndex currentHistoryIndex = m_historyModel->index(0, 0);
	ui->historyView->setCurrentIndex(currentHistoryIndex);
	on_historyView_clicked(currentHistoryIndex);

	showCurrentBranch();
}

void HistoryWidget::on_branchComboBox_currentIndexChanged(const QString &branchName)
{
	m_historyModel->setBranch(branchName);
}

void HistoryWidget::on_historyView_clicked(const QModelIndex &index)
{
	ui->commitWidget->setCommit(m_historyModel->mapToCommit(index));
}

void HistoryWidget::setRepository(Git::Repo *repo)
{
	clear();
	m_repo = repo;
	loadModels();
}

void HistoryWidget::showCurrentBranch()
{
	int currentBranchIndex = ui->branchComboBox->findText(m_repo->currentHead().name());
	ui->branchComboBox->setCurrentIndex(currentBranchIndex);
}

#include "HistoryWidget.moc"
