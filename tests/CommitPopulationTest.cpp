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



class CommitPopulationTest : public GitTestBase
{
	Q_OBJECT

	Git::Commit *commit;

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			commit = 0;
		}

		void init() {
			GitTestBase::init();
			commit = new Git::Commit("1111111");
		}

		void cleanup() {
			delete commit;
			GitTestBase::cleanup();
		}



		void shouldPopulateTreeCorrectly() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "author Me 1234567890 +0100";
			rawData << "committer You 1234567890 +0100";
			rawData << "";
			rawData << "    Some message.";
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			QCOMPARE(commit->tree(), QString("2222222"));
		}

		void shouldPopulateNoParentCorrectly() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "author Me 1234567890 +0100";
			rawData << "committer You 1234567890 +0100";
			rawData << "";
			rawData << "    Some message.";
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			QCOMPARE(commit->parents().size(), 0);
		}

		void shouldPopulateSingleParentCorrectly() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "author Me 1234567890 +0100";
			rawData << "committer You 1234567890 +0100";
			rawData << "";
			rawData << "    Some message.";
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			QCOMPARE(commit->parents().size(), 1);
			QCOMPARE(commit->parents().first()->id(), QString("3333333"));
		}

		void shouldPopulateMultiParentCorrectly() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "parent 4444444";
			rawData << "parent 5555555";
			rawData << "author Me 1234567890 +0100";
			rawData << "committer You 1234567890 +0100";
			rawData << "";
			rawData << "    Some message.";
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			QCOMPARE(commit->parents().size(), 3);
			QCOMPARE(commit->parents()[0]->id(), QString("3333333"));
			QCOMPARE(commit->parents()[1]->id(), QString("4444444"));
			QCOMPARE(commit->parents()[2]->id(), QString("5555555"));
		}

		void shouldPopulateAuthorCorrectly() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "author Me 1234567890 +0100";
			rawData << "committer You 1234567890 +0100";
			rawData << "";
			rawData << "    Some message.";
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			QCOMPARE(commit->author(), QString("Me"));
		}

		void shouldPopulateAuthorWithEmailCorrectly() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "author Me <me@some.tld> 1234567890 +0100";
			rawData << "committer You <you@some.tld> 1234567890 +0100";
			rawData << "";
			rawData << "    Some message.";
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			QCOMPARE(commit->author(), QString("Me <me@some.tld>"));
		}

		void shouldPopulateAuthoredAdCorrectly() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "author Me 1234567890 +0100";
			rawData << "committer You 1234567890 +0100";
			rawData << "";
			rawData << "    Some message.";
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			KDateTime authoredAt;
			authoredAt.setTime_t(1234567890);
			authoredAt = authoredAt.addSecs(3600/*==1h*/);
			QCOMPARE(commit->authoredAt().toString(), authoredAt.toString());
		}

		void shouldPopulateCommitterCorrectly() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "author Me 1234567890 +0100";
			rawData << "committer You 1234567890 +0100";
			rawData << "";
			rawData << "    Some message.";
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			QCOMPARE(commit->committer(), QString("You"));
		}

		void shouldPopulateCommitterWithEmailCorrectly() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "author Me <me@some.tld> 1234567890 +0100";
			rawData << "committer You <you@some.tld> 1234567890 +0100";
			rawData << "";
			rawData << "    Some message.";
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			QCOMPARE(commit->committer(), QString("You <you@some.tld>"));
		}

		void shouldPopulateCommittedAtCorrectly() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "author Me 1234567890 +0100";
			rawData << "committer You 1234567890 +0100";
			rawData << "";
			rawData << "    Some message.";
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			KDateTime committedAt;
			committedAt.setTime_t(1234567890);
			committedAt = committedAt.addSecs(3600/*==1h*/);
			QCOMPARE(commit->committedAt().toString(), committedAt.toString());
		}

		void shouldPopulateSingleLineMessageCorrectly() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "author Me 1234567890 +0100";
			rawData << "committer You 1234567890 +0100";
			rawData << "";
			rawData << "    Some message.";
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			QCOMPARE(commit->message(), QString("Some message."));
		}

		void shouldPopulateSingleLineMessageSummaryCorrectly() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "author Me 1234567890 +0100";
			rawData << "committer You 1234567890 +0100";
			rawData << "";
			rawData << "    Some message.";
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			QCOMPARE(commit->summary(), QString("Some message."));
		}

		void shouldPopulateMultiLineMessageCorrectly() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "author Me 1234567890 +0100";
			rawData << "committer You 1234567890 +0100";
			rawData << "";
			rawData << "    Some message.";
			rawData << "    More message.";
			rawData << "    Even more message.";
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			QCOMPARE(commit->message(), QString("Some message.\nMore message.\nEven more message."));
		}

		void shouldPopulateMultiLineMessageSummaryCorrectly() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "author Me 1234567890 +0100";
			rawData << "committer You 1234567890 +0100";
			rawData << "";
			rawData << "    Some message.";
			rawData << "    More message.";
			rawData << "    Even more message.";
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			QCOMPARE(commit->summary(), QString("Some message."));
		}
};

QTEST_KDEMAIN_CORE(CommitPopulationTest);

#include "CommitPopulationTest.moc"
