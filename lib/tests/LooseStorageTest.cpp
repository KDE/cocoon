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
#include "Git/LooseStorage.h"
#include "Git/LooseStorage_p.h"



class LooseStorageTest : public GitTestBase
{
	Q_OBJECT

	Git::LooseStorage *storage;

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			storage = 0;

			cloneFrom("LooseStorageTestRepo");
		}

		void init() {
			GitTestBase::init();
			storage = new Git::LooseStorage(*repo);
		}

		void cleanup() {
			delete storage;
			GitTestBase::cleanup();
		}



		void shouldHaveObjectsDir() {
			QVERIFY(!storage->d->objectsDir.path().isEmpty());
		}

		void shouldHaveCorrectObjectsDir() {
			QCOMPARE(storage->d->objectsDir.path(), QString("%1/objects").arg(repo->gitDir()));
		}

		void testSourceForFullIdIsCorrect() {
			QString sourcePath = storage->sourceFor(Git::Id("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8", *storage));

			QCOMPARE(sourcePath, QString("%1/objects/c5/6dada2cf4f67b35ed0019ddd4651a8c8a337e8").arg(repo->gitDir()));
		}

		void testSourceForShortIdIsCorrect() {
			QString id = "c56dada2cf4f67b35ed0019ddd4651a8c8a337e8";
			QString sourcePath = storage->sourceFor(Git::Id(id.left(7), *storage));

			QCOMPARE(sourcePath, QString("%1/objects/%2/%3").arg(repo->gitDir()).arg(id.left(2)).arg(id.mid(2)));
		}

		void testInflationIsWorking() {
			QString id = "c56dada2cf4f67b35ed0019ddd4651a8c8a337e8";
			QByteArray rawData = storage->rawDataFor(Git::Id(id, *storage));

			QCOMPARE(QTest::toHexRepresentation(rawData, 16), QTest::toHexRepresentation("commit 212\0tree ", 16));
			QCOMPARE(rawData.size(), QString("commit 212").length() + 1 + 212);
		}

		void objectTypeShouldBeCorrect() {
			Git::Id id = repo->commits()[0].id();
			Git::ObjectType type = storage->objectTypeFor(id);

			QCOMPARE(type, Git::OBJ_COMMIT);
		}

		void objectSizeShouldBeCorrect() {
			Git::Id id = repo->commits()[0].id();
			int size = storage->objectSizeFor(id);

			QCOMPARE(size, 212);
		}
};

QTEST_KDEMAIN_CORE(LooseStorageTest);



#include "LooseStorageTest.moc"
