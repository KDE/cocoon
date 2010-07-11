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

#include "Git/Id.h"
#include "Git/Id_p.h"
#include "Git/ObjectStorage.h"



class IdTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			cloneFrom("IdTestRepo");
		}

		void init() {
			GitTestBase::init();
		}

		void cleanup() {
			GitTestBase::cleanup();
		}



		void shouldHaveDefaultConstructor() {
			Git::Id id = Git::Id();

			QCOMPARE(id.d->sha1, QString());
			QVERIFY(!id.d->storage);
		}

		void shouldInstantiateWithRepo() {
			Git::Id id("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7", *repo);

			QCOMPARE(id.d->sha1, QString("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7"));
			QVERIFY(id.d->storage);
		}

		void shouldInstantiateWithStorage() {
			Git::Id id("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7", *repo->storages()[0]);

			QCOMPARE(id.d->sha1, QString("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7"));
			QCOMPARE(id.d->storage, repo->storages()[0]);
		}

		void shouldBeCopyable() {
			Git::Id id("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7", *repo->storages()[0]);
			Git::Id copyId = id;

			QCOMPARE(copyId.d->sha1, QString("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7"));
			QCOMPARE(copyId.d->storage, repo->storages()[0]);
		}

		void copyShouldShareInternalData() {
			Git::Id id("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7", *repo->storages()[0]);
			Git::Id copyId = id;

			QCOMPARE(id.d.constData(), copyId.d.constData());
		}

		void constructedWithDefaultCtorShouldNotBeValid() {
			Git::Id id = Git::Id();

			QVERIFY(!id.isValid());
		}

		void existingIdInRepoShouldBeValid() {
			Git::Id id("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7", *repo);

			QVERIFY(id.isValid());
			QCOMPARE(id.d->storage, repo->storages()[0]);
		}

		void existingIdInStorageShouldBeValid() {
			Git::Id id("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7", *repo->storages()[0]);

			QVERIFY(id.isValid());
			QCOMPARE(id.d->storage, repo->storages()[0]);
		}

		void nonExistingIdInRepoShouldNotBeValid() {
			Git::Id id("1234567", *repo);

			QVERIFY(!id.isValid());
			QCOMPARE(id.d->sha1, QString());
			QVERIFY(!id.d->storage);
		}

		void nonExistingIdInStorageShouldNonBeValid() {
			Git::Id id("1234567", *repo->storages()[0]);

			QVERIFY(!id.isValid());
			QCOMPARE(id.d->sha1, QString());
			QVERIFY(!id.d->storage);
		}

		void shouldDetermineExistanceCorrectly() {
			Git::Id id("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7", *repo->storages()[0]);

			QVERIFY(id.exists());
			QVERIFY(id.storage().contains(id.toSha1String()));
		}

		void shouldFindActualIdInRepo() {
			Git::Id id("d97d4abb", *repo);

			QCOMPARE(id.toSha1String(), QString("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7"));
		}

		void shouldFindActualIdInStorage() {
			Git::Id id("d97d4abb", *repo->storages()[0]);

			QCOMPARE(id.toSha1String(), QString("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7"));
		}

		void shouldProduceCorrectShortId() {
			Git::Id id("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7", *repo->storages()[0]);

			QCOMPARE(id.toShortSha1String(), QString("d97d4ab"));
		}
};

QTEST_KDEMAIN_CORE(IdTest);

#include "IdTest.moc"
