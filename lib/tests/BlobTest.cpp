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

#include "Git/Blob.h"
#include "Git/RawObject_p.h"



class BlobTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			cloneFrom("BlobTestRepo");
		}



		void shouldHaveDefaultConstructor() {
			Git::Blob blob;

			QCOMPARE(blob.d->id, Git::Id());
		}

		void shouldBeInstanceOfBlob() {
			Git::Id id("a907ec3", *repo);
			Git::RawObject *object = Git::RawObject::newInstance(id);
			QCOMPARE(object->metaObject()->className(), "Git::Blob");
		}

		void shouldBeCorrectObject() {
			Git::Id id("a907ec3", *repo);
			Git::RawObject *object = Git::RawObject::newInstance(id);
			QCOMPARE(object->id().toSha1String(), QLatin1String("a907ec3f431eeb6b1c75799a7e4ba73ca6dc627a"));
		}

		void file1ShouldHaveCorrectContent() {
			Git::Id id("a907ec3", *repo);
			Git::RawObject *object = Git::RawObject::newInstance(id);
			QCOMPARE(object->data(), QByteArray("foo\nbar"));
		}

		void file2ShouldHaveCorrectContent() {
			Git::Id id("5eadda", *repo);
			Git::RawObject *object = Git::RawObject::newInstance(id);
			QCOMPARE(object->data(), QByteArray("foo bar\nbaz\n"));
		}

		void constructedWithDefaultCtorShouldNotBeValid() {
			Git::Blob blob;

			QVERIFY(!blob.isValid());
		}

		void shouldBeCopyable() {
			Git::Blob blob(repo->idFor("5eadda"), *repo);
			Git::Blob copyBlob(repo->idFor("a907ec3"), *repo);
			copyBlob = blob;

			QCOMPARE(copyBlob.d->id.toSha1String(), QLatin1String("5eaddaa51a7494cec355a529631f0dba304cd534"));
		}

		void copyShouldShareInternalData() {
			Git::Blob blob(repo->idFor("5eadda"), *repo);
			Git::Blob copyBlob(repo->idFor("a907ec3"), *repo);
			copyBlob = blob;

			QCOMPARE(copyBlob.d.constData(), blob.d.constData());
		}
};

QTEST_KDEMAIN_CORE(BlobTest)

#include "BlobTest.moc"
