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

#include "GitHistoryModel.h"



HistoryWidget::HistoryWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::HistoryWidget)
{
	ui->setupUi(this);
}

HistoryWidget::~HistoryWidget()
{
	delete ui;
}

void HistoryWidget::loadModels()
{
	m_historyModel = new GitHistoryModel(*m_repo, this);

	ui->historyView->setModel(m_historyModel);
}

void HistoryWidget::setRepository(const Git::Repo *repo)
{
	m_repo = repo;
	loadModels();
}

#include "HistoryWidget.moc"