#pragma once
#include "twoDModel/robotModel/twoDRobotModel.h"
#include "twoDModel/engine/model/constants.h"
#include <QDebug>

namespace kitBase {
namespace robotModel {
class ConfigurationInterface;
}
}

namespace twoDModel {

namespace model {

class TWO_D_MODEL_EXPORT TwoDRobotModelAdapter: public QObject
{
	Q_OBJECT

public:
	static constexpr int robotWidth = 50;
	static constexpr int robotHeight = 50;
	static constexpr qreal robotMass = 1.05f;
	static constexpr qreal robotFriction = 0.8f;
	static constexpr qreal robotRestitution = 0.3f;
	static constexpr qreal robotAngularDamping = 1.0f;
	static constexpr qreal robotLinearDamping = 1.0f;
	static constexpr qreal defaultWheelRestitution = 0.5f;
	static constexpr qreal defaultWheelFriction = 1.0f;
	static constexpr qreal defaultWheelMass = 0.1f;

	explicit TwoDRobotModelAdapter(twoDModel::robotModel::TwoDRobotModel &twoDRobotModel)
		: mTwoDRobotModel(twoDRobotModel),
		mWidth(robotWidth),
		mHeight(robotHeight),
		mMass(robotMass),
		mFriction(robotFriction),
		mRestitution(robotRestitution),
		mLinearDamping(robotLinearDamping),
		mAngularDamping(robotAngularDamping),
		mWheelRestitution(defaultWheelRestitution),
		mWheelFriction(defaultWheelFriction),
		mWheelMass(defaultWheelMass)
	{
		connect(&mTwoDRobotModel, &twoDModel::robotModel::TwoDRobotModel::settingsChanged,
				this, &TwoDRobotModelAdapter::settingsChanged);
	}

	inline void reinit()
	{
		mWidth = robotWidth;
		mHeight = robotHeight;
		mMass = robotMass;
		mFriction = robotFriction;
		mRestitution = robotRestitution;
		mLinearDamping = robotLinearDamping;
		mAngularDamping = robotAngularDamping;
	}

	inline twoDModel::robotModel::TwoDRobotModel &model() const
	{
		return mTwoDRobotModel;
	}

	inline QString robotId() const
	{
		return mTwoDRobotModel.robotId();
	}

	inline QString name() const
	{
		return mTwoDRobotModel.name();
	}

	inline engine::TwoDModelDisplayWidget *displayWidget() const
	{
		return mTwoDRobotModel.displayWidget();
	}

	inline QList<kitBase::robotModel::PortInfo> configurablePorts() const
	{
		return mTwoDRobotModel.configurablePorts();
	}

	inline QPolygonF collidingPolygon() const
	{
		/* default:
		 * {
		 *	QPointF(1, 10),
		 *	QPointF(47, 10),
		 *	QPointF(49, 20),
		 *	QPointF(49, 30),
		 *	QPointF(47, 40),
		 *	QPointF(1, 40)
		 * }
		*/

		static constexpr qreal firstWidthRoundingWidthFactorPx = 47.0f / 50.0;
		static constexpr qreal secondWidthRoundingWidthFactorPx = 49.0f / 50.0;
		static constexpr qreal heightRoundingWidthFactorPx = 10.0f / 50.0;

		auto currentWidth = mWidth;
		auto currentHeight = mHeight;
		const auto heightStep = heightRoundingWidthFactorPx * currentHeight;

		return QPolygonF({
			QPointF{1, heightStep},
			QPointF{firstWidthRoundingWidthFactorPx * currentWidth, heightStep},
			QPointF{secondWidthRoundingWidthFactorPx * currentWidth, 2 * heightStep},
			QPointF{secondWidthRoundingWidthFactorPx * currentWidth, 3 * heightStep},
			QPointF{firstWidthRoundingWidthFactorPx * currentWidth, 4 * heightStep},
			QPointF{1, 4 * heightStep}
		});
	}

	inline void setMass(const qreal mass) {
		mMass = mass;
		Q_EMIT robotParamChanged();
	}

	inline void setWheelMass(const qreal mass) {
		mWheelMass = mass;
	}

	inline qreal wheelMass() {
		return mWheelMass;
	}

	inline void setWheelRestitution(const qreal restitution) {
		mWheelRestitution = restitution;
	}

	inline qreal wheelRestitution() {
		return mWheelRestitution;
	}

	inline void setWheelFriction(const qreal friction) {
		mWheelFriction = friction;
	}

	inline qreal wheelFriction() {
		return mWheelFriction;
	}

	inline void setSize(const QSizeF &size) {
		mWidth = size.width();
		mHeight = size.height();
		qDebug() << "robotParamChanged" << mWidth << mHeight;
		Q_EMIT robotParamChanged();
	}

	inline void setFriction(const qreal friction) {
		mFriction = friction;
	}

	inline void setRestitution(const qreal restitution) {
		mRestitution = restitution;
	}

	inline void setLinearDamping(const qreal linearDamping) {
		mLinearDamping = linearDamping;
	}

	inline void setAngularDamping(const qreal angularDamping) {
		mAngularDamping = angularDamping;
	}

	inline qreal mass() const
	{
		return mMass;
	}

	inline qreal friction() const
	{
		return mFriction;
	}

	inline qreal restitution() const
	{
		return mRestitution;
	}

	inline qreal angularDamping() const
	{
		return mAngularDamping;
	}

	inline qreal linearDamping() const
	{
		return mLinearDamping;
	}

	inline QSizeF size() const
	{
		qDebug() << "size()" << mWidth << mHeight;
		return QSizeF{mWidth, mHeight};
	}

	inline qreal wheelDiameter() const
	{
		return robotWheelDiameterInPx / robotWidth  * mWidth;
	}

	inline const kitBase::robotModel::ConfigurationInterface &configuration() const
	{
		return mTwoDRobotModel.configuration();
	}

	inline QPointF robotCenter() const
	{
		return QPointF(mWidth / 2, mHeight / 2);
	}

	inline qreal onePercentAngularVelocity() const
	{
		return mTwoDRobotModel.onePercentAngularVelocity();
	}

	inline QRect sensorImageRect(const kitBase::robotModel::DeviceInfo &deviceType) const
	{
		return mTwoDRobotModel.sensorImageRect(deviceType);
	}

	inline QList<QPointF> wheelsPosition() const
	{
		static constexpr qreal widthWheelFactor = 10.0f / 50.0;
		static constexpr qreal heightWheelFactor = 3.0f / 50.0;
		return {QPointF(widthWheelFactor * mWidth, heightWheelFactor * mHeight),
			QPointF(widthWheelFactor * mWidth, mHeight - heightWheelFactor * mHeight)};
	}

	inline QPointF rotationCenter() const
	{
		if (wheelsPosition().size() < 2) {
			return robotCenter();
		}

		return (wheelsPosition()[0] + wheelsPosition()[1]) / 2;
	}

	inline QHash<kitBase::robotModel::PortInfo, kitBase::robotModel::DeviceInfo> specialDevices() const
	{
		return mTwoDRobotModel.specialDevices();
	}

	QList<kitBase::robotModel::PortInfo> availablePorts() const
	{
		return mTwoDRobotModel.availablePorts();
	}

	inline QString sensorImagePath(const kitBase::robotModel::DeviceInfo &deviceType) const
	{
		return mTwoDRobotModel.sensorImagePath(deviceType);
	}

	inline QPair<QPoint, qreal> specialDeviceConfiguration(const kitBase::robotModel::PortInfo &port) const
	{
		return mTwoDRobotModel.specialDeviceConfiguration(port);
	}

	inline QString robotImage() const
	{
		return mTwoDRobotModel.robotImage();
	}

	inline QPair<qreal,int> rangeSensorAngleAndDistance (const kitBase::robotModel::DeviceInfo &deviceType) const
	{
		return mTwoDRobotModel.rangeSensorAngleAndDistance(deviceType);
	}

	inline QList<kitBase::robotModel::DeviceInfo> allowedDevices(const kitBase::robotModel::PortInfo &port) const
	{
		return mTwoDRobotModel.allowedDevices(port);
	}

	inline kitBase::robotModel::PortInfo defaultLeftWheelPort() const
	{
		return mTwoDRobotModel.defaultLeftWheelPort();
	}

	inline kitBase::robotModel::PortInfo defaultRightWheelPort() const
	{
		return mTwoDRobotModel.defaultRightWheelPort();
	}

	inline qreal robotTrack() const {
		if (wheelsPosition().size() < 2) {
			return mWidth;
		}
		return qAbs(wheelsPosition()[0].y() - wheelsPosition()[1].y());
	}

Q_SIGNALS:
	void settingsChanged();
	void robotParamChanged();
private:
	twoDModel::robotModel::TwoDRobotModel &mTwoDRobotModel;
	qreal mWidth;
	qreal mHeight;
	qreal mMass;
	qreal mFriction;
	qreal mRestitution;
	qreal mLinearDamping;
	qreal mAngularDamping;
	qreal mWheelRestitution;
	qreal mWheelFriction;
	qreal mWheelMass;

};

}
}
