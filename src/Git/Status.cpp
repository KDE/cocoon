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

#include "Status.h"

#include "gitrunner.h"
#include "Repo.h"

using namespace Git;



Status::Status(const Repo *repo)
	: QObject((QObject*)repo)
	, m_repo(repo)
{
	constuctStatus();
}

void Status::addFile(StatusFile *file)
{
	QString path = file->path();
	if (m_status[path].size() > 0) {
		StatusFile *lastFile = m_status[path].last();
		if (lastFile->m_status.isEmpty() || !file->isStaged()) {
			lastFile->merge(*file);
		} else {
			m_status[path] << new StatusFile(m_repo);
		}
	} else {
		m_status[path] << new StatusFile(m_repo);
	}

	StatusFile *statusFile  = m_status[path].last();
	statusFile->m_idIndex   = file->m_idIndex;
	statusFile->m_idRepo    = file->m_idRepo;
	statusFile->m_modeIndex = file->m_modeIndex;
	statusFile->m_modeRepo  = file->m_modeRepo;
	statusFile->m_path      = file->m_path;
	statusFile->m_staged    = file->m_staged;
	statusFile->m_status    = file->m_status;
}

void Status::constuctStatus()
{
	m_status.clear();

	foreach (StatusFile *file, lsFiles()) {
		addFile(file);
	}

	// find untracked files
	foreach (StatusFile *file, untrackedFiles()) {
		addFile(file);
	}

	// find modified in tree
	foreach (StatusFile *file, diffFiles()) {
		addFile(file);
	}

	// find added but not committed - new files
	foreach (StatusFile *file, diffIndex("HEAD")) {
		// the file has been staged
		// if the file has a index id or is marked as deleted
		// @info staged deleted files have no index id
		if ((!file->idIndex().isEmpty() || file->status() == "D")
			|| m_status[file->path()].last()->idRepo() != file->idRepo()) {
			file->m_staged = true;
		}

		if (!(m_status.contains(file->path()) && m_status[file->path()].last()->status() == "D")) {
			addFile(file);
		}
	}

	foreach (const QList<StatusFile*> list, m_status.values()) {
		m_files << list;
	}
}

QList<StatusFile*> Status::diffFiles() const
{
	QList<StatusFile*> result;

	GitRunner runner;
	runner.setDirectory(m_repo->workingDir());

	runner.diffFiles();

	QStringList lines = runner.getResult().split("\n");
	lines.removeLast();
	foreach(const QString &line, lines) {
		QString info = line.split("\t")[0];
		QString rawFile = line.split("\t")[1];
		QString fileSrc = unescapeFileName(rawFile);
		QString modeSrc = info.split(" ")[0].mid(1);
		QString modeDst = info.split(" ")[1];
		QString   idSrc = info.split(" ")[2];
		QString   idDst = info.split(" ")[3];
		QString  status = info.split(" ")[4];

		StatusFile *fileStatus = new StatusFile(m_repo);
		fileStatus->m_path = fileSrc;
		fileStatus->m_idIndex = QRegExp("^[0]*$").exactMatch(idDst) ? QString() : idDst;
		fileStatus->m_idRepo = QRegExp("^[0]*$").exactMatch(idSrc) ? QString() : idSrc;
		fileStatus->m_modeIndex = modeDst == "000000" ? QString() : modeDst;
		fileStatus->m_modeRepo = modeSrc == "000000" ? QString() : modeSrc;
		fileStatus->m_status = status;

		result << fileStatus;
	}

	return result;
}

QList<StatusFile*> Status::diffIndex(const QString &treeish) const
{
	QList<StatusFile*> result;

	GitRunner runner;
	runner.setDirectory(m_repo->workingDir());

	runner.diffIndex(treeish);

	QStringList lines = runner.getResult().split("\n");
	lines.removeLast();

	foreach(const QString &line, lines) {
		QString info = line.split("\t")[0];
		QString rawFile = line.split("\t")[1];
		QString fileSrc = unescapeFileName(rawFile);
		QString modeSrc = info.split(" ")[0].mid(1);
		QString modeDst = info.split(" ")[1];
		QString   idSrc = info.split(" ")[2];
		QString   idDst = info.split(" ")[3];
		QString  status = info.split(" ")[4];

		StatusFile *fileStatus = new StatusFile(m_repo);
		fileStatus->m_path = fileSrc;
		fileStatus->m_idIndex = QRegExp("^[0]*$").exactMatch(idDst) ? QString() : idDst;
		fileStatus->m_idRepo = QRegExp("^[0]*$").exactMatch(idSrc) ? QString() : idSrc;
		fileStatus->m_modeIndex = modeDst == "000000" ? QString() : modeDst;
		fileStatus->m_modeRepo = modeSrc == "000000" ? QString() : modeSrc;
		fileStatus->m_status = status;

		if (!fileStatus->m_idIndex.isNull() || fileStatus->m_status == "D") {
			fileStatus->m_staged = true;
		}

		result << fileStatus;
	}

	return result;
}

QList<StatusFile*> Status::files() const
{
	return m_files;
}

QList<StatusFile*> Status::forFile(const QString &file) const
{
	return m_status[file];
}

QList<StatusFile*> Status::ignoredFiles() const
{
	QList<StatusFile*> ignoredFiles;

	GitRunner runner;
	runner.setDirectory(m_repo->workingDir());

	// list ignored files
	QStringList otherIgnoredFiles;
	runner.lsFiles(QStringList() << "--others" << "--ignored" << "--exclude-standard");
	otherIgnoredFiles = runner.getResult().split("\n");
	otherIgnoredFiles.removeLast(); // remove empty line at the end
	foreach (const QString &file, otherIgnoredFiles) { // unescape file names
		StatusFile *statusFile = new StatusFile(m_repo);

		statusFile->m_path = unescapeFileName(file);

		ignoredFiles << statusFile;
	}

	// list ignored directories (may also include files)
	runner.lsFiles(QStringList() << "--others" << "--ignored" << "--exclude-standard" << "--directory");
	QStringList otherIgnoredDirs = runner.getResult().split("\n");
	otherIgnoredDirs.removeLast(); // remove empty line at the end
	for(int i=0; i < otherIgnoredDirs.size(); ++i) { // unescape file names
		otherIgnoredDirs[i] = unescapeFileName(otherIgnoredDirs[i]);
		if (QFileInfo(m_repo->workingDir(), otherIgnoredDirs[i]).isDir()) { // only leave dirs
			StatusFile *statusFile = new StatusFile(m_repo);

			statusFile->m_path = otherIgnoredDirs[i];

			ignoredFiles << statusFile;
		}
	}

//	// add all files in ignoredDirs
//	foreach (const QString &dir, ignoredDirs) {
//		foreach (const QString &file, glob(dir)) {
//			if (!ignoredFiles.contains(file)) {
//				ignoredFiles << file;
//			}
//		}
//	}

	return ignoredFiles;
}

QList<StatusFile*> Status::lsFiles() const
{
	QList<StatusFile*> result;

	GitRunner runner;
	runner.setDirectory(m_repo->workingDir());

	QStringList opts;
	opts << "--stage";

	runner.lsFiles(opts);

	QStringList lines = runner.getResult().split("\n");
	lines.removeLast();
	foreach(const QString &line, lines) {
		QString info = line.split("\t")[0];
		QString rawFile = line.split("\t")[1];
		QString file = unescapeFileName(rawFile);
		QString modeIndex = info.split(" ")[0];
		QString   idIndex = info.split(" ")[1];
		QString     stage = info.split(" ")[2];

		StatusFile *fileStatus = new StatusFile(m_repo);
		fileStatus->m_path = file;
		fileStatus->m_idIndex = QRegExp("^[0]*$").exactMatch(idIndex) ? QString() : idIndex;
		fileStatus->m_modeIndex = modeIndex == "000000" ? QString() : modeIndex;
//		fileStatus->m_stage = stage;

		result << fileStatus;
	}

	return result;
}

QList<StatusFile*> Status::stagedFiles() const
{
	QList<StatusFile*> files;

	foreach(StatusFile *file, m_files) {
		if (file->changesStaged()) {
			files << file;
		}
	}

	return files;
}

QString Status::unescapeFileName(const QString &escapedName) const
{
	return escapedName;
}

QList<StatusFile*> Status::unstagedFiles() const
{
	QList<StatusFile*> files;

	foreach(StatusFile *file, m_files) {
		if (file->changesUnstaged()) {
			files << file;
		}
	}

	return files;
}

QList<StatusFile*> Status::untrackedFiles() const
{
	GitRunner runner;
	runner.setDirectory(m_repo->workingDir());

	// if a file shows up here it has not yet been staged
	// @info staged deleted files don't show up in diff-files
	runner.lsFiles(QStringList() << "--others");

	QStringList otherFiles = runner.getResult().split("\n");
	otherFiles.removeLast(); // remove empty line at the end

	QList<StatusFile*> untrackedFiles;

	foreach (const QString &file, otherFiles) {
		StatusFile *statusFile = new StatusFile(m_repo);
		statusFile->m_path = unescapeFileName(file); // unescape file names
		statusFile->m_staged = false;
		statusFile->m_status = "U";

		untrackedFiles << statusFile;
	}

	// remove ignored files from the list
	foreach (const StatusFile *iFile, ignoredFiles()) {
		foreach (StatusFile *uFile, untrackedFiles) {
			if (uFile->path() == iFile->path() || uFile->path().startsWith(iFile->path())) {
				untrackedFiles.removeOne(uFile);
				delete uFile;
			}
		}
	}

	return untrackedFiles;
}



StatusFile::StatusFile(const Repo *repo)
	: QObject((QObject*)repo)
	, m_repo(repo)
	, m_staged(false)
{
}

const QByteArray StatusFile::blob(QString type) const
{
	if (type.isEmpty()) {
		type = changesUnstaged() ? "file" : "index";
	}

	GitRunner runner;
	runner.setDirectory(m_repo->workingDir());

	QString id;
	QByteArray blobData;

	if (type == "file") {
		QFile file(QDir(m_repo->workingDir()).filePath(path()));
		file.open(QFile::ReadOnly);
		blobData = file.readAll();
		file.close();
	} else if (type == "index") {
		if (!m_idIndex.isEmpty()) {
			id = m_idIndex;
		} else if (!m_idRepo.isEmpty()) {
			id = m_idRepo;
		}
	} else if (type == "repo") {
		if (!m_idRepo.isEmpty()) {
			id = m_idRepo;
		}
	}

	if (!id.isNull()) {
		runner.catFile(id, QStringList() << "-p");
		blobData = runner.getResult().toUtf8();
	}

	return blobData;
}

bool StatusFile::changesStaged() const
{
	return hasChanged() && isStaged();
}

bool StatusFile::changesUnstaged() const
{
	return hasChanged() && !isStaged();
}


const QString StatusFile::diff() const
{
	QString diff;

	if (!isUntracked()) {
		GitRunner runner;
		runner.setDirectory(m_repo->workingDir());

		QStringList opts;
		if (changesStaged()) {
			opts << "--cached";
		}

		runner.diff(QStringList(), opts, QStringList() << path());

		diff = runner.getResult();
	}

	return diff;
}

bool StatusFile::hasChanged() const
{
	return !m_status.isEmpty();
}

const QString& StatusFile::idIndex() const
{
	return m_idIndex;
}

const QString& StatusFile::idRepo() const
{
	return m_idRepo;
}

bool StatusFile::isAdded() const
{
	return m_status == "A";
}

bool StatusFile::isDeleted() const
{
	return m_status == "D";
}

bool StatusFile::isModified() const
{
	return m_status == "M";
}

bool StatusFile::isStaged() const
{
	return m_staged;
}

bool StatusFile::isUntracked() const
{
	return m_status == "U";
}

void StatusFile::merge(const StatusFile &file)
{
	if (!file.m_idIndex.isEmpty())   { m_idIndex   = file.m_idIndex; }
	if (!file.m_idRepo.isEmpty())    { m_idRepo    = file.m_idRepo; }
	if (!file.m_modeIndex.isEmpty()) { m_modeIndex = file.m_modeIndex; }
	if (!file.m_modeRepo.isEmpty())  { m_modeRepo  = file.m_modeRepo; }
	if (!file.m_path.isEmpty())      { m_path      = file.m_path; }
	if ( file.m_staged)              { m_staged    = file.m_staged; }
	else                             { m_staged    = file.m_staged; }
	if (!file.m_status.isEmpty())    { m_status    = file.m_status; }
}

const QString& StatusFile::modeIndex() const
{
	return m_modeIndex;
}

const QString& StatusFile::modeRepo() const
{
	return m_modeRepo;
}

const QString& StatusFile::path() const
{
	return m_path;
}

const QString& StatusFile::status() const
{
	return m_status;
}

#include "Status.moc"
