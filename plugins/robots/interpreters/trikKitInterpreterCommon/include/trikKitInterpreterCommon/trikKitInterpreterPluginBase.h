/* Copyright 2013-2016 CyberTech Labs Ltd.
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
#include <QtCore/QSharedPointer>

#include <kitBase/kitPluginInterface.h>
#include <twoDModel/robotModel/twoDRobotModel.h>
#include <twoDModel/engine/twoDModelControlInterface.h>

#include <trikKit/blocks/trikBlocksFactoryBase.h>
#include <trikKit/robotModel/trikRobotModelBase.h>

#include "robotModel/twoD/trikTwoDRobotModel.h"
#include "trikAdditionalPreferences.h"
#include <trikNetwork/mailboxInterface.h>
#include <trikKitInterpreterCommon/trikTextualInterpreter.h>

/// @todo: refactor
#include <qrgui/plugins/toolPluginInterface/usedInterfaces/mainWindowInterpretersInterface.h>

#include "declSpec.h"

namespace trik {

class ROBOTS_TRIK_KIT_INTERPRETER_COMMON_EXPORT TrikKitInterpreterPluginBase
		: public QObject, public kitBase::KitPluginInterface, /*for now*/ public kitBase::DevicesConfigurationProvider
{
	Q_OBJECT
	Q_INTERFACES(kitBase::KitPluginInterface)

public:
	TrikKitInterpreterPluginBase();
	~TrikKitInterpreterPluginBase() override;

	void init(const kitBase::KitPluginConfigurator &configurer) override;
	void release() override;

	QList<kitBase::robotModel::RobotModelInterface *> robotModels() override;

	kitBase::robotModel::RobotModelInterface *defaultRobotModel() override;

	QSharedPointer<kitBase::blocksBase::BlocksFactoryInterface> blocksFactoryFor(
			const kitBase::robotModel::RobotModelInterface *model) override;
	QList<kitBase::AdditionalPreferences *> settingsWidgets() override;

	QWidget *quickPreferencesFor(const kitBase::robotModel::RobotModelInterface &model) override;

	QString defaultSettingsFile() const override;

	QIcon iconForFastSelector(const kitBase::robotModel::RobotModelInterface &robotModel) const override;

	kitBase::DevicesConfigurationProvider * devicesConfigurationProvider() override;

	QList<qReal::ActionInfo> customActions() override;

	QList<qReal::HotKeyActionInfo> hotKeyActions() override;

	TrikTextualInterpreter *textualInterpreter() const;

signals:
	void started();
	void stopped(qReal::interpretation::StopReason reason);
	void codeInterpretationStarted(const QString &code, const QString &languageExtension);

private slots:
	QWidget *produceIpAddressConfigurer();  // Transfers ownership

	void testStart(); // QtS
	void testStop(qReal::interpretation::StopReason reason);
	void onTabChanged(const qReal::TabInfo &info);

protected:
	/// Takes ownership over all supplied pointers.
	void initKitInterpreterPluginBase(robotModel::TrikRobotModelBase * const realRobotModel
			, robotModel::twoD::TrikTwoDRobotModel * const twoDRobotModel
			, const QSharedPointer<blocks::TrikBlocksFactoryBase> &blocksFactory
			);

	qReal::gui::MainWindowInterpretersInterface *mMainWindow {};

private:
	void startCodeInterpretation(const QString &code, const QString &extension);
	void startCodeInterpretation(const QString &code, const QString &inputs, const QString &extension);

	/// Handles all file workings  with camera imitation process
	void handleImitationCameraWork();

	QScopedPointer<twoDModel::TwoDModelControlInterface> mTwoDModel;
	QScopedPointer<robotModel::TrikRobotModelBase> mRealRobotModel;
	QSharedPointer<robotModel::twoD::TrikTwoDRobotModel> mTwoDRobotModel;

	QScopedPointer<TrikTextualInterpreter> mTextualInterpreter;
	QScopedPointer<trikNetwork::MailboxInterface> mMailbox;

	QAction mStart;
	QAction mStop;

	bool mIsModelSelected = false;

	qReal::SystemEvents *mSystemEvents = nullptr; // Does not have ownership

	QSharedPointer<kitBase::blocksBase::BlocksFactoryInterface> mBlocksFactory;

	/// Ownership depends on mOwnsAdditionalPreferences flag.
	TrikAdditionalPreferences *mAdditionalPreferences = nullptr;
	bool mOwnsAdditionalPreferences = true;

	kitBase::InterpreterControlInterface *mInterpreterControl {};  // Does not have ownership.
	qReal::ProjectManagementInterface *mProjectManager {}; // Does not have ownership.
	qReal::LogicalModelAssistInterface *mLogicalModel {}; // Doesn`t have ownership
	QString mCurrentlySelectedModelName;
	QString mCurrentTabPath;
};

}
