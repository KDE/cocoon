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

#ifndef TREE_H
#define TREE_H

#include "RawObject.h"



class TreeTest;

namespace Git {



class KDE_EXPORT Tree : public RawObject
{
	Q_OBJECT

	public:
		Tree(const QString& id, Repo &repo);

	protected:
		Tree(const QString& id, QObject *parent=0);

		friend class ::TreeTest;
};

}

#endif // TREE_H
