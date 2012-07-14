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

#include "CommitInfoWidget.h"
#include "ui_CommitInfoWidget.h"

#include "CommitHelper.h"
using namespace Cocoon;

#include <QGit2/QGit2>



CommitInfoWidget::CommitInfoWidget(QWidget *parent)
	: QWidget(parent)
	, m_commit()
	, ui(new Ui::CommitInfoWidget)
{
	ui->setupUi(this);
}

CommitInfoWidget::~CommitInfoWidget()
{
	delete ui;
}

void CommitInfoWidget::clear()
{
	ui->idLabel->clear();
	ui->authorLabel->clear();
	ui->messageLabel->clear();
	ui->diffView->clear();
}

void CommitInfoWidget::updateView()
{
	if (m_commit.isNull()) {
		clear();
		return;
	}

	ui->idLabel->setText(m_commit.oid().format());
	ui->authorLabel->setText(i18n("%1<%2> %3", m_commit.author().name(), m_commit.author().email(), m_commit.author().when().toString()));
	ui->messageLabel->setText(m_commit.message());
	ui->diffView->setDiff(CommitHelper::diffParent(m_commit));
}

void CommitInfoWidget::setCommit(const QGitCommit &commit)
{
	m_commit = commit;
	updateView();
}

#include "CommitInfoWidget.moc"
