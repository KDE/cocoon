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



class CommitPopulationErrorsTest : public GitTestBase
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



		void shouldHandleMissingTree() {
			QStringList rawData;
			//rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "author Me 1234567890 +0100";
			rawData << "committer You 1234567890 +0100";
			rawData << "";
			rawData << "    Some message.";
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			QCOMPARE(commit->tree(), QString());

			//QCOMPARE(commit->tree(), QString("2222222"));
			QCOMPARE(commit->parents().size(), 1);
			QCOMPARE(commit->author(), QString("Me"));
			QCOMPARE(commit->committer(), QString("You"));
			QCOMPARE(commit->message(), QString("Some message."));
		}

		void shouldHandleAuthoredTimeWithMissingAuthor() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "author 1234567890 +0100"; // name/email missing
			rawData << "committer You 1234567890 +0100";
			rawData << "";
			rawData << "    Some message.";
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			QCOMPARE(commit->author(), QString());

			QCOMPARE(commit->tree(), QString("2222222"));
			QCOMPARE(commit->parents().size(), 1);
			//QCOMPARE(commit->author(), QString("Me"));
			QCOMPARE(commit->committer(), QString("You"));
			QCOMPARE(commit->message(), QString("Some message."));
		}

		void shouldHandleAuthorWithMissingAuthoredTime() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "author Me";// 1234567890 +0100";
			rawData << "committer You 1234567890 +0100";
			rawData << "";
			rawData << "    Some message.";
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			QCOMPARE(commit->authoredAt().toString(), KDateTime().toString());

			QCOMPARE(commit->tree(), QString("2222222"));
			QCOMPARE(commit->parents().size(), 1);
			QCOMPARE(commit->author(), QString("Me"));
			QCOMPARE(commit->committer(), QString("You"));
			QCOMPARE(commit->message(), QString("Some message."));
		}

		void shouldHandleCommitedTimeWithMissingCommitter() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "author Me 1234567890 +0100";
			rawData << "committer 1234567890 +0100"; // name/email missing
			rawData << "";
			rawData << "    Some message.";
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			QCOMPARE(commit->committer(), QString());

			QCOMPARE(commit->tree(), QString("2222222"));
			QCOMPARE(commit->parents().size(), 1);
			QCOMPARE(commit->author(), QString("Me"));
			//QCOMPARE(commit->committer(), QString("You"));
			QCOMPARE(commit->message(), QString("Some message."));
		}

		void shouldHandleCommitterWithMissingCommitedTime() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "author Me 1234567890 +0100";
			rawData << "committer You";// 1234567890 +0100";
			rawData << "";
			rawData << "    Some message.";
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			QCOMPARE(commit->committedAt().toString(), KDateTime().toString());

			QCOMPARE(commit->tree(), QString("2222222"));
			QCOMPARE(commit->parents().size(), 1);
			QCOMPARE(commit->author(), QString("Me"));
			QCOMPARE(commit->committer(), QString("You"));
			QCOMPARE(commit->message(), QString("Some message."));
		}

		void shouldIgnoreUnidentedMessage() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "author Me 1234567890 +0100";
			rawData << "committer You 1234567890 +0100";
			rawData << "";
			rawData << "Some message."; // missing leading spaces
			rawData << "More message."; // missing leading spaces
			rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			QCOMPARE(commit->message(), QString());

			QCOMPARE(commit->tree(), QString("2222222"));
			QCOMPARE(commit->parents().size(), 1);
			QCOMPARE(commit->author(), QString("Me"));
			QCOMPARE(commit->committer(), QString("You"));
			//QCOMPARE(commit->message(), QString("Some message.\nMore message."));
		}

		void shouldHandleCutOffData() {
			QStringList rawData;
			rawData << "tree 2222222";
			rawData << "parent 3333333";
			rawData << "author Me";// 1234567890 +0100";
			//rawData << "committer You 1234567890 +0100";
			//rawData << "";
			//rawData << "    Some message.";
			//rawData << "";
			Git::Commit::fillFromString(commit, rawData.join("\n"));

			QCOMPARE(commit->authoredAt().toString(), KDateTime().toString());

			QCOMPARE(commit->tree(), QString("2222222"));
			QCOMPARE(commit->parents().size(), 1);
			QCOMPARE(commit->parents()[0]->id(), QString("3333333"));
			QCOMPARE(commit->author(), QString("Me"));
			QCOMPARE(commit->committer(), QString());
			QCOMPARE(commit->message(), QString());
		}
};

QTEST_KDEMAIN_CORE(CommitPopulationErrorsTest);

#include "CommitPopulationErrorsTest.moc"
