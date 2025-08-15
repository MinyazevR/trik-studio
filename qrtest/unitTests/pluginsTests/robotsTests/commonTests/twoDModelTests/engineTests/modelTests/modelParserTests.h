#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QtCore/QScopedPointer>
#include <QtCore/QSharedPointer>

#include <kitBase/kitPluginInterface.h>
#include <twoDModel/engine/model/model.h>
#include <twoDModel/engine/view/twoDModelWidget.h>
#include <twoDModel/robotModel/twoDRobotModel.h>
#include <mocks/plugins/robots/common/kitBase/include/kitBase/robotModel/robotModelInterfaceMock.h>

namespace qrTest {
namespace robotsTests {
namespace commonTwoDModelTests {

/// Tests for ConstraintsParser.
class ModelParserTests : public testing::Test
{
public:
	ModelParserTests();

protected:
	void SetUp() override;
	~ModelParserTests() override;
	QScopedPointer<twoDModel::model::Model> mModel;
//	QScopedPointer<kitBase::robotModel::RobotModelInterface> mRealRobotModel;
	QScopedPointer<twoDModel::robotModel::TwoDRobotModel> mTwoDRobotModel;
};

}
}
}
