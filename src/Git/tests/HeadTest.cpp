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

#include "Git/Head.h"



class HeadTest : public GitTestBase
{
	Q_OBJECT

	Git::Head *head;

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			head = 0;

			QProcess::execute("git", gitBasicOpts() << "commit" << "--allow-empty" << "-m" << "Init empty repo.");

			QProcess::execute("git", gitBasicOpts() << "checkout" << "-b" << "branch");
			writeToFile("branch_file.txt", "foo\n");
			QProcess::execute("git", gitBasicOpts() << "add" << "branch_file.txt");
			QProcess::execute("git", gitBasicOpts() << "commit" << "-m" << "Added branch_file.");

			QProcess::execute("git", gitBasicOpts() << "checkout" << "master");
			writeToFile("master_file.txt", "foo\nbar\baz");
			QProcess::execute("git", gitBasicOpts() << "add" << "master_file.txt");
			QProcess::execute("git", gitBasicOpts() << "commit" << "-m" << "Added master_file.");

			QProcess::execute("git", gitBasicOpts() << "merge" << "branch");
		}

		void init() {
			GitTestBase::init();
			head = new Git::Head(repo);
		}

		void cleanup() {
			delete head;
			GitTestBase::cleanup();
		}



		void shouldHaveRefsDir() {
			QCOMPARE(head->m_refsDir.path(), QString("%1/refs/heads").arg(repo->gitDir()));
		}

		void shouldFindAllHeads() {
			Git::RefList heads = Git::Head(repo).all();
			QCOMPARE(heads.size(), 2);
			QCOMPARE(heads[0]->name(), QString("branch"));
			QCOMPARE(heads[1]->name(), QString("master"));
		}

		void shouldPointToCorrectCommit() {
			QString id = repo->commits("branch")[0]->id();

			QCOMPARE(Git::Head("branch", repo).commit(), id);
		}

		void shouldFindCurrentHead() {
		}
};

QTEST_KDEMAIN_CORE(HeadTest);

#include "HeadTest.moc"
