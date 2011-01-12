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
#include "Git/Commit_p.h"
#include "Git/Tree.h"



class CommitPopulationErrorsTest : public GitTestBase
{
	Q_OBJECT

	Git::Commit commit;

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			commit = Git::Commit();

			cloneFrom("CommitPopulationTestRepo");
		}

		void init() {
			GitTestBase::init();

			Git::Id id = repo->idFor("b462958");
			commit = Git::Commit(id, *repo);
		}

		void cleanup() {
			commit = Git::Commit();
			GitTestBase::cleanup();
		}



		void shouldHandleMissingTree() {
			QStringList rawData;
			//rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "parent abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d";
			rawData << "author Me 1234567890 +0100";
			rawData << "committer You 1234567890 +0100";
			rawData << "";
			rawData << "Some message.";
			rawData << "";
			commit.fillFromString(rawData.join("\n"));

			QVERIFY(!commit.tree().isValid());

			//QCOMPARE(commit.tree().id().toSha1String(), QLatin1String("4b825dc642cb6eb9a060e54bf8d69288fbee4904"));
			QCOMPARE(commit.parents().size(), 1);
			QCOMPARE(commit.author(), QLatin1String("Me"));
			QCOMPARE(commit.committer(), QLatin1String("You"));
			QCOMPARE(commit.message(), QLatin1String("Some message."));
		}

		void shouldHandleAuthoredTimeWithMissingAuthor() {
			QStringList rawData;
			rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "parent abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d";
			rawData << "author 1234567890 +0100"; // name/email missing
			rawData << "committer You 1234567890 +0100";
			rawData << "";
			rawData << "Some message.";
			rawData << "";
			commit.fillFromString(rawData.join("\n"));

			QCOMPARE(commit.author(), QString());

			QCOMPARE(commit.tree().id().toSha1String(), QLatin1String("4b825dc642cb6eb9a060e54bf8d69288fbee4904"));
			QCOMPARE(commit.parents().size(), 1);
			//QCOMPARE(commit.author(), QLatin1String("Me"));
			QCOMPARE(commit.committer(), QLatin1String("You"));
			QCOMPARE(commit.message(), QLatin1String("Some message."));
		}

		void shouldHandleAuthorWithMissingAuthoredTime() {
			QStringList rawData;
			rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "parent abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d";
			rawData << "author Me";// 1234567890 +0100";
			rawData << "committer You 1234567890 +0100";
			rawData << "";
			rawData << "Some message.";
			rawData << "";
			commit.fillFromString(rawData.join("\n"));

			QCOMPARE(commit.authoredAt().toString(), KDateTime().toString());

			QCOMPARE(commit.tree().id().toSha1String(), QLatin1String("4b825dc642cb6eb9a060e54bf8d69288fbee4904"));
			QCOMPARE(commit.parents().size(), 1);
			QCOMPARE(commit.author(), QLatin1String("Me"));
			QCOMPARE(commit.committer(), QLatin1String("You"));
			QCOMPARE(commit.message(), QLatin1String("Some message."));
		}

		void shouldHandleCommitedTimeWithMissingCommitter() {
			QStringList rawData;
			rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "parent abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d";
			rawData << "author Me 1234567890 +0100";
			rawData << "committer 1234567890 +0100"; // name/email missing
			rawData << "";
			rawData << "Some message.";
			rawData << "";
			commit.fillFromString(rawData.join("\n"));

			QCOMPARE(commit.committer(), QString());

			QCOMPARE(commit.tree().id().toSha1String(), QLatin1String("4b825dc642cb6eb9a060e54bf8d69288fbee4904"));
			QCOMPARE(commit.parents().size(), 1);
			QCOMPARE(commit.author(), QLatin1String("Me"));
			//QCOMPARE(commit.committer(), QLatin1String("You"));
			QCOMPARE(commit.message(), QLatin1String("Some message."));
		}

		void shouldHandleCommitterWithMissingCommitedTime() {
			QStringList rawData;
			rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "parent abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d";
			rawData << "author Me 1234567890 +0100";
			rawData << "committer You";// 1234567890 +0100";
			rawData << "";
			rawData << "Some message.";
			rawData << "";
			commit.fillFromString(rawData.join("\n"));

			QCOMPARE(commit.committedAt().toString(), KDateTime().toString());

			QCOMPARE(commit.tree().id().toSha1String(), QLatin1String("4b825dc642cb6eb9a060e54bf8d69288fbee4904"));
			QCOMPARE(commit.parents().size(), 1);
			QCOMPARE(commit.author(), QLatin1String("Me"));
			QCOMPARE(commit.committer(), QLatin1String("You"));
			QCOMPARE(commit.message(), QLatin1String("Some message."));
		}

		void shouldHandleCutOffData() {
			QStringList rawData;
			rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "parent abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d";
			rawData << "author Me";// 1234567890 +0100";
			//rawData << "committer You 1234567890 +0100";
			//rawData << "";
			//rawData << "Some message.";
			//rawData << "";
			commit.fillFromString(rawData.join("\n"));

			QCOMPARE(commit.authoredAt().toString(), KDateTime().toString());

			QCOMPARE(commit.tree().id().toSha1String(), QLatin1String("4b825dc642cb6eb9a060e54bf8d69288fbee4904"));
			QCOMPARE(commit.parents().size(), 1);
			QCOMPARE(commit.parents()[0].id().toSha1String(), QLatin1String("abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d"));
			QCOMPARE(commit.author(), QLatin1String("Me"));
			QCOMPARE(commit.committer(), QString());
			QCOMPARE(commit.message(), QString());
		}
};

QTEST_KDEMAIN_CORE(CommitPopulationErrorsTest);

#include "CommitPopulationErrorsTest.moc"
