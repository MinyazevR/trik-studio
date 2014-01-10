#pragma once

#include "interpreterBase/robotModel/robotModelInterface.h"
#include "interpreterBase/interpreterBaseDeclSpec.h"

namespace interpreterBase {
namespace robotModel {

class ROBOTS_INTERPRETER_BASE_EXPORT CommonRobotModel : public RobotModelInterface {
	Q_OBJECT

public:
	CommonRobotModel();
	virtual ~CommonRobotModel();

	void init() override;
	void stopRobot() override;
	void disconnectFromRobot() override;

	bool needsConnection() const override;

	ConfigurationInterface &configuration() override;  // TODO: Don't like this, breaks incapsulation.

	robotParts::Brick &brick() override;
	robotParts::Display &display() override;
};

}
}
