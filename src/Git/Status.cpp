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
//	foreach(const String &file, untrackedFiles()) {
//		QHash<QString, QString> data;
//		data["path"] = file;
//		data["staged"] = "false";
//		data["status"] = "U";
//
//		addFile(file, data);
//	}

	// find modified in tree
	filesData = diffFiles();
	for(int i=0; i < filesData.size(); ++i) {
		QString key = filesData.keys()[i];

		// if a file shows up here it has not yet been staged
		// @info staged deleted files don't show up in diff-files
		filesData[key]["staged"] = "false";

		addFile(key, filesData[key]);
	}

	// find added but not committed - new files
	filesData = diffIndex("HEAD");
	for(int i=0; i < filesData.size(); ++i) {
		QString file = filesData.keys()[i];

		// the file has been staged
		// if the file has a index id or is marked as deleted
		// @info staged deleted files have no index id
		if (!QRegExp("^[0]*$").exactMatch(filesData[file]["idIndex"]) || filesData[file]["status"] == "D"
			|| m_status[file]->m_idRepo != filesData[file]["idRepo"]) {
			filesData[file]["staged"] = "true";
		}

		if (!(m_status.contains(file) && m_status[file]->m_status == "D")) {
			addFile(file, filesData[file]);
		}
	}
/*
				@status.each do |file, data_array|
					if data_array.size > 1
						@status[file] = data_array.map{ |data| StatusFile.new(@base, data) }
					else
						@status[file] = StatusFile.new(@base, data_array.first)
					end
				end
				@files = @status.values.flatten
*/
	m_files = m_status.values();
}

void Status::addFile(const QString &file, QHash<QString, QString> data)
{
/*
				if @status[file]
					last_data = @status[file].last
					if !last_data[:status] || data[:staged].nil?
						@status[file][-1] =  data.merge!(last_data)
					else
						@status[file] << data
					end
				else
					@status[file] = [data]
				end
*/
	if (!m_status.contains(file)) {
		m_status[file] = new StatusFile(this);
	}

	StatusFile *statusFile = m_status[file];

	statusFile->m_idIndex = data["idIndex"];
	statusFile->m_idRepo = data["idRepo"];
	statusFile->m_modeIndex = data["modeIndex"];
	statusFile->m_modeRepo = data["modeRepo"];
	statusFile->m_path = data["path"];
	statusFile->m_staged = data["staged"] == "true";
	statusFile->m_status = data["status"];
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
		if (file->isStaged()) {
			files << file;
		}
	}

	return files;
}

QString Status::unescapeFileName(const QString &escapedName) const
{
	return escapedName;
}



StatusFile::StatusFile(Status *parent)
	: QObject(parent)
{
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

const QString& StatusFile::path() const
{
	return m_path;
}

#include "Status.moc"
