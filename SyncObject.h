// "Monitor" pattern - http://stackoverflow.com/questions/8971168/how-to-use-qmutex
#pragma once
#include <QMutexLocker>
#include <QObject>

class CSyncObject
{
public:
	CSyncObject();
	//bool isLocked();
    bool tryLock(QObject* obj);
    void lock(QObject* obj);
    void unlock(QObject* obj);

private:
    enum LOCK_STATE {
		SV_UNLOCKED = 0,
		SV_LOCKED = 1
	};

	QMutex mutex;
	LOCK_STATE syncVal;
	QObject * locker;
    Q_DISABLE_COPY(CSyncObject)
};
