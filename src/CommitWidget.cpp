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

#include "CommitWidget.h"
#include "ui_CommitWidget.h"

#include <Git/Repo.h>
#include <Git/Status.h>

#include <KMessageBox>



CommitWidget::CommitWidget(QWidget *parent)
	: QWidget(parent)
	, m_repo(0)
	, m_status(0)
	, ui(new Ui::CommitWidget)
{
	ui->setupUi(this);

	connect(ui->commitMessageTextEdit, SIGNAL(textChanged()), this, SLOT(enableCommit()));
}

CommitWidget::~CommitWidget()
{
	delete ui;
}

void CommitWidget::clear()
{
	ui->commitMessageTextEdit->clear();
	m_status = 0;
}

void CommitWidget::commit()
{
	if (error()) {
		KMessageBox::sorry(this, i18n("You can't commit yet, because you have %1.", m_errorMessage));
		return;
	}

	QString message = ui->commitMessageTextEdit->toPlainText();
	m_repo->commitIndex(message);

	clear();
	reload();
}

void CommitWidget::enableCommit()
{
	ui->commitErrorLabel->setText(error() ? i18n("You have %1.", m_errorMessage) : QString());
	ui->commitButton->setEnabled(!error());
}

bool CommitWidget::error()
{
	if (repoStatus()->stagedFiles().isEmpty()) {
		m_errorMessage = i18n("no files staged");
		return true;
	} else if (ui->commitMessageTextEdit->toPlainText().isEmpty()) {
		m_errorMessage = i18n("no commit message");
		return true;
	}

	m_errorMessage = QString();
	return false;
}

void CommitWidget::on_commitButton_clicked()
{
	commit();
}

void CommitWidget::on_Repo_indexChanged()
{
	m_status = 0;
	enableCommit();
}

void CommitWidget::reload()
{
	enableCommit();
}

Git::Status* CommitWidget::repoStatus()
{
	if (!m_status) {
		m_status = m_repo->status();
	}

	return m_status;
}

void CommitWidget::setRepository(Git::Repo *repo)
{
	if (m_repo) {
		m_repo->disconnect(this);
	}

	m_repo = repo;

	connect(m_repo, SIGNAL(indexChanged()), this, SLOT(on_Repo_indexChanged()));

	clear();
	reload();
}

#include "CommitWidget.moc"
