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



class RepoCommitIndexTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void init() {
			workingDir = newTempDirName();
			cloneFrom("RepoCommitIndexTestRepo");
			repo = new Git::Repo(workingDir, this);
		}

		void cleanup() {
			delete repo;
			repo = 0;
		}



		void shouldNotCommitWithEmptyIndex() {

		}

		void shouldCommitWithOutMessage() {
			repo->stageFiles(QStringList() << pathTo("some_file.txt"));

			QVERIFY(!repo->status()->forFile("some_file.txt").isEmpty());

			repo->commitIndex("");

			QCOMPARE(repo->commits().size(), 2);
		}

		void shouldCommitWithMessage() {
			repo->stageFiles(QStringList() << pathTo("some_file.txt"));
			repo->commitIndex("Added some file.");

			QCOMPARE(repo->commits().size(), 2);
		}

		void commitWithoutMessageShouldHaveCorrectMessage() {
			repo->stageFiles(QStringList() << pathTo("some_file.txt"));
			repo->commitIndex("");

			/** @todo This will fail for now, as KProcess has a bug handling empty arguments. */
			QCOMPARE(repo->commits()[0]->message(), QLatin1String(""));
		}

		void commitWithMessageShouldHaveCorrectMessage() {
			repo->stageFiles(QStringList() << pathTo("some_file.txt"));
			repo->commitIndex("Added some_file.txt");

			QCOMPARE(repo->commits()[0]->message(), QLatin1String("Added some_file.txt"));
		}
};

QTEST_KDEMAIN_CORE(RepoCommitIndexTest);

#include "RepoCommitIndexTest.moc"
