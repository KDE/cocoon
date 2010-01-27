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
#include <KIO/DeleteJob>
#include <KMessageBox>

#include <QFileDialog>



CloneRepositoryDialog::CloneRepositoryDialog(QWidget *parent)
	: QDialog(parent)
	, m_finished(false)
	, m_localDirFilters(QDir::AllEntries | QDir::Hidden | QDir::NoDotAndDotDot)
	, ui(new Ui::CloneRepositoryDialog)
{
	ui->setupUi(this);

	ui->cloneUrlRequester->setMode(KFile::Directory | KFile::ExistingOnly);
	ui->localUrlRequester->setMode(KFile::Directory | KFile::ExistingOnly | KFile::LocalOnly);

	setWindowIcon(KIcon("git-clone"));
	ui->cancelButton->setIcon(KIcon("dialog-cancel"));
	ui->cloneButton->setIcon(KIcon("git-clone"));
	ui->cloningFinishedIconLabel->setPixmap(KIcon("task-waiting").pixmap(32));
	ui->cloningIconLabel->setPixmap(KIcon("git-clone").pixmap(128));
	ui->compressingObjectsIconLabel->setPixmap(KIcon("task-waiting").pixmap(32));
	ui->countingObjectsIconLabel->setPixmap(KIcon("task-waiting").pixmap(32));
	ui->finishedButton->setIcon(KIcon("dialog-ok"));
	ui->receivingObjectsIconLabel->setPixmap(KIcon("task-waiting").pixmap(32));
	ui->resolvingDeltasIconLabel->setPixmap(KIcon("task-waiting").pixmap(32));

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

	KIO::DeleteJob *deleteJob = KIO::del(path);
	deleteJob->exec(); /** @todo error handling */

	Git::CloneRepositoryProcess *thread = new Git::CloneRepositoryProcess(repoUrl, path, this);

	connect(thread, SIGNAL(cloningProgress(int)), this, SLOT(slotCloningProgress(int)));
	connect(thread, SIGNAL(cloningProgress(QString)), this, SLOT(slotCloningProgress(QString)));
	connect(thread, SIGNAL(cloningFinished()), this, SLOT(slotCloningFinished()));

	thread->start();
}

void CloneRepositoryDialog::enableClone()
{
	KUrl cloneUrl = ui->cloneUrlRequester->url();
	KUrl localUrl = ui->localUrlRequester->url();

	QString errorMessage;
	bool enabled = true;

	QDir localDir = QDir(localUrl.pathOrUrl());

	/** @todo Check for network connection */
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
	} else if (!QDir(localUrl.pathOrUrl()).entryList(m_localDirFilters).isEmpty()) {
		if (!ui->cleanLocalDirCheckBox->isChecked()) {
			errorMessage = i18n("Clone destination is not empty.");
			enabled = false;
		}
	}

	ui->errorLabel->setText(errorMessage);
	ui->cloneButton->setEnabled(enabled);
}

void CloneRepositoryDialog::enableFinish()
{
	if (m_finished) {
		ui->cloningFinishedIconLabel->setPixmap(KIcon("task-complete").pixmap(64));
		ui->compressingObjectsIconLabel->setPixmap(KIcon("task-complete").pixmap(32));
		ui->countingObjectsIconLabel->setPixmap(KIcon("task-complete").pixmap(32));
		ui->receivingObjectsIconLabel->setPixmap(KIcon("task-complete").pixmap(32));
		ui->resolvingDeltasIconLabel->setPixmap(KIcon("task-complete").pixmap(32));
	}
	ui->finishedButton->setEnabled(m_finished);
}

void CloneRepositoryDialog::on_cleanLocalDirCheckBox_toggled(bool checked)
{
	Q_UNUSED(checked);

	enableClone();
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

	KUrl localUrl = ui->localUrlRequester->url();

	ui->cleanLocalDirCheckBox->setEnabled(localUrl.isValid() && !QDir(localUrl.pathOrUrl()).entryList(m_localDirFilters).isEmpty());

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

void CloneRepositoryDialog::slotCloningProgress(QString progress)
{
	if (progress.startsWith("remote: Counting objects")) {
		ui->countingObjectsIconLabel->setPixmap(KIcon("task-in-progress").pixmap(32));
		ui->countingObjectsLabel->setText(progress.mid(8)); // get rid of the "remote: " part
	} else if (progress.startsWith("remote: Compressing objects")) {
		ui->countingObjectsIconLabel->setPixmap(KIcon("task-complete").pixmap(32));
		ui->compressingObjectsIconLabel->setPixmap(KIcon("task-in-progress").pixmap(32));
		ui->compressingObjectsLabel->setText(progress.mid(8)); // get rid of the "remote: " part
	} else if (progress.startsWith("Receiving objects")) {
		ui->compressingObjectsIconLabel->setPixmap(KIcon("task-complete").pixmap(32));
		ui->receivingObjectsIconLabel->setPixmap(KIcon("task-in-progress").pixmap(32));
		ui->receivingObjectsLabel->setText(progress);
	} else if (progress.startsWith("Resolving deltas")) {
		ui->receivingObjectsIconLabel->setPixmap(KIcon("task-complete").pixmap(32));
		ui->resolvingDeltasIconLabel->setPixmap(KIcon("task-in-progress").pixmap(32));
		ui->resolvingDeltasLabel->setText(progress);
	}
}

QString CloneRepositoryDialog::repositoryPath() const
{
	return ui->localUrlRequester->text();
}

#include "CloneRepositoryDialog.moc"
