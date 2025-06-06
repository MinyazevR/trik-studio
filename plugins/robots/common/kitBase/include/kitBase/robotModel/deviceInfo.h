/* Copyright 2007-2015 QReal Research Group
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#pragma once

#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>

#include <QReadWriteLock>

#include "direction.h"
#include "kitBase/kitBaseDeclSpec.h"

namespace kitBase {
namespace robotModel {

/// Describes a type of a device. Provides information about inheritance relation between devices by isA method
/// (for example, rangeSensor.isA<Sensor>()).
class ROBOTS_KIT_BASE_EXPORT DeviceInfo
{
public:
	/// Constructs invalid DeviceInfo instance.
	DeviceInfo();

	/// Creates a new instance of a Device descriptor. The resulting object will
	/// correspond to a given type only if Q_OBJECT macro is used inside its declaration.
	/// @warning Given device type must contain friendlyName() and direction() static functions
	/// and Q_OBJECT macro.
	template <typename T>
	static DeviceInfo &create()
	{
		// This line performs Q_OBJECT macro checking in the given type declaration.
		// Without Q_OBJECT macro incorrect metaObject will be passed and it will lead
		// to invalid isA() method work.
		static_assert(HasQObjectMacro<T>::Value, "No Q_OBJECT macro in the class that is passed into a template");
		const QMetaObject *metaObject = &T::staticMetaObject;
		const QString name = property(metaObject, "name");
		const QString friendlyName = property(metaObject, "friendlyName");
		const bool simulated = property(metaObject, "simulated") == "true";
		const Direction direction = property(metaObject, "direction").toLower() == "input" ? input : output;
		DeviceInfo result(metaObject, name, friendlyName, simulated, direction);
		QWriteLocker w(&mRWLock);
		auto &r = mCreatedInfos[QString(metaObject->className())] = std::move(result);
		return r;
	}

	/// Deserializes inner string representation obtained by toString().
	static DeviceInfo &fromString(const QString &string);

	/// Serializes given device info into inner string representation.
	QString toString() const;

	/// Returns if the device corresponding to 'this' inherits a 'parent' one or they are the devices of the same type.
	bool isA(const DeviceInfo &parent) const;

	/// Template shorthand notation for inheritance check.
	template<typename T>
	bool isA() const
	{
		return isA(create<T>());
	}

	/// Returns a string that is unique for each class of devices and that can be used,
	/// for example, by generators for searching corresponding templates.
	QString name() const;

	/// Returns a string that can be displayed to a user as the name of the device.
	QString friendlyName() const;

	/// Returns true if device should be simulated.
	bool simulated() const;

	/// Returns the direction of communication with devices of this type.
	Direction direction() const;

	/// Returns true if device is empty (instantiated with DeviceInfo() constructor).
	bool isNull() const;

	/// Cleanup resources
	static void release();

private:
	/// Trait that tells if the Object has a Q_OBJECT macro.
	template <typename Object>
	struct HasQObjectMacro
	{
		// Following two overloads will return int if qt_metacall function (generated by Q_OBJECT macro)
		// belongs to 'Object' type (and so Q_OBJECT macro is placed there) and char otherwise.
		template <typename T>
		static char returnIntIfHasQObjectMacro(int (T::*)(QMetaObject::Call, int, void **));
		static int returnIntIfHasQObjectMacro(int (Object::*)(QMetaObject::Call, int, void **));

		// Enum is used for forcing compiler evaluating below expression on compile-time
		enum { Value = sizeof(returnIntIfHasQObjectMacro(&Object::qt_metacall)) == sizeof(int) };
	};

	friend bool operator ==(const DeviceInfo &device1, const DeviceInfo &device2);
	friend bool operator !=(const DeviceInfo &device1, const DeviceInfo &device2);

	DeviceInfo(const QMetaObject *deviceType
			, const QString &name
			, const QString &friendlyName
			, bool isSimulated
			, Direction direction);

	static QString property(const QMetaObject * const metaObject, const QString &name);

	static QReadWriteLock mRWLock;
	static QMap<QString, DeviceInfo> mCreatedInfos;

	const QMetaObject *mDeviceType;
	QString mName;
	QString mFriendlyName;
	bool mSimulated {};
	Direction mDirection;
};

inline bool operator ==(const DeviceInfo &device1, const DeviceInfo &device2)
{
	if (!device1.mDeviceType || !device2.mDeviceType) {
		return device1.mDeviceType == device2.mDeviceType;
	}

	return QString(device1.mDeviceType->className()) == QString(device2.mDeviceType->className());
}

inline bool operator !=(const DeviceInfo &device1, const DeviceInfo &device2)
{
	return !(device1 == device2);
}

}
}

Q_DECLARE_METATYPE(kitBase::robotModel::DeviceInfo)
