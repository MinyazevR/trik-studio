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

#include "printTextBlock.h"

#include <kitBase/robotModel/robotModelUtils.h>
#include <kitBase/robotModel/robotParts/display.h>
#include <QRegularExpression>

using namespace interpreterCore::coreBlocks::details;

PrintTextBlock::PrintTextBlock(kitBase::robotModel::RobotModelInterface &robotModel)
	: kitBase::blocksBase::common::DisplayBlock(robotModel)
{
}

void PrintTextBlock::doJob(kitBase::robotModel::robotParts::Display &display)
{
	const int x = eval<int>("XCoordinateText");
	const int y = eval<int>("YCoordinateText");

	QString result = stringProperty("PrintText");
	if (boolProperty("Evaluate")) {
	     result = eval<QString>("PrintText");
	     bool ok;
	     auto doubleResult = result.toDouble(&ok);
	     if (ok) {
		     result = QString::number(doubleResult, 'f', 6).remove(QRegularExpression("\\.?0+$"));
	     }
	}

	const bool redraw = boolProperty("Redraw");
	if (!errorsOccured()) {
		display.printText(x, y, result);
		if (redraw) {
			display.redraw();
		}

		emit done(mNextBlockId);
	}
}
