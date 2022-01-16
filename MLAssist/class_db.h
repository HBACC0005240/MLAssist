#ifndef CLASS_DB_H
#define CLASS_DB_H

#include "BattleBase.h"
#include <QVariant>
#include <QString>
#include <QMutex>




void _global_lock();
void _global_unlock();

struct _GlobalLock {

	_GlobalLock() { _global_lock(); }
	~_GlobalLock() { _global_unlock(); }
};
#define GLOBAL_LOCK_FUNCTION _GlobalLock _global_lock_;

class ClassDB {
public:
	enum APIType {
		API_CORE,
		API_EDITOR,
		API_NONE
	};

public:
	struct PropertySetGet {

		int index;
		QString setter;
		QString getter;
		/*MethodBind *_setptr;
		MethodBind *_getptr;*/
		QVariant type;
	};

	struct ClassInfo {

		APIType api;
		ClassInfo *inherits_ptr;
		void *class_ptr;
//		QHash<QString, MethodBind *> method_map;
		QHash<QString, int> constant_map;
		QHash<QString, QStringList > enum_map;
//		QHash<QString, MethodInfo> signal_map;

		QHash<QString, PropertySetGet> property_setget;

		QString inherits;
		QString name;
		bool disabled;
		bool exposed;
		QObject *(*creation_func)();
		ClassInfo();
		~ClassInfo();
	};

	template <class T>
	static QObject *creator() {
		return memnew(T);
	}

	static QMutex *lock;
	static QHash<QString, ClassInfo> classes;
	static QHash<QString, QString> resource_base_extensions;
	static QHash<QString, QString> compat_classes;


	static APIType current_api;

	static void _add_class2(const QString& p_class, const QString& p_inherits);

public:
	// DO NOT USE THIS!!!!!! NEEDS TO BE PUBLIC BUT DO NOT USE NO MATTER WHAT!!!
	template <class T>
	static void _add_class() {

		_add_class2(T::get_class_static(), T::get_parent_class_static());
	}

	template <class T>
	static void register_class() {

		GLOBAL_LOCK_FUNCTION;
		T::initialize_class();
		ClassInfo *t = classes.getptr(T::get_class_static());
		ERR_FAIL_COND(!t);
		t->creation_func = &creator<T>;
		t->exposed = true;
		t->class_ptr = T::get_class_ptr_static();
		T::register_custom_data_to_otdb();
	}

	
	template <class T>
	static QObject *_create_ptr_func() {

		return T::create();
	}
};


#define BIND_CONSTANT(m_constant) \
	ClassDB::bind_integer_constant(get_class_static(), StringName(), #m_constant, m_constant);

#define BIND_ENUM_CONSTANT(m_constant) \
	ClassDB::bind_integer_constant(get_class_static(), StringName(), #m_constant, m_constant);


#endif // CLASS_DB_H
