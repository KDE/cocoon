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
			Git::RawObject *object = Git::RawObject::newInstance("273b4fb", *repo);
			QCOMPARE(object->metaObject()->className(), "Git::Tree");
		}

		void shouldBeCorrectObject() {
			Git::RawObject *object = Git::RawObject::newInstance("273b4fb", *repo);
			QCOMPARE(object->id(), QLatin1String("273b4fbfa8910e2806d6999c8433f29c95c1ac86"));
			QCOMPARE(object->data().size(), 97);
		}

		void shouldHaveCorrectNumberOfEntries() {
			Git::Tree *tree = repo->tree("273b4fb");
			QCOMPARE(tree->entries().count(), 3);
			QCOMPARE(tree->entriesByName().count(), 3);
		}

		void shouldHaveCorrectNumberOfTrees() {
			Git::Tree *tree = repo->tree("273b4fb");
			QCOMPARE(tree->trees().count(), 1);
			QCOMPARE(tree->treesByName().count(), 1);
		}

		void shouldHaveCorrectNumberOfBlobs() {
			Git::Tree *tree = repo->tree("273b4fb");
			QCOMPARE(tree->blobs().count(), 2);
			QCOMPARE(tree->blobsByName().count(), 2);
		}
};

QTEST_KDEMAIN_CORE(TreeTest);

#include "TreeTest.moc"
