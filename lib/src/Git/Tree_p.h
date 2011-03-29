/*
	Cocoon - A GUI for Git.
	Copyright (C) 2009-2011  Riyad Preukschas <riyad@informatik.uni-bremen.de>

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

#ifndef TREE_P_H
#define TREE_P_H

#include "RawObject_p.h"

#include <QHash>



namespace Git {

class Tree;



class TreePrivate : public RawObjectPrivate {
public:
	TreePrivate()
		: RawObjectPrivate()
		, entries()
		, entryModes()
		, entryNames()
	{}
	TreePrivate(const RawObjectPrivate &other)
		: RawObjectPrivate(other)
		, entries()
		, entryModes()
		, entryNames()
	{}
	TreePrivate(const TreePrivate &other)
		: RawObjectPrivate(other)
		, entries(other.entries)
		, entryModes(other.entryModes)
		, entryNames(other.entryNames)
	{}
	~TreePrivate() {}

	QList<RawObject>   entries;
	QHash<Id, QString> entryModes;
	QHash<Id, QString> entryNames;
};

}

#endif // TREE_P_H
