#include "SyncObject.h"
#include <QTest>

CSyncObject::CSyncObject()
    : mutex(), syncVal(SV_UNLOCKED), locker(NULL) {

}
bool CSyncObject::tryLock(QObject* obj) {
	bool res = false;
    QMutexLocker ml(&mutex);
    if (locker == obj && syncVal == SV_LOCKED) // already locket by obj
		res = true;

    if (syncVal != SV_LOCKED) {
		locker = obj;
		syncVal = SV_LOCKED;
	}

	return res;	
}
void CSyncObject::lock(QObject* obj) {
    while(true) {
        QMutexLocker ml(&mutex);
        if (locker == obj && syncVal == SV_LOCKED) // already locket by obj
            break;

        if (syncVal != SV_LOCKED) {
			locker = obj;
			syncVal = SV_LOCKED;
			break;
		}

		ml.unlock();
        QTest::qWait(20);
	}
}
void CSyncObject::unlock(QObject* obj) {
    if (obj != locker)
		throw( "Something bad happened" );

    QMutexLocker ml(&mutex);
	syncVal = SV_UNLOCKED;
	locker = NULL;
}
