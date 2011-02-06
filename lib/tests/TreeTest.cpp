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

#include "Git/Tree.h"
#include "Git/Tree_p.h"
#include "Git/RawObject.h"



class TreeTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			cloneFrom("TreeTestRepo");
		}



		void shouldBeInstanceOfTree() {
			Git::Id id("273b4fb", *repo);
			Git::RawObject *object = Git::RawObject::newInstance(id);

			QCOMPARE(object->metaObject()->className(), "Git::Tree");
		}

		void shouldBeCorrectObject() {
			Git::Id id("273b4fb", *repo);
			Git::RawObject *object = Git::RawObject::newInstance(id);

			QCOMPARE(object->id().toSha1String(), QLatin1String("273b4fbfa8910e2806d6999c8433f29c95c1ac86"));
			QCOMPARE(object->data().size(), 97);
		}

		void shouldNotPopulateOnConstruction() {
			Git::Id id("273b4fb", *repo);
			Git::Tree *tree = (Git::Tree*)Git::RawObject::newInstance(id);

			QVERIFY( tree->d->entries.isEmpty());
			QVERIFY( tree->d->entryModes.isEmpty());
			QVERIFY( tree->d->entryNames.isEmpty());
		}

		void shouldPopulateOnPropertyAccess() {
			Git::Id id("273b4fb", *repo);
			Git::Tree *tree = (Git::Tree*)Git::RawObject::newInstance(id);
			tree->entries();

			QVERIFY(!tree->d->entries.isEmpty());
			QVERIFY(!tree->d->entryModes.isEmpty());
			QVERIFY(!tree->d->entryNames.isEmpty());
		}

		void shouldHaveCorrectNumberOfEntries() {
			Git::Tree tree = repo->tree(repo->idFor("273b4fb"));

			QCOMPARE(tree.entries().count(), 3);
			QCOMPARE(tree.entriesByName().count(), 3);
		}

		void shouldHaveCorrectEntries() {
			Git::Tree tree = repo->tree(repo->idFor("273b4fb"));

			QStringList entryIds;
			entryIds << "5b51924c72272342a7490dd267ed022e90174480"; // dir0
			entryIds << "a907ec3f431eeb6b1c75799a7e4ba73ca6dc627a"; // file1
			entryIds << "eb697c0d58b8e5fce1855b606a665c4a2ad3a1c7"; // file2

			foreach(const Git::RawObject entry, tree.entries()) {
				QVERIFY(entryIds.contains(entry.id().toSha1String()));
			}
		}

		void shouldHaveCorrectEntryNames_data() {
			QTest::addColumn<QString>("entryName");
			QTest::addColumn<QString>("entryId");

			QTest::newRow("dir0")  << QString("dir0" ) << QString("5b51924c72272342a7490dd267ed022e90174480");
			QTest::newRow("file1") << QString("file1") << QString("a907ec3f431eeb6b1c75799a7e4ba73ca6dc627a");
			QTest::newRow("file2") << QString("file2") << QString("eb697c0d58b8e5fce1855b606a665c4a2ad3a1c7");
		}

		void shouldHaveCorrectEntryNames() {
			Git::Tree tree = repo->tree(repo->idFor("273b4fb"));
			QFETCH(QString, entryName);
			QFETCH(QString, entryId);

			QMap<QString, Git::RawObject> entries = tree.entriesByName();
			QCOMPARE(entries[entryName].id().toSha1String(), entryId);

			QVERIFY(!entries["none_existent"].isValid());
		}

		void shouldHaveCorrectNumberOfTrees() {
			Git::Tree tree = repo->tree(repo->idFor("273b4fb"));

			QCOMPARE(tree.trees().count(), 1);
			QCOMPARE(tree.treesByName().count(), 1);
		}

		void shouldHaveCorrectNumberOfBlobs() {
			Git::Tree tree = repo->tree(repo->idFor("273b4fb"));

			QCOMPARE(tree.blobs().count(), 2);
			QCOMPARE(tree.blobsByName().count(), 2);
		}

		void shouldFindCorrectNameForObject_data() {
			QTest::addColumn<QString>("entryId");
			QTest::addColumn<QString>("entryName");

			QTest::newRow("none")  << QString("273b4fbfa8910e2806d6999c8433f29c95c1ac86") << QString();
			QTest::newRow("dir0")  << QString("5b51924c72272342a7490dd267ed022e90174480") << QString("dir0");
			QTest::newRow("file1") << QString("a907ec3f431eeb6b1c75799a7e4ba73ca6dc627a") << QString("file1");
			QTest::newRow("file2") << QString("eb697c0d58b8e5fce1855b606a665c4a2ad3a1c7") << QString("file2");
		}

		void shouldFindCorrectNameForObject() {
			Git::Tree tree = repo->tree(repo->idFor("273b4fb"));
			QFETCH(QString, entryId);
			QFETCH(QString, entryName);

			QCOMPARE(tree.nameFor(entryId), entryName);
		}
};

QTEST_KDEMAIN_CORE(TreeTest);

#include "TreeTest.moc"
