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
#include <QtWidgets/QAction>
#include <QtSvg/QSvgRenderer>
#include <QDebug>

namespace {
	static constexpr int defaultBallRadiusPx = 30;
	static constexpr qreal ballMass = 0.3f;
	static constexpr qreal ballFriction = 1.0f;
	static constexpr qreal ballRestituion = 0.2f;
	static constexpr qreal ballAngularDamping = 1.0f;
	static constexpr qreal ballLinearDamping = 1.0f;
}


using namespace twoDModel::items;

BallItem::BallItem(graphicsUtils::AbstractCoordinateSystem *metricSystem,
		const QPointF &position):
	mSvgRenderer(new QSvgRenderer),
	mRadiusPx(defaultBallRadiusPx),
	mMass(ballMass),
	mFriction(ballFriction),
	mRestitution(ballRestituion),
	mAngularDamping(ballAngularDamping),
	mLinearDamping(ballLinearDamping)
{
	qDebug() << "ball mass" << mMass;
	setCoordinateSystem(metricSystem);
	mSvgRenderer->load(QString(":/icons/2d_ball.svg"));
	setPos(position);
	setZValue(ZValue::Moveable);
	setTransformOriginPoint(boundingRect().center());
}

BallItem::~BallItem()
{
	delete mSvgRenderer;
}

QAction *BallItem::ballTool()
{
	QAction * const result = new QAction(QIcon(":/icons/2d_ball.svg"), tr("Ball (B)"), nullptr);
	result->setShortcuts({QKeySequence(Qt::Key_B), QKeySequence(Qt::Key_4)});
	result->setCheckable(true);
	return result;
}

QRectF BallItem::boundingRect() const
{
	return QRectF({-static_cast<qreal>(mRadiusPx / 2.0), -static_cast<qreal>(mRadiusPx / 2.0)}
				  , QSizeF{mRadiusPx, mRadiusPx});
}

void BallItem::drawItem(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)
	mSvgRenderer->render(painter, boundingRect());
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
	auto *coordSystem = coordinateSystem();
	ballNode.setTagName("ball");
	ballNode.setAttribute("x",
	                      QString::number(coordSystem->toUnit(x1() + scenePos().x())));
	ballNode.setAttribute("y",
	                      QString::number(coordSystem->toUnit(y1() + scenePos().y())));
	ballNode.setAttribute("markerX",
	                      QString::number(coordSystem->toUnit(x1() + mStartPosition.x())));
	ballNode.setAttribute("markerY",
	                      QString::number(coordSystem->toUnit(y1() + mStartPosition.y())));
	ballNode.setAttribute("rotation", QString::number(rotation()));
	ballNode.setAttribute("startRotation", QString::number(mStartRotation));
	ballNode.setAttribute("radius", QString::number(coordSystem->toUnit(mRadiusPx)));
	ballNode.setAttribute("mass", QString::number(mMass));
	ballNode.setAttribute("friction", QString::number(mFriction));
	ballNode.setAttribute("restitution", QString::number(mRestitution));
	ballNode.setAttribute("angularDamping", QString::number(mAngularDamping));
	ballNode.setAttribute("linearDamping", QString::number(mLinearDamping));
	return ballNode;
}

void BallItem::deserialize(const QDomElement &element)
{
	AbstractItem::deserialize(element);
	auto *coordSystem = coordinateSystem();
	qreal x = coordSystem->toPx(element.attribute("x", "0").toDouble());
	qreal y = coordSystem->toPx(element.attribute("y", "0").toDouble());
	qreal markerX = coordSystem->toPx(element.attribute("markerX", "0").toDouble());
	qreal markerY = coordSystem->toPx(element.attribute("markerY", "0").toDouble());
	qreal rotation = element.attribute("rotation", "0").toDouble();

	const auto radius = element.attribute("radius", "");
	if (!radius.isEmpty()) {
		setRadius(coordSystem->toPx(radius.toDouble()));
	}
	const auto mass = element.attribute("mass", "");
	if (!mass.isEmpty()) {
		mMass = mass.toDouble();
	}
	const auto friction = element.attribute("friction", "");
	if (!friction.isEmpty()) {
		mFriction = friction.toDouble();
	}
	const auto angularDamping = element.attribute("angularDamping", "");
	if (!angularDamping.isEmpty()) {
		mAngularDamping = angularDamping.toDouble();
	}
	const auto linearDamping = element.attribute("linearDamping", "");
	if (!linearDamping.isEmpty()) {
		mLinearDamping = linearDamping.toDouble();
	}

	mStartRotation = element.attribute("startRotation", "0").toDouble();

	setPos(QPointF(x, y));
	setTransformOriginPoint(boundingRect().center());
	mStartPosition = {markerX, markerY};
	setRotation(rotation);
	emit x1Changed(x1());
}

QPainterPath BallItem::shape() const
{
	QPainterPath result;
	result.addEllipse(boundingRect());
	return result;
}

void BallItem::saveStartPosition()
{
	if (this->editable()) {
		mStartPosition = pos();
		mStartRotation = rotation();
		emit x1Changed(x1());
	}
}

void BallItem::returnToStartPosition()
{
	setPos(mStartPosition);
	setRotation(mStartRotation);
	emit x1Changed(x1());
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

SolidItem::BodyType BallItem::bodyType() const
{
	return SolidItem::DYNAMIC;
}

void BallItem::setRadius(const qreal radius)
{
	prepareGeometryChange();
	mRadiusPx = radius;
}
