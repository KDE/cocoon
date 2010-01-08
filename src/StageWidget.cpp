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

#include "GitStagedFilesModel.h"



StageWidget::StageWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::StageWidget)
{
	ui->setupUi(this);
}

StageWidget::~StageWidget()
{
	delete ui;
}

void StageWidget::loadModels()
{
	m_stagedFilesModel = new GitStagedFilesModel(*m_repo, this);
	ui->stagedChangesView->setModel(m_stagedFilesModel);
}

void StageWidget::setRepository(const Git::Repo *repo)
{
	m_repo = repo;
	loadModels();
}

#include "StageWidget.moc"
