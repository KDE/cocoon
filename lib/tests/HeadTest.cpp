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
#include "Git/Ref.h"



class HeadTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			cloneFrom("HeadTestRepo");
		}

		void init() {
			GitTestBase::init();
		}

		void cleanup() {
			GitTestBase::cleanup();
		}



		void shouldHaveRefsDir() {
			Git::Ref head = Git::Ref::head(QString(), *repo);

			QCOMPARE(head.d->refsDir.path(), QString("%1/refs/heads").arg(repo->gitDir()));
		}

		void shouldNotBeRemote() {
			Git::Ref head = Git::Ref::head(QString(), *repo);

			QVERIFY(head.remote().isEmpty());
			QVERIFY(!head.isRemote());
		}

		void shouldHaveCorrectPrefix() {
			Git::Ref head = Git::Ref::head("master", *repo);

			QCOMPARE(head.prefix(), QLatin1String("heads"));
		}

		void shouldFindAllHeads() {
			QList<Git::Ref> heads = Git::Ref(*repo).allHeads();

			QCOMPARE(heads.size(), 2);
			QCOMPARE(heads[0].name(), QString("branch"));
			QCOMPARE(heads[1].name(), QString("master"));
		}

		void shouldPointToCorrectCommit() {
			Git::Ref ref = Git::Ref::head("branch", *repo);

			QCOMPARE(ref.commit().id().toShortSha1String(), QLatin1String("632851d"));
		}
};

QTEST_KDEMAIN_CORE(HeadTest);

#include "HeadTest.moc"
