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

#include <QtCore/QStringList>

#include <qrutils/mathUtils/math.h>
#include <qrutils/mathUtils/geometry.h>

#include "twoDModel/engine/model/sensorsConfiguration.h"
#include "kitBase/robotModel/robotParts/lidarSensor.h"
#include "twoDModel/engine/model/metricCoordinateSystem.h"

using namespace twoDModel::model;
using namespace kitBase::robotModel;

SensorsConfiguration::SensorsConfiguration(twoDModel::model::MetricCoordinateSystem &metricSystem,
			const QString &robotModelName, const QSizeF &robotSize)
	: mRobotSize(robotSize)
	, mRobotId(robotModelName)
	, mMetricSystem(metricSystem)
{
}

void SensorsConfiguration::clear() {
	clearConfiguration(Reason::loading);
}

void SensorsConfiguration::onDeviceConfigurationChanged(const QString &robotId
		, const PortInfo &port, const DeviceInfo &device, Reason reason)
{
	if (robotId != mRobotId) {
		// Ignoring external events
		return;
	}

	if (device.isNull()) {
		mSensorsInfo[port] = SensorInfo();
		emit deviceRemoved(port, reason == Reason::loading);
		return;
	}

	// If there was no sensor before then placing it right in front of the robot;
	// else putting it instead of old one.
	mSensorsInfo[port] = mSensorsInfo[port].isNull ? SensorInfo(defaultPosition(device), 0) : mSensorsInfo[port];

	emit deviceAdded(port, reason == Reason::loading);
}

QPointF SensorsConfiguration::defaultPosition(const DeviceInfo &device) const
{
	/// @todo: Move it somewhere?
	return  !device.simulated() || device.isA<kitBase::robotModel::robotParts::LidarSensor>()
			? QPointF(mRobotSize.width() / 2, mRobotSize.height() / 2)
			: QPointF(mRobotSize.width() * 3 / 2, mRobotSize.height() / 2);
}

QPointF SensorsConfiguration::position(const PortInfo &port) const
{
	return mSensorsInfo[port].position;
}

void SensorsConfiguration::onSizeUpdated(const QSizeF size)
{
	mOldRobotSize = mRobotSize;
	mRobotSize = size;
}

void SensorsConfiguration::updateAllSensorPosition()
{
	for (auto &&sensorInfo: mSensorsInfo.values()) {
		// relative "left-up" corner of robot
		auto currentPosition = sensorInfo.position;
		auto currentWidth = currentPosition.x();
		auto currentHeight = currentPosition.y();
		auto widthDiff = mRobotSize.width() - mOldRobotSize.width();
		auto heightDiff = mRobotSize.height() - mOldRobotSize.height();
		sensorInfo.position = QPointF {
			currentWidth + widthDiff,
			currentHeight + heightDiff
		};
	}
}

void SensorsConfiguration::setPosition(const PortInfo &port, const QPointF &position)
{
	if (!mathUtils::Geometry::eq(mSensorsInfo[port].position, position)) {
		mSensorsInfo[port].position = position;
		emit positionChanged(port);
	}
}

qreal SensorsConfiguration::direction(const PortInfo &port) const
{
	return mSensorsInfo[port].direction;
}

void SensorsConfiguration::setDirection(const PortInfo &port, qreal direction)
{
	if (!mathUtils::Math::eq(mSensorsInfo[port].direction, direction)) {
		mSensorsInfo[port].direction = direction;
		emit rotationChanged(port);
	}
}

DeviceInfo SensorsConfiguration::type(const PortInfo &port) const
{
	return currentConfiguration(mRobotId, port);
}

void SensorsConfiguration::serialize(QDomElement &robot) const
{
	QDomElement sensorsElem = robot.ownerDocument().createElement("sensors");
	robot.appendChild(sensorsElem);

	for (const PortInfo &port: mSensorsInfo.keys()) {
		const DeviceInfo device = currentConfiguration(mRobotId, port);
		const SensorInfo sensor = mSensorsInfo.value(port);
		QDomElement sensorElem = robot.ownerDocument().createElement("sensor");
		sensorsElem.appendChild(sensorElem);
		sensorElem.setAttribute("port", port.toString());
		sensorElem.setAttribute("type", device.toString());

		sensorElem.setAttribute("position"
		                , QString::number(
		                        mMetricSystem.toUnit(sensor.position.x()))
		                        + ":" + QString::number(mMetricSystem.toUnit(sensor.position.y())));

		sensorElem.setAttribute("direction", QString::number(sensor.direction));
	}
}

void SensorsConfiguration::deserialize(const QDomElement &element)
{
	if (element.isNull()) {
		/// @todo Report error
		return;
	}

	mSensorsInfo.clear();

	QDomNodeList sensors = element.elementsByTagName("sensor");
	for (int i = 0; i < sensors.count(); ++i) {
		const QDomElement sensorNode = sensors.at(i).toElement();

		const PortInfo port = PortInfo::fromString(sensorNode.attribute("port"));

		const DeviceInfo &type = DeviceInfo::fromString(sensorNode.attribute("type"));

		const QString positionStr = sensorNode.attribute("position", "0:0");
		const QStringList splittedStr = positionStr.split(":");
		const qreal x = static_cast<qreal>(splittedStr[0].toDouble());
		const qreal y = static_cast<qreal>(splittedStr[1].toDouble());
		const QPointF position = mMetricSystem.toPx({x, y});

		const qreal direction = sensorNode.attribute("direction", "0").toDouble();

		deviceConfigurationChanged(mRobotId, port, DeviceInfo(), Reason::loading);
		deviceConfigurationChanged(mRobotId, port, type, Reason::loading);
		setPosition(port, position);
		setDirection(port, direction);
	}
}

SensorsConfiguration::SensorInfo::SensorInfo()
	: direction(0)
	, isNull(true)
{
}

SensorsConfiguration::SensorInfo::SensorInfo(const QPointF &position,
					     qreal direction)
	: position(position)
	, direction(direction)
	, isNull(false)
{
}
