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



class CommitMergeDetectionTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void initTestCase();
		void cleanup();

		void testMergeDetectionOnUnbranched();
		void testMergeDetectionOnSideBanch();
		void testMergeDetectionOnMergedBanch();

	private:
		Git::CommitList commits;
};

QTEST_KDEMAIN_CORE(CommitMergeDetectionTest)



void CommitMergeDetectionTest::initTestCase()
{
	GitTestBase::initTestCase();

	QProcess::execute("git", gitBasicOpts() << "commit" << "--allow-empty" << "-m" << "Empty inital commit.");

	QProcess::execute("git", gitBasicOpts() << "checkout" << "-b" << "unbranched");
	writeToFile("file_on_master.txt", "foo");
	QProcess::execute("git", gitBasicOpts() << "add" << ".");
	QProcess::execute("git", gitBasicOpts() << "commit" << "-m" << "First commit on unbranched.");

	QProcess::execute("git", gitBasicOpts() << "checkout" << "-b" << "branch");
	writeToFile("file_on_branch.txt", "foo\nbar");
	QProcess::execute("git", gitBasicOpts() << "add" << ".");
	QProcess::execute("git", gitBasicOpts() << "commit" << "-m" << "Commit on branch.");

	QProcess::execute("git", gitBasicOpts() << "checkout" << "unbranched");
	writeToFile("file_on_master.txt", "foo\nbar\nbaz");
	QProcess::execute("git", gitBasicOpts() << "add" << ".");
	QProcess::execute("git", gitBasicOpts() << "commit" << "-m" << "Second commit on unbranched.");

	QProcess::execute("git", gitBasicOpts() << "checkout" << "-b" << "merged");
	QProcess::execute("git", gitBasicOpts() << "merge" << "branch");
}

void CommitMergeDetectionTest::cleanup()
{
	commits.clear();
}



void CommitMergeDetectionTest::testMergeDetectionOnUnbranched()
{
	commits = repo->commits("unbranched");
	QVERIFY(commits.size() == 3);

	QVERIFY(!commits[0]->isMerge());
	QVERIFY(!commits[1]->isMerge());
	QVERIFY(!commits[2]->isMerge());
}

void CommitMergeDetectionTest::testMergeDetectionOnSideBanch()
{
	commits = repo->commits("branch");
	QVERIFY(commits.size() == 3);

	QVERIFY(!commits[0]->isMerge());
	QVERIFY(!commits[1]->isMerge());
	QVERIFY(!commits[2]->isMerge());
}

void CommitMergeDetectionTest::testMergeDetectionOnMergedBanch()
{
	commits = repo->commits("merged");
	QVERIFY(commits.size() == 5);

	QVERIFY(commits[0]->isMerge());

	QVERIFY(!commits[1]->isMerge());
	QVERIFY(!commits[2]->isMerge());
	QVERIFY(!commits[3]->isMerge());
	QVERIFY(!commits[4]->isMerge());
}

#include "CommitMergeDetectionTest.moc"
