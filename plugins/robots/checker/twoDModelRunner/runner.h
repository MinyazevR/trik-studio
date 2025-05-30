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

#include <QtCore/QScopedPointer>

#include <qrgui/systemFacade/systemFacade.h>
#include <qrgui/systemFacade/components/consoleErrorReporter.h>
#include <qrgui/systemFacade/components/nullMainWindow.h>
#include <qrgui/systemFacade/components/projectManager.h>
#include <textEditor/textManager.h>
#include <qrgui/controller/controller.h>
#include <qrgui/editor/sceneCustomizer.h>
#include <qrgui/plugins/toolPluginInterface/pluginConfigurator.h>
#include <interpreterCore/robotsPluginFacade.h>
#include "reporter.h"
#include <twoDModel/engine/view/twoDModelWidget.h>

namespace qReal {
namespace ui {
class ConsoleDock;
}
}

namespace twoDModel {

namespace model {
class RobotModel;
}

/// Creates instances null QReal environment, of robots plugin and runs interpretation on 2D model window.
class Runner : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param report A path to a file where JSON report about the session will be written after it ends.
	/// @param trajectory A path to a file where robot`s trajectory will be written during the session.
	/// @param input A path to a file where JSON with inputs for JavaScript.
	/// @param mode Interpret mode.
	/// @param qrsFile Path to TRIK Studio project
	Runner(const QString &report, const QString &trajectory, const QString &input,
	       const QString &mode, const QString &qrsFile);

	~Runner();

	/// Starts the interpretation process. The given save file will be opened and interpreted in 2D model window.
	/// @param saveFile QReal save file (qrs) that will be opened and interpreted.
	/// @param background If true then the save file will be interpreted in the fastest speed and 2D model window
	/// will be closed immediately after the interpretation stopped.
	/// @param speedFactor can be used when not in background mode to tune interpretation speed
	/// @param closeOnSuccessMode If true then model will be closed if the program finishes without errors.
	/// @param showConsole If true then robot's console will be showed.
	/// @param filePath If not QString() interpret code on this path instead of the code in the TRIK Studio file format.
	bool interpret(bool background, int speedFactor
				   , bool closeOnFinish, bool closeOnSuccess, bool showConsole, const QString &filePath);

	/// Generate code from TRIK Studio save file
	/// @param generatePath The path to save the generated code
	/// @param generateMode "python" or "javascript"
	bool generate(const QString &generatePath, const QString &generateMode);

private slots:
	void close();

private:
	void connectRobotModel(const model::RobotModel *robotModel, const qReal::ui::ConsoleDock* console);
	void onRobotRided(const QPointF &newPosition, const qreal newRotation);
	void onDeviceStateChanged(const QString &robotId, const kitBase::robotModel::robotParts::Device *device
			, const QString &property, const QVariant &value);
	void attachNewConsoleTo(view::TwoDModelWidget *twoDModelWindow);

	QScopedPointer<qReal::SystemFacade> mQRealFacade;
	QScopedPointer<qReal::Controller> mController;
	QScopedPointer<qReal::ConsoleErrorReporter> mErrorReporter;
	QScopedPointer<qReal::ProjectManager> mProjectManager;
	QScopedPointer<qReal::NullMainWindow> mMainWindow;
	QScopedPointer<qReal::text::TextManager> mTextManager;
	QScopedPointer<qReal::gui::editor::SceneCustomizer> mSceneCustomizer;
	QScopedPointer<qReal::PluginConfigurator> mConfigurator;
	QScopedPointer<Reporter> mReporter;
	QScopedPointer<interpreterCore::RobotsPluginFacade> mPluginFacade;
	QList<qReal::ui::ConsoleDock *> mRobotConsoles;
	QString mInputsFile;
	QString mMode;
	QString mSaveFile;
};

}
