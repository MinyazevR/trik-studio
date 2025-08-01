/* Copyright 2018 CyberTech Labs Ltd.
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

#include "ballItem.h"

#include <QtGui/QIcon>
#if (QT_VERSION <= QT_VERSION_CHECK(6, 0, 0))
        #include <QtWidgets/QAction>
#else
        #include <QtGui/QAction>
#endif
#include <QtSvg/QSvgRenderer>
#include <QDebug>
#include <twoDModel/engine/model/constants.h>
#include "../view/parts/itemPropertiesDialog.h"

using namespace twoDModel::items;

BallItem::BallItem(const QPointF &position)
        : SolidGraphicItem(new twoDModel::view::ItemPropertiesDialog()),
          mRadius(ballRadius),
          mMass(ballMass),
          mFriction(ballFriction),
          mRestitution(ballRestituion),
          mAngularDamping(ballAngularDamping),
          mLinearDamping(ballLinearDamping),
          mSvgRenderer(new QSvgRenderer)

{
	mSvgRenderer->load(QStringLiteral(":/icons/2d_ball.svg"));
	setPos(position);
	setZValue(ZValue::Moveable);
	setTransformOriginPoint(boundingRect().center());
	emit defaultParamsSetted(this);
}

BallItem::~BallItem()
{
	delete mSvgRenderer;
}

QAction *BallItem::ballTool()
{
	QAction * const result = new QAction(QIcon(QStringLiteral(":/icons/2d_ball.svg")), tr("Ball (B)"), nullptr);
	result->setShortcuts({QKeySequence(Qt::Key_B), QKeySequence(Qt::Key_4)});
	result->setCheckable(true);
	return result;
}

QRectF BallItem::boundingRect() const
{
	return QRectF({-static_cast<qreal>(mRadius / 2.0), -static_cast<qreal>(mRadius / 2.0)}
	              , QSize{mRadius, mRadius});
}

void BallItem::drawItem(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)
	auto rect = QRectF({-static_cast<qreal>(30 / 2.0), -static_cast<qreal>(30 / 2.0)}
	                   , QSize{30, 30});
	mSvgRenderer->render(painter, rect);
}

void BallItem::setPenBrushForExtraction(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
	Q_UNUSED(option)
	painter->setPen(getStrokePen());
	if (isSelected()) {
		QColor extraColor = getStrokePen().color();
		extraColor.setAlphaF(0.5);
		painter->setBrush(extraColor);
	}
}

void BallItem::drawExtractionForItem(QPainter *painter)
{
	painter->drawEllipse(boundingRect());
}

void BallItem::savePos()
{
	saveStartPosition();
	AbstractItem::savePos();
}

QDomElement BallItem::serialize(QDomElement &element) const
{
	QDomElement ballNode = AbstractItem::serialize(element);
	ballNode.setTagName(QStringLiteral("ball"));
	ballNode.setAttribute(QStringLiteral("x"), QString::number(x1() + scenePos().x()));
	ballNode.setAttribute(QStringLiteral("y"), QString::number(y1() + scenePos().y()));
	ballNode.setAttribute(QStringLiteral("markerX"), QString::number(x1() + mStartPosition.x()));
	ballNode.setAttribute(QStringLiteral("markerY"), QString::number(y1() + mStartPosition.y()));
	ballNode.setAttribute(QStringLiteral("rotation"), QString::number(rotation()));
	ballNode.setAttribute(QStringLiteral("startRotation"), QString::number(mStartRotation));
	ballNode.setAttribute(QStringLiteral("radius"), QString::number(mRadius));
	ballNode.setAttribute(QStringLiteral("mass"), QString::number(mMass));
	ballNode.setAttribute(QStringLiteral("friction"), QString::number(mFriction));
	ballNode.setAttribute(QStringLiteral("restitution"), QString::number(mRestitution));
	ballNode.setAttribute(QStringLiteral("angularDumping"), QString::number(mAngularDamping));
	ballNode.setAttribute(QStringLiteral("linearDumping"), QString::number(mLinearDamping));
	return ballNode;
}

void BallItem::deserialize(const QDomElement &element)
{
	AbstractItem::deserialize(element);

	qreal x = element.attribute(QStringLiteral("x"), QStringLiteral("0")).toDouble();
	qreal y = element.attribute(QStringLiteral("y"), QStringLiteral("0")).toDouble();
	qreal markerX = element.attribute(QStringLiteral("markerX"), QStringLiteral("0")).toDouble();
	qreal markerY = element.attribute(QStringLiteral("markerY"), QStringLiteral("0")).toDouble();
	qreal rotation = element.attribute(QStringLiteral("rotation"), QStringLiteral("0")).toDouble();
	mStartRotation = element.attribute(QStringLiteral("startRotation"), QStringLiteral("0")).toDouble();

	if (element.hasAttribute(QStringLiteral("radius"))) {
		mRadius = element.attribute(QStringLiteral("radius"), QStringLiteral("0")).toInt();
	}

	if (element.hasAttribute(QStringLiteral("mass"))) {
		mMass = element.attribute(QStringLiteral("mass"), QStringLiteral("0")).toDouble();
	}

	if (element.hasAttribute(QStringLiteral("friction"))) {
		mFriction = element.attribute(QStringLiteral("friction"), QStringLiteral("0")).toDouble();
	}

	if (element.hasAttribute(QStringLiteral("restitution"))) {
		mRestitution = element.attribute(QStringLiteral("restitution"), QStringLiteral("0")).toDouble();
	}

	if (element.hasAttribute(QStringLiteral("angularDumping"))) {
		mAngularDamping = element.attribute(QStringLiteral("angularDumping"), QStringLiteral("0")).toDouble();
	}

	if (element.hasAttribute(QStringLiteral("linearDumping"))) {
		mLinearDamping = element.attribute(QStringLiteral("linearDumping"), QStringLiteral("0")).toDouble();
	}

	setPos(QPointF(x, y));
	setTransformOriginPoint(boundingRect().center());
	mStartPosition = {markerX, markerY};
	setRotation(rotation);
	Q_EMIT x1Changed(x1());
	Q_EMIT allItemParamsChanged(this);
}

QPainterPath BallItem::shape() const
{
	QPainterPath result;
	result.addEllipse(boundingRect());
	return result;
}

void BallItem::setRestitution(const qreal restitution)
{
	mRestitution = restitution;
	Q_EMIT itemParamsChanged(this);
}

void BallItem::setFriction(const qreal friction)
{
	mFriction = friction;
	Q_EMIT itemParamsChanged(this);
}

void BallItem::setMass(const qreal mass)
{
	qDebug() << "IN SET MASS";
	mMass = mass;
	Q_EMIT itemParamsChanged(this);
}

void BallItem::setRadius(const qreal radius)
{
	qDebug() << "IN SET radius";
	mRadius = radius;
	Q_EMIT allItemParamsChanged(this);
}

void BallItem::onDialogAccepted() {
	auto currentSettings = propertyDialog()->currentSettings();

	for (auto &&key: currentSettings.keys()) {
		auto value = currentSettings[key];
		if (key == "Radius") {
			setRadius(value.toDouble());
		}
	}
	SolidGraphicItem::onDialogAccepted();
}

void BallItem::saveStartPosition()
{
	if (this->editable()) {
		mStartPosition = pos();
		mStartRotation = rotation();
		Q_EMIT x1Changed(x1());
	}
}

void BallItem::returnToStartPosition()
{
	setPos(mStartPosition);
	setRotation(mStartRotation);
	Q_EMIT x1Changed(x1());
}

QPolygonF BallItem::collidingPolygon() const
{
	return QPolygonF(boundingRect().adjusted(1, 1, -1, -1).translated(scenePos()));
}

qreal BallItem::angularDamping() const
{
	return mAngularDamping;
}

qreal BallItem::linearDamping() const
{
	return mLinearDamping;
}

QPainterPath BallItem::path() const
{
	QPainterPath path;
	QPolygonF collidingPlgn = collidingPolygon();
	QMatrix m;
	m.rotate(rotation());

	QPointF firstP = collidingPlgn.at(0);
	collidingPlgn.translate(-firstP.x(), -firstP.y());

	path.addEllipse(collidingPlgn.boundingRect());
	path = m.map(path);
	path.translate(firstP.x(), firstP.y());

	return path;
}

bool BallItem::isCircle() const
{
	return true;
}

qreal BallItem::mass() const
{
	return mMass;
}

qreal BallItem::friction() const
{
	return mFriction;
}

qreal BallItem::restitution() const
{
	return mRestitution;
}

QMap<QString, QVariant> BallItem::defaultParams() const
{
	return {
		{"Radius", mRadius},
		{"Restitution", mRestitution},
		{"Friction", mFriction},
		{"Mass", mMass},
		{"Angular Damping", mAngularDamping},
		{"Linear Damping", mLinearDamping}
	};
}

SolidItem::BodyType BallItem::bodyType() const
{
	return SolidItem::DYNAMIC;
}
