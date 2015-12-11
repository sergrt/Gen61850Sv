#pragma once
#include <QString>
#include <QStringList>

namespace SCPI {
	bool commandCompare(const QString& command, const QString& shortForm, const QString& longForm, bool getSuffix, QString *suffix, bool getArgument, QString *argument);
	bool scpiParse(const QStringList& command, const QString& shortForm, const QString& longForm, bool getSuffix, QString *suffix, bool getArgument, QString *argument, QStringList& commandSequence, int level);
}