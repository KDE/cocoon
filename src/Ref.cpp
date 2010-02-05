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

#include "Ref.h"

#include "Repo.h"

#include <KDebug>

using namespace Git;



Ref::Ref(const QString &prefix, const Repo &repo)
	: QObject((QObject*)&repo)
	, m_repo(repo)
{
	m_refsDir = QDir(m_repo.gitDir());
	m_refsDir.cd("refs");
	m_refsDir.cd(prefix);
}

Ref::Ref(const QString &name, const QString &prefix, const Repo &repo)
	: QObject((QObject*)&repo)
	, m_commit()
	, m_name(name)
	, m_repo(repo)
{
	m_refsDir = QDir(m_repo.gitDir());
	m_refsDir.cd("refs");
	m_refsDir.cd(prefix);

	populate();
}



RefList Ref::all() const
{
	RefList refs;

	foreach (const QString &name, m_refsDir.entryList(QDir::Files)) {
		kDebug() << "ref found:" << name;
		refs << newInstance(name, m_repo);
	}

	return refs;
}

const QString& Ref::name() const
{
	return m_name;
}

Commit* Ref::commit() const
{
	return m_commit;
}

void Ref::populate()
{
	QFile refFile(m_refsDir.filePath(m_name));
	refFile.open(QFile::ReadOnly);

	QString commitId = refFile.readAll().trimmed();

	kDebug() << "reading head:" << refFile.fileName();
	kDebug() << "head content:" << commitId;

	m_commit = m_repo.commit(commitId);

	refFile.close();
}



#include "Ref.moc"
