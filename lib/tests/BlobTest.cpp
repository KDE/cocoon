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
#include "Git/RawObject.h"



class BlobTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			cloneFrom("BlobTestRepo");
		}



		void shouldBeInstanceOfBlob() {
			Git::RawObject *object = Git::RawObject::newInstance("a907ec3", *repo);
			QCOMPARE(object->metaObject()->className(), "Git::Blob");
		}

		void shouldBeCorrectObject() {
			Git::RawObject *object = Git::RawObject::newInstance("a907ec3", *repo);
			QCOMPARE(object->id(), QLatin1String("a907ec3f431eeb6b1c75799a7e4ba73ca6dc627a"));
		}

		void file1ShouldHaveCorrectContent() {
			Git::RawObject *object = Git::RawObject::newInstance("a907ec3", *repo);
			QCOMPARE(object->data(), QByteArray("foo\nbar"));
		}

		void file2ShouldHaveCorrectContent() {
			Git::RawObject *object = Git::RawObject::newInstance("5eadda", *repo);
			QCOMPARE(object->data(), QByteArray("foo bar\nbaz\n"));
		}
};

QTEST_KDEMAIN_CORE(BlobTest);

#include "BlobTest.moc"
