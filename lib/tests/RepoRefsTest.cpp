/*
	Cocoon - A GUI for Git.
	Copyright (C) 2010  Riyad Preukschas <riyad@informatik.uni-bremen.de>

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



class RepoRefsTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			cloneFrom("RepoRefsTestRepo");
		}

		void init() {
			GitTestBase::init();
		}

		void cleanup() {
			GitTestBase::cleanup();
		}



		void shouldFindCurrentHead() {
			const Git::Ref &ref = repo->currentHead();

			QCOMPARE(ref.fullName(), QLatin1String("refs/heads/branch"));
		}

		void shouldFindFullNamedHead() {
			const Git::Ref &ref = repo->ref("refs/heads/branch");

			QCOMPARE(ref.fullName(), QLatin1String("refs/heads/branch"));
		}

		void shouldFindPrefixedHead() {
			const Git::Ref &ref = repo->ref("heads/branch");

			QCOMPARE(ref.fullName(), QLatin1String("refs/heads/branch"));
		}

		void shouldFindHeadByName() {
			const Git::Ref &ref = repo->ref("branch");

			QCOMPARE(ref.fullName(), QLatin1String("refs/heads/branch"));
		}

		void shouldReturnHead() {
			const Git::Ref &ref = repo->ref("branch");

			QCOMPARE(ref.metaObject()->className(), "Git::Head");
		}

		void shouldFindFullyNamedTag() {
			const Git::Ref &ref = repo->ref("refs/tags/commit2");

			QCOMPARE(ref.fullName(), QLatin1String("refs/tags/commit2"));
		}

		void shouldFindPrefixedTag() {
			const Git::Ref &ref = repo->ref("tags/commit2");

			QCOMPARE(ref.fullName(), QLatin1String("refs/tags/commit2"));
		}

		void shouldFindTagByName() {
			const Git::Ref &ref = repo->ref("commit2");

			QCOMPARE(ref.fullName(), QLatin1String("refs/tags/commit2"));
		}

		void shouldReturnTag() {
			const Git::Ref &ref = repo->ref("commit2");

			QCOMPARE(ref.metaObject()->className(), "Git::TagRef");
		}
};

QTEST_KDEMAIN_CORE(RepoRefsTest);

#include "RepoRefsTest.moc"
