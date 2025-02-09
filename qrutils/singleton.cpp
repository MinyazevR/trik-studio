#include "singleton.h"

using namespace utils;

QHash<const void *, QWeakPointer<QObject>> Singleton::_typeToObject;
QMutex Singleton::_m;
