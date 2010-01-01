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

#include "CommitInfoWidget.h"
#include "ui_CommitInfoWidget.h"

#include "Git/Commit.h"



CommitInfoWidget::CommitInfoWidget(QWidget *parent)
	: QWidget(parent)
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
	ui->shaLabel->clear();
	ui->authorLabel->clear();
	ui->messageLabel->clear();
	//ui->diffWidget->setDiff(0);
}

void CommitInfoWidget::updateView()
{
	if (!m_commit) {
		clear();
		return;
	}

	ui->shaLabel->setText(m_commit->id());
	ui->authorLabel->setText(i18n("%1 %2", m_commit->author(), m_commit->authoredAt().toString()));
	ui->messageLabel->setText(m_commit->message());
/*
			@ui.diffWidget.diff = unless commit.diffs.empty?
															commit.diffs.map(&:diff).join("\n")
														else
															''
														end
*/
}

void CommitInfoWidget::setCommit(const Git::Commit *commit)
{
	m_commit = commit;
	updateView();
}

#include "CommitInfoWidget.moc"
