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

#include <QtXml/QDomElement>

#include "twoDModel/engine/model/settings.h"
#include "twoDModel/engine/model/constants.h"
#include <qrkernel/settingsManager.h>

using namespace twoDModel::model;

bool Settings::realisticPhysics() const
{
	return mRealisticPhysics;
}

bool Settings::realisticSensors() const
{
	return mRealisticSensors;
}

bool Settings::realisticMotors() const
{
	return mRealisticMotors;
}

qreal Settings::pixelsInCm() const
{
	return mPixelsInCm;
}

void Settings::serialize(QDomElement &parent) const
{
	auto result = parent.ownerDocument().createElement("settings");
	parent.appendChild(result);
	result.setAttribute("realisticPhysics", mRealisticPhysics ? "true" : "false");
	result.setAttribute("realisticSensors", mRealisticSensors ? "true" : "false");
	result.setAttribute("realisticMotors", mRealisticMotors ? "true" : "false");
}

void Settings::deserialize(const QDomElement &parent)
{
	mRealisticPhysics = parent.attribute("realisticPhysics") == "true";
	mRealisticSensors = parent.attribute("realisticSensors") == "true";
	mRealisticMotors = parent.attribute("realisticMotors") == "true";
	const auto pixelsInCmStr = parent.attribute("pixelsInCm", "");
	if (!pixelsInCmStr.isEmpty()) {
		mPixelsInCm = pixelsInCmStr.toDouble();
	}
	emit pixelInCmChanged(mPixelsInCm);
	emit physicsChanged(mRealisticPhysics);
}

void Settings::setRealisticPhysics(bool set)
{
	mRealisticPhysics = set;
	emit physicsChanged(mRealisticPhysics);
}

void Settings::setPixelsInCm(const qreal pixelsInCm)
{
	mPixelsInCm = pixelsInCm;
	Q_EMIT pixelInCmChanged(mPixelsInCm);
}

void Settings::setRealisticSensors(bool set)
{
	mRealisticSensors = set;
}

void Settings::setRealisticMotors(bool set)
{
	mRealisticMotors = set;
}
