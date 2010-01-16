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

#include "CloneRepositoryProcess.h"

#include <QByteArray>
#include <QDebug>

using namespace Git;



CloneRepositoryProcess::CloneRepositoryProcess(const QString &fromRepo, const QString &toDirectory, QObject *parent)
	: KProcess(parent)
	, m_fromRepo(fromRepo)
	, m_toDirectory(toDirectory)
{
	setWorkingDirectory(m_toDirectory);
	qDebug() << program();
	setOutputChannelMode(KProcess::SeparateChannels);
	setEnvironment(QProcess::systemEnvironment());

	// -v needed for progress reports
	setProgram("git", QStringList() << "clone" << "-v" << m_fromRepo << m_toDirectory);

	// progress is written to stderr
	connect(this, SIGNAL(readyReadStandardError()), this, SLOT(slotReadyReadStandardError()));

	connect(this, SIGNAL(finished(int)), this, SIGNAL(cloningFinished()));
}

void CloneRepositoryProcess::slotReadyReadStandardError()
{
	QString output = readAllStandardError();
	qDebug() << "StdErr:" << output;

	QRegExp progressRegExp("(\\d{1,3})%");

	// output could be multiple lines
	foreach (const QString &line, output.split(QRegExp("\n|\r"))) {
		if (!line.isEmpty()) {
			qDebug() << "Progress:" << line;
			emit cloningProgress(line);

			progressRegExp.indexIn(line);
			emit cloningProgress(progressRegExp.cap(1).toInt());
		}
	}
}

#include "CloneRepositoryProcess.moc"
