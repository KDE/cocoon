/*
	Cocoon - A GUI for Git.
	Copyright (C) 2009-2012  Riyad Preukschas <riyad@informatik.uni-bremen.de>

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
		lines[i] = lines[i].replace(QRegExp("^(diff --git .*)"),   "<div class=\"header\">\\1</div>");
		lines[i] = lines[i].replace(QRegExp("^(index .*)"),        "<div class=\"header info\">\\1</div>");
		lines[i] = lines[i].replace(QRegExp("^(new file .*)"),     "<div class=\"header add\">\\1</div>");
		lines[i] = lines[i].replace(QRegExp("^(deleted file .*)"), "<div class=\"header remove\">\\1</div>");
		lines[i] = lines[i].replace(QRegExp("^(\\+\\+\\+ .*)"),    "<div class=\"header add\">\\1</div>");
		lines[i] = lines[i].replace(QRegExp("^(--- .*)"),          "<div class=\"header remove\">\\1</div>");
		lines[i] = lines[i].replace(QRegExp("^(\\+.*)"),           "<div class=\"add\">\\1</div>");
		lines[i] = lines[i].replace(QRegExp("^(-.*)"),             "<div class=\"remove\">\\1</div>");
		lines[i] = lines[i].replace(QRegExp("^(@@.*)"),            "<div class=\"info\">\\1</div>");
		lines[i] = lines[i].replace(QRegExp("^(\\.*)"),            "<div class=\"warning\">\\1</div>");
	}

	QString fullyFormattedDiff =
	"<?xml version=\"1.0\" ?>\n"
	"<html>\n"
	"<head>\n"
	"  <style type=\"text/css\">\n"
	"    pre {\n"
	"      font-family: 'Droid Sans Mono';\n"
	"    }\n"
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
