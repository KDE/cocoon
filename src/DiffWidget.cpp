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

#include "DiffWidget.h"

DiffWidget::DiffWidget(QWidget *parent)
	: KTextBrowser(parent)
{
	setLineWrapMode(QTextEdit::NoWrap);
}

const QString DiffWidget::format(const QString &unformattedDiff) const
{
	QStringList lines = Qt::escape(unformattedDiff).split("\n");

	for(int i=0; i < lines.size(); ++i) {
		lines[i] = lines[i].replace(QRegExp("^(diff --git .*)"),   "<span class=\"header\">\\1</span>");
		lines[i] = lines[i].replace(QRegExp("^(index .*)"),        "<span class=\"header info\">\\1</span>");
		lines[i] = lines[i].replace(QRegExp("^(new file .*)"),     "<span class=\"header add\">\\1</span>");
		lines[i] = lines[i].replace(QRegExp("^(deleted file .*)"), "<span class=\"header remove\">\\1</span>");
		lines[i] = lines[i].replace(QRegExp("^(\\+\\+\\+ .*)"),    "<span class=\"header add\">\\1</span>");
		lines[i] = lines[i].replace(QRegExp("^(--- .*)"),          "<span class=\"header remove\">\\1</span>");
		lines[i] = lines[i].replace(QRegExp("^(\\+.*)"),           "<span class=\"add\">\\1</span>");
		lines[i] = lines[i].replace(QRegExp("^(-.*)"),             "<span class=\"remove\">\\1</span>");
		lines[i] = lines[i].replace(QRegExp("^(@@.*)"),            "<span class=\"info\">\\1</span>");
		lines[i] = lines[i].replace(QRegExp("^(\\.*)"),            "<span class=\"warning\">\\1</span>");
	}

	QString fullyFormattedDiff =
	"<?xml version=\"1.0\" ?>\n"
	"<html>\n"
	"<head>\n"
	"	 <style type=\"text/css\">\n"
	"	   pre {\n"
	"      font-family: 'Droid Sans Mono';\n"
	"    }\n"
	"    div { display: inline; }\n"
	"    .add { color: #008C00; }\n"
	"    .header { background-color: #EEEEEE; }\n"
	"    .info { color: #0057AE; }\n"
	"    .remove { color: #BF0303; }\n"
	"    .warning { color: #FFAA00; }\n"
	"  </style>\n"
	"</head>\n"
	"<body>\n"
	"<pre>%1</pre>\n"
	"</body>\n"
	"</html>\n";

	return fullyFormattedDiff.arg(lines.join("\n"));
}

void DiffWidget::setDiff(const QString &diffString)
{
	setHtml(format(diffString));
}
