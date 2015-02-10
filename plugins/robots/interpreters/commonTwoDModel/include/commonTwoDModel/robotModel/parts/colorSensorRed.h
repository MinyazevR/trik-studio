#pragma once

#include <interpreterBase/robotModel/robotParts/colorSensorRed.h>

#include "commonTwoDModel/commonTwoDModelDeclSpec.h"

namespace twoDModel {

namespace engine {
class TwoDModelEngineInterface;
}

namespace robotModel {
namespace parts {

/// An implementation of the color sensor in the red color mode for the 2D model window.
/// Returns a red color percent in the picture under the sensor: a value from 0 to 100.
/// Configuration is perfomed immediately, the answer is ready immediately too.
class COMMON_TWO_D_MODEL_EXPORT ColorSensorRed : public interpreterBase::robotModel::robotParts::ColorSensorRed
{
	Q_OBJECT
	Q_CLASSINFO("friendlyName", tr("Color sensor (red)"))

public:
	ColorSensorRed(const interpreterBase::robotModel::DeviceInfo &info
			, const interpreterBase::robotModel::PortInfo &port
			, engine::TwoDModelEngineInterface &engine);

	void read();

private:
	engine::TwoDModelEngineInterface &mEngine;
};

}
}
}
