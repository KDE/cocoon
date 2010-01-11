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

void Status::constuctStatus()
{
	QHash<QString, QHash<QString, QString> > filesData;
	m_status.clear();

	filesData = lsFiles();
	for(int i=0; i < filesData.size(); ++i) {
		QString key = filesData.keys()[i];

		addFile(key, filesData[key]);
	}

	// find untracked files
	foreach(const QString &file, untrackedFiles()) {
		QHash<QString, QString> data;
		data["path"] = file;
		data["staged"] = "false";
		data["status"] = "U";

		addFile(file, data);
	}

	// find modified in tree
	filesData = diffFiles();
	foreach (const QString &file, filesData.keys()) {
		// if a file shows up here it has not yet been staged
		// @info staged deleted files don't show up in diff-files
		filesData[file]["staged"] = "false";

		addFile(file, filesData[file]);
	}

	// find added but not committed - new files
	filesData = diffIndex("HEAD");
	foreach (const QString &file, filesData.keys()) {
		// the file has been staged
		// if the file has a index id or is marked as deleted
		// @info staged deleted files have no index id
		if (!QRegExp("^[0]*$").exactMatch(filesData[file]["idIndex"]) || filesData[file]["status"] == "D"
			|| m_status[file].last()->m_idRepo != filesData[file]["idRepo"]) {
			filesData[file]["staged"] = "true";
		}

		if (!(m_status.contains(file) && m_status[file].last()->m_status == "D")) {
			addFile(file, filesData[file]);
		}
	}

	foreach (const QList<StatusFile*> list, m_status.values()) {
		m_files << list;
	}
}

void Status::addFile(const QString &file, QHash<QString, QString> data)
{
	if (m_status.contains(file) && m_status[file].size() > 0) {
		StatusFile *lastFile = m_status[file].last();
		if (lastFile->m_status.isEmpty() || data["staged"].isEmpty()) {
			if (!lastFile->m_idIndex.isEmpty())   { data["idIndex"]   = lastFile->m_idIndex; }
			if (!lastFile->m_idRepo.isEmpty())    { data["idRepo"]    = lastFile->m_idRepo; }
			if (!lastFile->m_modeIndex.isEmpty()) { data["modeIndex"] = lastFile->m_modeIndex; }
			if (!lastFile->m_modeRepo.isEmpty())  { data["modeRepo"]  = lastFile->m_modeRepo; }
			if (!lastFile->m_path.isEmpty())      { data["path"]      = lastFile->m_path; }
			if ( lastFile->m_staged)              { data["staged"]    = "true"; }
			else                                  { data["staged"]    = "false"; }
			if (!lastFile->m_status.isEmpty())    { data["status"]    = lastFile->m_status; }

			m_status[file][m_status[file].size()-1] = new StatusFile(m_repo);
		} else {
			m_status[file] << new StatusFile(m_repo);
		}
	} else {
		m_status[file] << new StatusFile(m_repo);
	}

	StatusFile *statusFile = m_status[file].last();
	statusFile->m_idIndex   = data["idIndex"];
	statusFile->m_idRepo    = data["idRepo"];
	statusFile->m_modeIndex = data["modeIndex"];
	statusFile->m_modeRepo  = data["modeRepo"];
	statusFile->m_path      = data["path"];
	statusFile->m_staged    = data["staged"] == "true";
	statusFile->m_status    = data["status"];
}

QHash<QString, QHash<QString, QString> > Status::diffFiles() const
{
	QHash<QString, QHash<QString, QString> > result;

	GitRunner runner;
	runner.setDirectory(m_repo->workingDir());

	runner.diffFiles();

	QStringList lines = runner.getResult().split("\n");
	lines.removeLast();
	foreach(const QString &line, lines) {
		QString info = line.split("\t")[0];
		QString rawFile = line.split("\t")[1];
		QString file = unescapeFileName(rawFile);
		QString modeRepo  = info.split(" ")[0].mid(1);
		QString modeIndex = info.split(" ")[1];
		QString   idRepo  = info.split(" ")[2];
		QString   idIndex = info.split(" ")[3];
		QString    status = info.split(" ")[4];

		result[file];
		result[file]["path"] = file;
		result[file]["idIndex"] = idIndex;
		result[file]["idRepo"] = idRepo;
		result[file]["modeIndex"] = modeIndex;
		result[file]["modeRepo"] = modeRepo;
		result[file]["status"] = status;
	}

	return result;
}

QHash<QString, QHash<QString, QString> > Status::diffIndex(const QString &treeish) const
{
	QHash<QString, QHash<QString, QString> > result;

	GitRunner runner;
	runner.setDirectory(m_repo->workingDir());

	runner.diffIndex(treeish);

	QStringList lines = runner.getResult().split("\n");
	lines.removeLast();

	foreach(const QString &line, lines) {
		QString info = line.split("\t")[0];
		QString rawFile = line.split("\t")[1];
		QString file = unescapeFileName(rawFile);
		QString modeRepo  = info.split(" ")[0].mid(1);
		QString modeIndex = info.split(" ")[1];
		QString   idRepo  = info.split(" ")[2];
		QString   idIndex = info.split(" ")[3];
		QString    status = info.split(" ")[4];

		result[file];
		result[file]["path"] = file;
		result[file]["idIndex"] = idIndex;
		result[file]["idRepo"] = idRepo;
		result[file]["modeIndex"] = modeIndex;
		result[file]["modeRepo"] = modeRepo;
		result[file]["status"] = status;
	}

	return result;
}

QStringList Status::ignoredFiles() const
{
	QStringList ignoredFiles;

	GitRunner runner;
	runner.setDirectory(m_repo->workingDir());

	// list ignored files
	runner.lsFiles(QStringList() << "--others" << "--ignored" << "--exclude-standard");
	ignoredFiles = runner.getResult().split("\n");
	ignoredFiles.removeLast(); // remove empty line at the end
	for(int i=0; i < ignoredFiles.size(); ++i) { // unescape file names
		ignoredFiles[i] = unescapeFileName(ignoredFiles[i]);
	}

	// list ignored directories (may also include files)
	runner.lsFiles(QStringList() << "--others" << "--ignored" << "--exclude-standard" << "--directory");
	QStringList ignoredDirs = runner.getResult().split("\n");
	ignoredDirs.removeLast(); // remove empty line at the end
	for(int i=0; i < ignoredDirs.size(); ++i) { // unescape file names
		ignoredDirs[i] = unescapeFileName(ignoredDirs[i]);
		if (!QFileInfo(m_repo->workingDir(), ignoredDirs[i]).isDir()) { // only leave dirs
			ignoredDirs.removeAt(i);
			--i;
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
	ignoredFiles << ignoredDirs;

	return ignoredFiles;
}

QHash<QString, QHash<QString, QString> > Status::lsFiles() const
{
	QHash<QString, QHash<QString, QString> > result;

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

		result[file];
		result[file]["path"] = file;
		result[file]["modeIndex"] = modeIndex;
		result[file]["idIndex"] = idIndex;
		result[file]["stage"] = stage;
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
		if (!file->changesUnstaged()) {
			files << file;
		}
	}

	return files;
}

QStringList Status::untrackedFiles() const
{
	GitRunner runner;
	runner.setDirectory(m_repo->workingDir());

	runner.lsFiles(QStringList() << "--others");

	QStringList otherFiles = runner.getResult().split("\n");
	otherFiles.removeLast(); // remove empty line at the end

	// unescape file names
	for (int i=0; i < otherFiles.size(); ++i) {
		otherFiles[i] = unescapeFileName(otherFiles[i]);
	}

	// remove ignored files from the list
	foreach (const QString &iFile, ignoredFiles()) {
		foreach (const QString &oFile, otherFiles) {
			if (oFile == iFile || oFile.startsWith(iFile)) {
				otherFiles.removeOne(oFile);
			}
		}
	}

	return otherFiles;
}



StatusFile::StatusFile(const Repo *repo)
	: QObject((QObject*)repo)
	, m_repo(repo)
{
}

const QByteArray StatusFile::blob(QString type) const
{
	if (type.isEmpty()) {
		if (changesUnstaged()) {
			type = "file";
		} else {
			type = "index";
		}
	}

	GitRunner runner;
	runner.setDirectory(m_repo->workingDir());

	QByteArray blobData;

	if (type == "file") {
		QFile file(QDir(m_repo->workingDir()).filePath(path()));
		file.open(QFile::ReadOnly);
		blobData = file.readAll();
		file.close();
	} else if (type == "index") {
		if (!m_idIndex.isEmpty()) {
			runner.catFile(m_idIndex, QStringList() << "-p");
			blobData = runner.getResult().toUtf8();
		}
	} else if (type == "repo") {
		if (!m_idRepo.isEmpty()) {
			runner.catFile(m_idRepo, QStringList() << "-p");
			blobData = runner.getResult().toUtf8();
		}
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

const QString& StatusFile::path() const
{
	return m_path;
}

#include "Status.moc"
