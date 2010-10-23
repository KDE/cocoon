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

#include "FileStatusWidget.h"
#include "ui_FileStatusWidget.h"

#include "Git/Repo.h"
#include "Git/Status.h"



FileStatusWidget::FileStatusWidget(QWidget *parent)
		: QWidget(parent)
		, m_byteArray(0)
		, m_repo(0)
		, ui(new Ui::FileStatusWidget)
{
	ui->setupUi(this);

	clear();
}

FileStatusWidget::~FileStatusWidget()
{
	delete ui;
}

QByteArray FileStatusWidget::byteArray()
{
	if (m_byteArray.isNull()) {
		m_byteArray = fileData("blob");
	}

	return m_byteArray;
}

void FileStatusWidget::clear()
{
	QPixmap emptyPixmap;
	ui->stageIconLabel->setPixmap(emptyPixmap);
	ui->stageLabel->clear();
	ui->statusIconLabel->setPixmap(emptyPixmap);
	ui->statusLabel->clear();
	ui->mimeTypeIconLabel->setPixmap(emptyPixmap);
	ui->filePathLabel->clear();
	ui->fileInfoLabel->clear();
	ui->diffWidget->clear();
	m_byteArray.clear();
}

QByteArray FileStatusWidget::fileData(QString type)
{
	if (type.isEmpty()) {
		type = fileType();
	}

	if (type == "blob") {
		return m_file->blob();
	} else {
		return m_file->diff().toUtf8();
	}
}

QString FileStatusWidget::fileType()
{
	if (m_file->isUntracked()) {
		return "blob";
	} else {
		return "diff";
	}
}

bool FileStatusWidget::isBinary()
{
	return KMimeType::isBufferBinaryData(byteArray());
}

KMimeType::Ptr FileStatusWidget::mimeType()
{
	return KMimeType::findByNameAndContent(m_file->path(), byteArray());
}

void FileStatusWidget::setFile(const Git::StatusFile &file)
{
	m_file = &file;
	clear();
	showFileStatus();
}

void FileStatusWidget::setRepository(Git::Repo *repo)
{
	if (m_repo) {
		m_repo->disconnect(this);
	}

	m_repo = repo;

	connect(m_repo, SIGNAL(historyChanged()), this, SLOT(clear()));

	clear();
}

void FileStatusWidget::showFileStatus()
{
	QString stage;
	QIcon stageIcon;
	if (m_file->changesStaged()) {
		stage = "Staged";
		stageIcon = KIcon("git-file-staged");
	} else {
		stage = "Unstaged";
		stageIcon = KIcon("git-file-unstaged");
	}

	QString fileInfo;
	if (!isBinary()) {
		if (m_file->isUntracked()) {
			fileInfo = i18n("%1 lines", m_file->blob().split('\n').size());
		} else {
			int insertions = 0;
			int  deletions = 0;
			foreach (const QString &line, m_file->diff().split("\n")) {
				if (line.startsWith("+") && !line.startsWith("+++")) { // the "+++" thing is ok, because we deal with single files
					++insertions;
				} else if (line.startsWith("-") && !line.startsWith("---")) { // as above
					++deletions;
				}
			}

			fileInfo = i18n("<html><body><span style=\"color: green;\">+%1</span> <span style=\"color: red;\">-%2</span> lines</body></html>", insertions, deletions);
		}
	} else {
		fileInfo = i18n("%1 Bytes", byteArray().size());
	}

	QString diff;
	if (isBinary()) {
		diff = "Binary file (content not shown)";
	} else {
		diff = fileData(fileType());
	}

	ui->stageIconLabel->setPixmap(stageIcon.pixmap(16));
	ui->stageLabel->setText(stage);
	ui->statusIconLabel->setPixmap(KIcon(statusToIconName(m_file->status())).pixmap(16));
	ui->statusLabel->setText(statusToString(m_file->status()));
	ui->mimeTypeIconLabel->setPixmap(KIcon(mimeType()->iconName()).pixmap(32));
	ui->filePathLabel->setText(m_file->path());
	ui->fileInfoLabel->setText(fileInfo);
	ui->diffWidget->setDiff(diff);
}

QString FileStatusWidget::statusToIconName(Git::StatusFile::Status status)
{
	switch (status) {
	case Git::StatusFile::Added:
		return "git-file-added";
	case Git::StatusFile::Deleted:
		return "git-file-deleted";
	case Git::StatusFile::Modified:
		return "git-file-modified";
	case Git::StatusFile::Untracked:
		return "git-file-untracked";
	default:
		return "git-file-normal";
	}
}

QString FileStatusWidget::statusToString(Git::StatusFile::Status status)
{
	switch (status) {
	case Git::StatusFile::Added:
		return i18n("Added");
	case Git::StatusFile::Deleted:
		return i18n("Deleted");
	case Git::StatusFile::Modified:
		return i18n("Modified");
	case Git::StatusFile::Untracked:
		return i18n("Untracked");
	default:
		return i18n("None");
	}
}

#include "FileStatusWidget.moc"
