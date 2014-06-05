#pragma once

#include "waitForSensorBlock.h"

namespace interpreterBase {
namespace blocksBase {
namespace common {

/// An interpreter`s implementation for the collision sensor`s wait block.
class ROBOTS_INTERPRETER_BASE_EXPORT WaitForTouchSensorBlock : public WaitForSensorBlock
{
	Q_OBJECT

public:
	explicit WaitForTouchSensorBlock(interpreterBase::robotModel::RobotModelInterface &robotModel);
	~WaitForTouchSensorBlock() override;

protected slots:
	void responseSlot(int reading) override;

protected:
	interpreterBase::robotModel::DeviceInfo device() const override;
};

}
}
}
