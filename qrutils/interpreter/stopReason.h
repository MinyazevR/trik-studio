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

#pragma once
#include <QMetaType>
namespace qReal {
namespace interpretation {

/// A set of reasons why program execution stopped.
enum class StopReason
{
	/// Program finished correctly
	finished = 0
	/// Program finished incorrectly, error occured
	, error
	/// User stopped program execution (explicitly or implicitly)
	, userStop
};

}
}

Q_DECLARE_METATYPE(qReal::interpretation::StopReason)
