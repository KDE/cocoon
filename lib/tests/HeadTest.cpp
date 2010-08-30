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

#include "GitTestBase.h"

#include "Git/Commit.h"
#include "Git/Head.h"



class HeadTest : public GitTestBase
{
	Q_OBJECT

	Git::Head *head;

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			head = 0;

			cloneFrom("HeadTestRepo");
		}

		void init() {
			GitTestBase::init();
			head = new Git::Head(*repo);
		}

		void cleanup() {
			delete head;
			GitTestBase::cleanup();
		}



		void shouldHaveRefsDir() {
			QCOMPARE(head->d->refsDir.path(), QString("%1/refs/heads").arg(repo->gitDir()));
		}

		void shouldHaveCorrectPrefix() {
			const Git::Ref head = Git::Head("master", *repo);

			QCOMPARE(head.prefix(), QLatin1String("heads"));
		}

		void shouldFindAllHeads() {
			QList<Git::Ref> heads = Git::Head(*repo).all();

			QCOMPARE(heads.size(), 2);
			QCOMPARE(heads[0].name(), QString("branch"));
			QCOMPARE(heads[1].name(), QString("master"));
		}

		void shouldPointToCorrectCommit() {
			QString id = "632851d69abeb631f6529b50452611b9915be7fb";

			QCOMPARE(Git::Head("branch", *repo).commit()->id().toSha1String(), id);
		}

		void shouldFindCurrentHead() {
		}
};

QTEST_KDEMAIN_CORE(HeadTest);

#include "HeadTest.moc"
