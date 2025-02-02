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

#include <QtCore/QElapsedTimer>
#include <QtCore/QString>

#include "kernelDeclSpec.h"

namespace qReal {

/// Measures time interval between its creation and deletion, writes results to qDebug. Used for profiling.
class QRKERNEL_EXPORT TimeMeasurer
{
	Q_DISABLE_COPY_MOVE(TimeMeasurer)
public:
	/// Constructor.
	/// @param methodName A name of a method or part of program to be measured.
	explicit TimeMeasurer(const QString &methodName);

	/// Destructor. Calling it indicates end of measured interval.
	~TimeMeasurer();

	/// This method is used to avoid unused variables problem, because sometimes the functionality of the object of
	/// this class is limited to the functionality in destructor.
	void doNothing() const;

private:
	/// Timer to measure time interval.
	QElapsedTimer mTimer;

	/// Name of a method to measure.
	QString mMethodName;
};
}

/// Macro to conveniently log time it takes to exit current block.
#define LOG_TIME const qReal::TimeMeasurer measurer(Q_FUNC_INFO); measurer.doNothing();
