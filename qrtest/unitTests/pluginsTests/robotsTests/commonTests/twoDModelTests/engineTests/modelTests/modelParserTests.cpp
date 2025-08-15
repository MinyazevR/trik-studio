#include "modelParserTests.h"
#include <src/robotModel/nullTwoDRobotModel.h>

// #include <twoDModel/engine/twoDModelEngineFacade.h>
using namespace qrTest::robotsTests::commonTwoDModelTests;

ModelParserTests::ModelParserTests()
        : mModel(new twoDModel::model::Model())
        , mTwoDRobotModel(new twoDModel::robotModel::NullTwoDRobotModel("testRobot"))
{
	qDebug() << "lol";
	mModel->addRobotModel(*mTwoDRobotModel.data());
	qDebug() << "kek";
}

ModelParserTests::~ModelParserTests()
{
}

void ModelParserTests::SetUp()
{
}

/*
 * 	const QString xml =
			"<constraints>"
			"	<timelimit value=\"2000\"/>"
			"	<event id=\"event\" settedUpInitially=\"true\" dropsOnFire=\"false\">"
			"		<conditions glue=\"and\">"
			"			<condition>"
			"				<timer timeout=\"1000\" forceDropOnTimeout=\"false\"/>"
			"			</condition>"
			"			<condition>"
			"				<timer timeout=\"1500\" forceDropOnTimeout=\"false\"/>"
			"			</condition>"
			"		</conditions>"
			"		<trigger>"
			"			<success/>"
			"		</trigger>"
			"	</event>"
			"</constraints>";
 */

TEST_F(ModelParserTests, initialSettingTest)
{
	auto &settings = mModel->settings();
	EXPECT_EQ(settings.pixelsInCm(), twoDModel::pixelsInCm);
	EXPECT_EQ(settings.realisticMotors(), false);
	EXPECT_EQ(settings.realisticPhysics(), false);
	EXPECT_EQ(settings.realisticPhysics(), false);

	QDomDocument doc;
	const QString xml =
	                        "<constraints>"
	                        "	<timelimit value=\"2000\"/>"
	                        "	<event id=\"event\" settedUpInitially=\"true\" dropsOnFire=\"false\">"
	                        "		<conditions glue=\"and\">"
	                        "			<condition>"
	                        "				<timer timeout=\"1000\" forceDropOnTimeout=\"false\"/>"
	                        "			</condition>"
	                        "			<condition>"
	                        "				<timer timeout=\"1500\" forceDropOnTimeout=\"false\"/>"
	                        "			</condition>"
	                        "		</conditions>"
	                        "		<trigger>"
	                        "			<success/>"
	                        "		</trigger>"
	                        "	</event>"
	                        "</constraints>";
	doc.setContent(xml);
	mModel->deserialize(doc);
}
