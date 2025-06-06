/* Copyright 2015 CyberTech Labs Ltd.
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

#include "trikKitInterpreterCommon/robotModel/twoD/trikTwoDRobotModel.h"

#include <QtGui/QColor>
#include <QtCore/QFile>

#include <qrkernel/logging.h>
#include <qrkernel/platformInfo.h>
#include <qrkernel/settingsManager.h>

#include <kitBase/robotModel/robotModelUtils.h>
#include <kitBase/robotModel/robotParts/lightSensor.h>
#include <kitBase/robotModel/robotParts/touchSensor.h>
#include <kitBase/robotModel/robotParts/gyroscopeSensor.h>
#include <kitBase/robotModel/robotParts/lidarSensor.h>

#include <trikKit/robotModel/parts/trikVideoCamera.h>
#include <trikKit/robotModel/parts/trikLineSensor.h>
#include <trikKit/robotModel/parts/trikObjectSensor.h>
#include <trikKit/robotModel/parts/trikColorSensor.h>
#include <trikKit/robotModel/parts/trikInfraredSensor.h>
#include <trikKit/robotModel/parts/trikSonarSensor.h>
#include <trikKit/robotModel/parts/trikShell.h>
#include <trikKitInterpreterCommon/trikDisplayWidget.h>

#include "trikKitInterpreterCommon/robotModel/twoD/parts/twoDDisplay.h"
#include "trikKitInterpreterCommon/robotModel/twoD/parts/twoDSpeaker.h"
#include "trikKitInterpreterCommon/robotModel/twoD/parts/twoDShell.h"
#include "trikKitInterpreterCommon/robotModel/twoD/parts/twoDLed.h"
#include "trikKitInterpreterCommon/robotModel/twoD/parts/twoDLineSensor.h"
#include "trikKitInterpreterCommon/robotModel/twoD/parts/twoDObjectSensor.h"
#include "trikKitInterpreterCommon/robotModel/twoD/parts/twoDColorSensor.h"
#include "trikKitInterpreterCommon/robotModel/twoD/parts/twoDLightSensor.h"
#include "trikKitInterpreterCommon/robotModel/twoD/parts/twoDGyroscopeSensor.h"
#include "trikKitInterpreterCommon/robotModel/twoD/parts/twoDNetworkCommunicator.h"
using namespace trik::robotModel;
using namespace trik::robotModel::twoD;
using namespace kitBase::robotModel;

TrikTwoDRobotModel::TrikTwoDRobotModel(RobotModelInterface &realModel)
	: twoDModel::robotModel::TwoDRobotModel(realModel)
	, mLeftWheelPort("M3")
	, mRightWheelPort("M4")
	, mDisplayWidget(new TrikDisplayWidget())
	, mCollidingPolygon({QPointF(1, 10), QPointF(47, 10), QPointF(49, 20)
			, QPointF(49, 30), QPointF(47, 40), QPointF(1, 40)})
{
	/// @todo: One day we will support gamepad in 2D model and there will be piece.
	/// But till that day gamepad ports must be killed cause they spam logs.
	const QList<PortInfo> realRobotPorts = CommonRobotModel::availablePorts();
	for (const PortInfo &port : realRobotPorts) {
		if (port.name().contains("Gamepad", Qt::CaseInsensitive)) {
			removeAllowedConnections(port);
		}
	}
}

QPair<qreal, int> TrikTwoDRobotModel::rangeSensorAngleAndDistance
		(const kitBase::robotModel::DeviceInfo &deviceType) const
{
	return deviceType.isA<robotModel::parts::TrikInfraredSensor>() ? QPair<qreal, int>(5, 80) :
			deviceType.isA<robotModel::parts::TrikSonarSensor>() ? QPair<qreal, int>(20, 300) :
			deviceType.isA<robotParts::LidarSensor>() ? QPair<qreal,int>(360, 400) :
			TwoDRobotModel::rangeSensorAngleAndDistance(deviceType);
}

robotParts::Device *TrikTwoDRobotModel::createDevice(const PortInfo &port, const DeviceInfo &deviceInfo)
{
	if (deviceInfo.isA<robotParts::Communicator>()) {
		return new parts::TwoDNetworkCommunicator(deviceInfo, port, mMailbox);
	}


	if (deviceInfo.isA<robotParts::Display>()) {
		return new parts::Display(deviceInfo, port, *engine());
	}

	if (deviceInfo.isA<robotParts::Speaker>()) {
		return new parts::TwoDSpeaker(deviceInfo, port, *engine());
	}

	if (deviceInfo.isA<robotParts::LightSensor>()) {
		return new parts::TwoDLightSensor(deviceInfo, port, *engine());
	}

	if (deviceInfo.isA<robotModel::parts::TrikShell>()) {
		parts::Shell * const shell = new parts::Shell(deviceInfo, port, *engine());
		// Error reporter will come only after global plugin init() is called. Shell is however
		// configured even later. So setting error reporter only when everything will be ready.
		connect(shell, &parts::Shell::configured, this, [=]() { shell->setErrorReporter(*mErrorReporter); });
		return shell;
	}

	if (deviceInfo.isA<robotModel::parts::TrikLed>()) {
		return new parts::TwoDLed(deviceInfo, port, *engine());
	}

	if (deviceInfo.isA<robotModel::parts::TrikLineSensor>()) {
		return new parts::LineSensor(deviceInfo, port, *engine());
	}

	if (deviceInfo.isA<robotModel::parts::TrikObjectSensor>()) {
		return new parts::ObjectSensor(deviceInfo, port);
	}

	if (deviceInfo.isA<robotModel::parts::TrikColorSensor>()) {
		return new parts::ColorSensor(deviceInfo, port, *engine());
	}

	if (deviceInfo.isA<kitBase::robotModel::robotParts::GyroscopeSensor>()) {
		return new parts::GyroscopeSensor(deviceInfo, port, *engine());
	}

	return twoDModel::robotModel::TwoDRobotModel::createDevice(port, deviceInfo);
}

void TrikTwoDRobotModel::onInterpretationStarted()
{
	robotModel::parts::TrikDisplay * const display =
			RobotModelUtils::findDevice<robotModel::parts::TrikDisplay>(*this, "DisplayPort");
	if (display) {
		display->clearScreen();
		display->setBackground(QColor(Qt::gray));
		display->redraw();
	} else {
		QLOG_WARN() << "TRIK display is not configured before intepretation start!";
	}

	parts::Shell * const shell = RobotModelUtils::findDevice<parts::Shell>(*this, "ShellPort");
	if (shell) {
		shell->reset();
	} else {
		QLOG_WARN() << "TRIK shell is not configured before intepretation start!";
	}

	auto * const mailbox = RobotModelUtils::findDevice<parts::TwoDNetworkCommunicator>(*this, "CommunicatorPort");
	if (mailbox) {
		mailbox->release();
	} else {
		QLOG_WARN() << "TRIK network is not configured before intepretation start!";
	}
}

QString TrikTwoDRobotModel::robotImage() const
{
	const QString key = "trikRobot2DImage";
	const QString hackDefaultPath = "./images/trik-robot.svg";
	if (qReal::SettingsManager::value(key).isNull()) {
		qReal::SettingsManager::setValue(key, hackDefaultPath);
	}

	const QString settingsPath = qReal::PlatformInfo::invariantSettingsPath(key);
	return QFile::exists(settingsPath) ? settingsPath : ":icons/trik-robot.svg";
}

PortInfo TrikTwoDRobotModel::defaultLeftWheelPort() const
{
	return PortInfo(mLeftWheelPort, output);
}

PortInfo TrikTwoDRobotModel::defaultRightWheelPort() const
{
	return PortInfo(mRightWheelPort, output);
}

twoDModel::engine::TwoDModelDisplayWidget *TrikTwoDRobotModel::displayWidget() const
{
	return mDisplayWidget;
}

QString TrikTwoDRobotModel::sensorImagePath(const DeviceInfo &deviceType) const
{
	if (deviceType.isA<kitBase::robotModel::robotParts::LightSensor>()) {
		return ":icons/twoDColorEmpty.svg";
	} else if (deviceType.isA<robotModel::parts::TrikInfraredSensor>()) {
		return ":icons/twoDIrRangeSensor.svg";
	} else if (deviceType.isA<robotModel::parts::TrikSonarSensor>()) {
		return ":icons/twoDUsRangeSensor.svg";
	} else if (deviceType.isA<robotModel::parts::TrikVideoCamera>()) {
		return ":icons/twoDVideoModule.svg";
	} else if (deviceType.isA<robotParts::LidarSensor>()) {
		return ":icons/twoDIrRangeSensor.svg";
	}

	return QString();
}

void TrikTwoDRobotModel::setWheelPorts(const QString &leftWheelPort, const QString &rightWheelPort)
{
	mLeftWheelPort = leftWheelPort;
	mRightWheelPort = rightWheelPort;
}

QRect TrikTwoDRobotModel::sensorImageRect(const kitBase::robotModel::DeviceInfo &deviceType) const
{
	if (deviceType.isA<robotParts::TouchSensor>()) {
			return QRect(-12, -5, 25, 10);
	} else if (deviceType.isA<robotParts::LightSensor>()) {
		return QRect(-6, -6, 12, 12);
	} else if (deviceType.isA<robotModel::parts::TrikInfraredSensor>()) {
		return QRect(-18, -18, 36, 36);
	} else if (deviceType.isA<robotModel::parts::TrikSonarSensor>()) {
		return QRect(-18, -18, 36, 36);
	} else if (deviceType.isA<robotModel::parts::TrikVideoCamera>()) {
		return QRect(-9, -9, 18, 18);
	} else if (deviceType.isA<robotParts::LidarSensor>()) {
		return QRect(-18, -18, 36, 36);
	}

	return QRect();
}

QHash<kitBase::robotModel::PortInfo, kitBase::robotModel::DeviceInfo> TrikTwoDRobotModel::specialDevices() const
{
	QHash<PortInfo, DeviceInfo> result(twoDModel::robotModel::TwoDRobotModel::specialDevices());
	return result;
}

QPair<QPoint, qreal> TrikTwoDRobotModel::specialDeviceConfiguration(const PortInfo &port) const
{
	return twoDModel::robotModel::TwoDRobotModel::specialDeviceConfiguration(port);
}

QPolygonF TrikTwoDRobotModel::collidingPolygon() const
{
	return mCollidingPolygon;
}

qreal TrikTwoDRobotModel::mass() const
{
	return 1.05;
}

qreal TrikTwoDRobotModel::friction() const
{
	return 0.3;  /// @todo measure it
}

qreal TrikTwoDRobotModel::onePercentAngularVelocity() const
{
	return 0.0055;
}

QList<QPointF> TrikTwoDRobotModel::wheelsPosition() const
{
	return {QPointF(10, 3), QPointF(10, 47)};
}

QHash<QString, int> TrikTwoDRobotModel::buttonCodes() const
{
	QHash<QString, int> result;
	result["LeftButton"] = 105;
	result["RightButton"] = 106;
	result["UpButton"] = 103;
	result["DownButton"] = 108;
	result["EnterButton"] = 28;
	result["PowerButton"] = 116;
	result["EscButton"] = 1;
	return result;
}

void TrikTwoDRobotModel::setErrorReporter(qReal::ErrorReporterInterface &errorReporter)
{
	mErrorReporter = &errorReporter;
}

void TrikTwoDRobotModel::setMailbox(trikNetwork::MailboxInterface &mailbox)
{
	mMailbox = &mailbox;
}
