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

#include "CloneRepositoryDialog.h"
#include "ui_CloneRepositoryDialog.h"

#include "Git/CloneRepositoryProcess.h"
#include "Git/Repo.h"

#include <KApplication>
#include <KMessageBox>

#include <QFileDialog>



CloneRepositoryDialog::CloneRepositoryDialog(QWidget *parent)
	: QDialog(parent)
	, m_finished(false)
	, ui(new Ui::CloneRepositoryDialog)
{
	ui->setupUi(this);

	setWindowIcon(KIcon("repository-clone"));
	ui->cancelButton->setIcon(KIcon("dialog-cancel"));
	ui->cloneButton->setIcon(KIcon("git-clone"));
	ui->cloningFinishedIconLabel->setPixmap(KIcon("finished").pixmap(32));
	ui->cloningProgressIconLabel->setPixmap(KIcon("cloning").pixmap(64));
	ui->finishedButton->setIcon(KIcon("dialog-ok"));

	enableClone();
}

CloneRepositoryDialog::~CloneRepositoryDialog()
{
	delete ui;
}



void CloneRepositoryDialog::startCloning()
{
	QString repoUrl = ui->cloneUrlRequester->url().url();
	QString path = ui->localUrlRequester->url().pathOrUrl(KUrl::RemoveTrailingSlash);
	QDir().rmdir(path);

	Git::CloneRepositoryProcess *thread = new Git::CloneRepositoryProcess(repoUrl, path, this);

	connect(thread, SIGNAL(cloningFinished()), this, SLOT(slotCloningFinished()));

	thread->start();
}

void CloneRepositoryDialog::enableClone()
{
	KUrl cloneUrl = ui->cloneUrlRequester->url();
	KUrl localUrl = ui->localUrlRequester->url();

	QString errorMessage;
	bool enabled;

	if (cloneUrl.isEmpty()) {
		errorMessage = i18n("No clone source.");
		enabled = false;
	} else if (!cloneUrl.isValid()) {
		errorMessage = i18n("Clone source has an invalid URL.");
		enabled = false;
	} else if (localUrl.isEmpty()) {
		errorMessage = i18n("No clone destination.");
		enabled = false;
	} else if (!localUrl.isValid()) {
		errorMessage = i18n("Clone destination has an invalid URL.");
		enabled = false;
	} else if (!QFile(localUrl.pathOrUrl()).exists()) {
		errorMessage = i18n("Clone destination does not exist.");
		enabled = false;
	} else if (!QDir(localUrl.pathOrUrl()).entryList(QDir::NoDotAndDotDot).isEmpty()) {
		errorMessage = i18n("Clone destination is not empty.");
		enabled = false;
	} else {
		errorMessage = QString();
		enabled = true;
	}

	ui->errorLabel->setText(errorMessage);
	ui->cloneButton->setEnabled(enabled);
}

void CloneRepositoryDialog::enableFinish()
{
	ui->cloningFinishedIconLabel->setVisible(m_finished);
	ui->cloningFinishedLabel->setVisible(m_finished);
	ui->finishedButton->setEnabled(m_finished);
}

void CloneRepositoryDialog::on_cloneButton_clicked()
{
	enableFinish();
	ui->stackedWidget->setCurrentIndex(1);
}

void CloneRepositoryDialog::on_cloneUrlRequester_textChanged(const QString &text)
{
	Q_UNUSED(text);

	enableClone();
}

void CloneRepositoryDialog::on_localUrlRequester_textChanged(const QString &text)
{
	Q_UNUSED(text);

	enableClone();
}

void CloneRepositoryDialog::on_stackedWidget_currentChanged(int index)
{
	switch (index) {
	case 0:
		enableClone();
		enableFinish();
		break;
	case 1:
		// do the actual cloning
		startCloning();
	}
}

void CloneRepositoryDialog::slotCloningFinished()
{
	m_finished = true;
	enableFinish();
}

QString CloneRepositoryDialog::repositoryPath() const
{
	return ui->localUrlRequester->text();
}

#include "CloneRepositoryDialog.moc"
