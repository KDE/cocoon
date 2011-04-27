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

#include "Git/Status.h"



class RepoFileStagingTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void init() {
			workingDir = newTempDirName();
			cloneFrom("RepoFileStagingTestRepo");
			repo = new Git::Repo(workingDir, this);
		}

		void cleanup() {
			delete repo;
			repo = 0;
		}



		void shouldNotStageBogusFile() {
			repo->stageFiles(QStringList() << "bogus.txt");

			QVERIFY(repo->status()->stagedFiles().isEmpty());
		}

		void shouldStageOneFile() {
			repo->stageFiles(QStringList() << "file2.txt");
			Git::Status *status = repo->status();

			QCOMPARE(status->stagedFiles().size(), 1);
			QCOMPARE(status->stagedFiles()[0]->path(), QLatin1String("file2.txt"));
		}

		void shouldStageMultipleFiles() {
			repo->stageFiles(QStringList() << "file1.txt" << "some_dir/file4.txt");
			Git::Status *status = repo->status();

			QCOMPARE(status->stagedFiles().size(), 2);
			QCOMPARE(status->stagedFiles()[0]->path(), QLatin1String("some_dir/file4.txt"));
			QCOMPARE(status->stagedFiles()[1]->path(), QLatin1String("file1.txt"));
		}

		void shouldStageDirectory() {
			repo->stageFiles(QStringList() << "some_dir");
			Git::Status *status = repo->status();

			QCOMPARE(status->stagedFiles().size(), 2);
			QCOMPARE(status->stagedFiles()[0]->path(), QLatin1String("some_dir/file4.txt"));
			QCOMPARE(status->stagedFiles()[1]->path(), QLatin1String("some_dir/file3.txt"));
		}



		void shouldNotUnstageBogusFile() {
			repo->stageFiles(QStringList() << "file1.txt" << "file2.txt" << "some_dir");

			repo->unstageFiles(QStringList() << "bogus.txt");
			Git::Status *status = repo->status();

			QCOMPARE(status->stagedFiles().size(), 4);
		}

		void shouldUnstageOneFile() {
			repo->stageFiles(QStringList() << "file1.txt" << "file2.txt" << "some_dir");

			repo->unstageFiles(QStringList() << "some_dir/file3.txt");
			Git::Status *status = repo->status();

			QCOMPARE(status->stagedFiles().size(), 3);
			QCOMPARE(status->stagedFiles()[0]->path(), QLatin1String("some_dir/file4.txt"));
			QCOMPARE(status->stagedFiles()[1]->path(), QLatin1String("file1.txt"));
			QCOMPARE(status->stagedFiles()[2]->path(), QLatin1String("file2.txt"));
		}

		void shouldUnstageMultipleFiles() {
			repo->stageFiles(QStringList() << "file1.txt" << "file2.txt" << "some_dir");

			repo->unstageFiles(QStringList() << "some_dir/file3.txt" << "file2.txt");
			Git::Status *status = repo->status();

			QCOMPARE(status->stagedFiles().size(), 2);
			QCOMPARE(status->stagedFiles()[0]->path(), QLatin1String("some_dir/file4.txt"));
			QCOMPARE(status->stagedFiles()[1]->path(), QLatin1String("file1.txt"));
		}

		void shouldUnstageDirectory() {
			repo->stageFiles(QStringList() << "file1.txt" << "file2.txt" << "some_dir");

			repo->unstageFiles(QStringList() << "some_dir");
			Git::Status *status = repo->status();

			QCOMPARE(status->stagedFiles().size(), 2);
			QCOMPARE(status->stagedFiles()[0]->path(), QLatin1String("file1.txt"));
			QCOMPARE(status->stagedFiles()[1]->path(), QLatin1String("file2.txt"));
		}

		void shouldNotUnstageInEmptyIndex() {
			repo->unstageFiles(QStringList() << "some_dir");
			Git::Status *status = repo->status();

			QCOMPARE(status->stagedFiles().size(), 0);
		}
};

QTEST_KDEMAIN_CORE(RepoFileStagingTest)

#include "RepoFileStagingTest.moc"
