#include "scpi.h"

bool SCPI::commandCompare(const QString& command, const QString& shortForm, const QString& longForm, bool getSuffix, QString *suffix, bool getArgument, QString *argument) {
	bool res = true;

	if (getSuffix) {
		QString cmdCompareTo = command.left(command.indexOf(' '));

		bool longFormUsed = false;
		if (cmdCompareTo.left(longForm.length()).compare(longForm, Qt::CaseInsensitive) == 0)
			longFormUsed = true;
		else if (cmdCompareTo.left(shortForm.length()).compare(shortForm, Qt::CaseInsensitive) == 0)
			longFormUsed = false;
		else 
			res = false;

		if (res) {	
			if (getArgument && argument)
				*argument = command.right(command.length() - command.lastIndexOf(' ') - 1);

			if (suffix) {
				if ((longFormUsed && longForm.length() == cmdCompareTo.length()) ||
					(!longFormUsed && shortForm.length() == cmdCompareTo.length()))
					*suffix = "1"; //Значение по умолчанию, если не указано
				else {
					if (longFormUsed)						
						*suffix = cmdCompareTo.right(cmdCompareTo.length() - longForm.length());
					else
						*suffix = cmdCompareTo.right(cmdCompareTo.length() - shortForm.length());
					//res = convertOk;
				}
			}				
		}
	} else {
		QString commandToCheck = command;

		if (getArgument) {
			if (argument)
				*argument = command.right(command.length() - command.lastIndexOf(' ') - 1);

			commandToCheck = command.left(command.indexOf(' '));			
		}

		if (commandToCheck.compare(shortForm, Qt::CaseInsensitive) == 0 || commandToCheck.compare(longForm, Qt::CaseInsensitive) == 0) {
			// nothing to do
		} else {
			res = false;
		}
	}

	return res;
}

bool SCPI::scpiParse(const QStringList& command, const QString& shortForm, const QString& longForm, bool getSuffix, QString *suffix, bool getArgument, QString *argument, QStringList& commandSequence, int level) {
	bool res = false;
	if (command.size() > level) {
		if (commandCompare(command[level], shortForm, longForm, getSuffix, suffix, getArgument, argument)) {
			if (!getArgument) { //Не последняя ветка дерева команд
				if (commandSequence.size() < level + 1)
					commandSequence.push_back(command[level]);
				else	
					commandSequence[level] = command[level];
			}
			res = true;
		}
	}
	return res;
}