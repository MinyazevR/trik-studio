/* Copyright 2016 CyberTech Labs Ltd.
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

#include <qrgui/controller/commands/abstractCommand.h>

#include "createRemoveSensorImplementation.h"

namespace twoDModel {
namespace commands {

class RemoveSensorCommand : public qReal::commands::AbstractCommand
{
	Q_OBJECT
public:
	RemoveSensorCommand(model::SensorsConfiguration &configurator
			, const QString &robotModel
			, const kitBase::robotModel::PortInfo &port);

private:
	bool execute() override;
	bool restoreState() override;

	CreateRemoveSensorImplementation mImpl;
};

}
}
