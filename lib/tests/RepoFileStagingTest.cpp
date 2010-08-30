/*
	Cocoon - A GUI for Git.
	Copyright (C) 2010  Riyad Preukschas <riyad@informatik.uni-bremen.de>

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



class RepoFileStagingTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			cloneFrom("RepoFileStagingTestRepo");
		}



		void shouldStageOneFile() {

		}

		void shouldStageMultipleFiles() {

		}

		void shouldStageDirectory() {

		}

		void shouldUnstageOneFile() {

		}

		void shouldUnstageMultipleFiles() {

		}

		void shouldUnstageDirectory() {

		}

		void shouldUnstageInEmptyRepo() {

		}
};

QTEST_KDEMAIN_CORE(RepoFileStagingTest);

#include "RepoFileStagingTest.moc"
