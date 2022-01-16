#include "class_db.h"


QMutex* _global_mutex = new QMutex;// NULL;

void _global_lock() {

	if (_global_mutex)
		_global_mutex->lock();
}
void _global_unlock() {

	if (_global_mutex)
		_global_mutex->unlock();
}

#define OBJTYPE_RLOCK RWLockRead _rw_lockr_(lock);
#define OBJTYPE_WLOCK RWLockWrite _rw_lockw_(lock);


QHash<QString, ClassDB::ClassInfo> ClassDB::classes;
QHash<QString, QString> ClassDB::resource_base_extensions;
QHash<QString, QString> ClassDB::compat_classes;

ClassDB::ClassInfo::ClassInfo() {

	api = API_NONE;
	creation_func = NULL;
	inherits_ptr = NULL;
	disabled = false;
	exposed = false;
}

ClassDB::ClassInfo::~ClassInfo() {
}
