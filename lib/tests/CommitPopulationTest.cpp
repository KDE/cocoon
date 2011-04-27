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



class CommitPopulationTest : public GitTestBase
{
	Q_OBJECT

	Git::Commit *commit;

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			cloneFrom("CommitPopulationTestRepo");
		}

		void init() {
			GitTestBase::init();

			Git::Id id = repo->idFor("b462958");
			commit = new Git::Commit(id, *repo);
		}

		void cleanup() {
			delete commit;
			GitTestBase::cleanup();
		}



		void shouldNotPopulateOnConstruction() {
			QVERIFY( commit->d->author.isNull());
			QVERIFY( commit->d->authoredAt.isNull());
			QVERIFY( commit->d->committer.isNull());
			QVERIFY( commit->d->committedAt.isNull());
			QVERIFY( commit->d->message.isNull());
			QVERIFY( commit->d->parentIds.isEmpty());
			QVERIFY( commit->d->summary.isNull());
			QVERIFY(!commit->d->treeId.isValid());
		}

		void shouldPopulateOnPropertyAccess() {
			commit->message();

			QVERIFY(!commit->d->author.isEmpty());
			QVERIFY(!commit->d->authoredAt.isNull());
			QVERIFY(!commit->d->committer.isEmpty());
			QVERIFY(!commit->d->committedAt.isNull());
			QVERIFY(!commit->d->message.isEmpty());
			QVERIFY(!commit->d->parentIds.isEmpty());
			QVERIFY(!commit->d->summary.isEmpty());
			QVERIFY( commit->d->treeId.isValid());
		}

		void shouldPopulateTreeCorrectly() {
			QStringList rawData;
			rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "parent abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d";
			rawData << "author Me 1234567890 -0230";
			rawData << "committer You 1234567890 -0230";
			rawData << "";
			rawData << "Some message.";
			rawData << "";
			commit->fillFromString(rawData.join("\n"));

			QCOMPARE(commit->tree().id().toSha1String(), QLatin1String("4b825dc642cb6eb9a060e54bf8d69288fbee4904"));
		}

		void shouldPopulateNoParentCorrectly() {
			QStringList rawData;
			rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "author Me 1234567890 -0230";
			rawData << "committer You 1234567890 -0230";
			rawData << "";
			rawData << "Some message.";
			rawData << "";
			commit->fillFromString(rawData.join("\n"));

			QCOMPARE(commit->parents().size(), 0);
		}

		void shouldPopulateSingleParentCorrectly() {
			QStringList rawData;
			rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "parent abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d";
			rawData << "author Me 1234567890 -0230";
			rawData << "committer You 1234567890 -0230";
			rawData << "";
			rawData << "Some message.";
			rawData << "";
			commit->fillFromString(rawData.join("\n"));

			QCOMPARE(commit->parents().size(), 1);
			QCOMPARE(commit->parents().first().id().toSha1String(), QLatin1String("abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d"));
		}

		void shouldPopulateMultiParentCorrectly() {
			QStringList rawData;
			rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "parent abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d";
			rawData << "parent 6421f09a627d8ea6a85a9155e481cae7ed483b50";
			rawData << "parent 4262f0d5b0d062a0d655f16c2fc372c92689c853";
			rawData << "author Me 1234567890 -0230";
			rawData << "committer You 1234567890 -0230";
			rawData << "";
			rawData << "Some message.";
			rawData << "";
			commit->fillFromString(rawData.join("\n"));

			QCOMPARE(commit->parents().size(), 3);
			QCOMPARE(commit->parents()[0].id().toSha1String(), QLatin1String("abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d"));
			QCOMPARE(commit->parents()[1].id().toSha1String(), QLatin1String("6421f09a627d8ea6a85a9155e481cae7ed483b50"));
			QCOMPARE(commit->parents()[2].id().toSha1String(), QLatin1String("4262f0d5b0d062a0d655f16c2fc372c92689c853"));
		}

		void shouldPopulateAuthorCorrectly() {
			QStringList rawData;
			rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "parent abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d";
			rawData << "author Me 1234567890 -0230";
			rawData << "committer You 1234567890 -0230";
			rawData << "";
			rawData << "Some message.";
			rawData << "";
			commit->fillFromString(rawData.join("\n"));

			QCOMPARE(commit->author(), QLatin1String("Me"));
		}

		void shouldPopulateAuthorWithEmailCorrectly() {
			QStringList rawData;
			rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "parent abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d";
			rawData << "author Me <me@some.tld> 1234567890 -0230";
			rawData << "committer You <you@some.tld> 1234567890 -0230";
			rawData << "";
			rawData << "Some message.";
			rawData << "";
			commit->fillFromString(rawData.join("\n"));

			QCOMPARE(commit->author(), QLatin1String("Me <me@some.tld>"));
		}

		void shouldPopulateAuthoredAtCorrectly() {
			QStringList rawData;
			rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "parent abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d";
			rawData << "author Me 1234567890 -0230";
			rawData << "committer You 1234567890 -0230";
			rawData << "";
			rawData << "Some message.";
			rawData << "";
			commit->fillFromString(rawData.join("\n"));

			KDateTime authoredAt;
			authoredAt.setTime_t(1234567890);
			authoredAt.setTimeSpec(KDateTime::Spec(KDateTime::OffsetFromUTC, -9000/*==2,5h*/));
			QCOMPARE(commit->authoredAt().toString(), authoredAt.toString());
			QCOMPARE(commit->authoredAt().utcOffset(), -9000);
		}

		void shouldPopulateCommitterCorrectly() {
			QStringList rawData;
			rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "parent abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d";
			rawData << "author Me 1234567890 -0230";
			rawData << "committer You 1234567890 -0230";
			rawData << "";
			rawData << "Some message.";
			rawData << "";
			commit->fillFromString(rawData.join("\n"));

			QCOMPARE(commit->committer(), QLatin1String("You"));
		}

		void shouldPopulateCommitterWithEmailCorrectly() {
			QStringList rawData;
			rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "parent abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d";
			rawData << "author Me <me@some.tld> 1234567890 -0230";
			rawData << "committer You <you@some.tld> 1234567890 -0230";
			rawData << "";
			rawData << "Some message.";
			rawData << "";
			commit->fillFromString(rawData.join("\n"));

			QCOMPARE(commit->committer(), QLatin1String("You <you@some.tld>"));
		}

		void shouldPopulateCommittedAtCorrectly() {
			QStringList rawData;
			rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "parent abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d";
			rawData << "author Me 1234567890 -0230";
			rawData << "committer You 1234567890 -0230";
			rawData << "";
			rawData << "Some message.";
			rawData << "";
			commit->fillFromString(rawData.join("\n"));

			KDateTime committedAt;
			committedAt.setTime_t(1234567890);
			committedAt.setTimeSpec(KDateTime::Spec(KDateTime::OffsetFromUTC, -9000/*==2,5h*/));
			QCOMPARE(commit->committedAt().toString(), committedAt.toString());
			QCOMPARE(commit->committedAt().utcOffset(), -9000);
		}

		void shouldPopulateSingleLineMessageCorrectly() {
			QStringList rawData;
			rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "parent abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d";
			rawData << "author Me 1234567890 -0230";
			rawData << "committer You 1234567890 -0230";
			rawData << "";
			rawData << "Some message.";
			rawData << "";
			commit->fillFromString(rawData.join("\n"));

			QCOMPARE(commit->message(), QLatin1String("Some message."));
		}

		void shouldPopulateSingleLineMessageSummaryCorrectly() {
			QStringList rawData;
			rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "parent abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d";
			rawData << "author Me 1234567890 -0230";
			rawData << "committer You 1234567890 -0230";
			rawData << "";
			rawData << "Some message.";
			rawData << "";
			commit->fillFromString(rawData.join("\n"));

			QCOMPARE(commit->summary(), QLatin1String("Some message."));
		}

		void shouldPopulateMultiLineMessageCorrectly() {
			QStringList rawData;
			rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "parent abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d";
			rawData << "author Me 1234567890 -0230";
			rawData << "committer You 1234567890 -0230";
			rawData << "";
			rawData << "Some message.";
			rawData << "More message.";
			rawData << "Even more message.";
			rawData << "";
			commit->fillFromString(rawData.join("\n"));

			QCOMPARE(commit->message(), QLatin1String("Some message.\nMore message.\nEven more message."));
		}

		void shouldPopulateMultiLineMessageSummaryCorrectly() {
			QStringList rawData;
			rawData << "tree 4b825dc642cb6eb9a060e54bf8d69288fbee4904";
			rawData << "parent abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d";
			rawData << "author Me 1234567890 -0230";
			rawData << "committer You 1234567890 -0230";
			rawData << "";
			rawData << "Some message.";
			rawData << "More message.";
			rawData << "Even more message.";
			rawData << "";
			commit->fillFromString(rawData.join("\n"));

			QCOMPARE(commit->summary(), QLatin1String("Some message."));
		}
};

QTEST_KDEMAIN_CORE(CommitPopulationTest)

#include "CommitPopulationTest.moc"
