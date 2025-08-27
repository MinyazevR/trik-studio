/* Copyright 2017-2018 CyberTech Labs Ltd.
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

#include "skittleItem.h"

#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtSvg/QSvgRenderer>

#include <twoDModel/engine/model/constants.h>

namespace {
	static constexpr int skittleRadius = 20;
	static constexpr qreal skittleMass = 0.05f;
	static constexpr qreal skittleFriction = 0.2f;
	static constexpr qreal skittleRestituion = 0.8f;
	static constexpr qreal skittleAngularDamping = 6.0f;
	static constexpr qreal skittleLinearDamping = 6.0f;
}

using namespace twoDModel::items;

SkittleItem::SkittleItem(graphicsUtils::AbstractCoordinateSystem *metricSystem,
			const QPointF &position)
	: mSvgRenderer(new QSvgRenderer),
	mRadiusPx(skittleRadius),
	mMass(skittleMass),
	mFriction(skittleFriction),
	mRestitution(skittleRestituion),
	mAngularDamping(skittleAngularDamping),
	mLinearDamping(skittleLinearDamping)
{
	setCoordinateSystem(metricSystem);
	mSvgRenderer->load(QString(":/icons/2d_can.svg"));
	setPos(position);
	setZValue(ZValue::Moveable);
	setTransformOriginPoint(boundingRect().center());
}

SkittleItem::~SkittleItem()
{
	delete mSvgRenderer;
}

QAction *SkittleItem::skittleTool()
{
	QAction * const result = new QAction(QIcon(":/icons/2d_can.svg"), tr("Can (C)"), nullptr);
	result->setShortcuts({QKeySequence(Qt::Key_C), QKeySequence(Qt::Key_3)});
	result->setCheckable(true);
	return result;
}

QRectF SkittleItem::boundingRect() const
{
	return QRectF({ -mRadiusPx / 2, -mRadiusPx / 2}, QSizeF{mRadiusPx, mRadiusPx});
}

void SkittleItem::drawItem(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)
	mSvgRenderer->render(painter, boundingRect());
}

void SkittleItem::setPenBrushForExtraction(QPainter *painter, const QStyleOptionGraphicsItem *option)
{
	Q_UNUSED(option)
	painter->setPen(getStrokePen());
	if (isSelected()) {
		QColor extraColor = getStrokePen().color();
		extraColor.setAlphaF(0.5);
		painter->setBrush(extraColor);
	}
}

void SkittleItem::drawExtractionForItem(QPainter *painter)
{
	painter->drawEllipse(boundingRect());
}

void SkittleItem::savePos()
{
	saveStartPosition();
	AbstractItem::savePos();
}

QDomElement SkittleItem::serialize(QDomElement &element) const
{
	QDomElement skittleNode = AbstractItem::serialize(element);
	skittleNode.setTagName("skittle");
	auto *coordSystem = coordinateSystem();
	skittleNode.setAttribute("x",
	                         QString::number(coordSystem->toUnit(x1() + scenePos().x())));
	skittleNode.setAttribute("y",
	                         QString::number(coordSystem->toUnit(y1() + scenePos().y())));
	skittleNode.setAttribute("markerX",
	                         QString::number(coordSystem->toUnit(x1() + mStartPosition.x())));
	skittleNode.setAttribute("markerY",
	                         QString::number(coordSystem->toUnit(y1() + mStartPosition.y())));
	skittleNode.setAttribute("rotation", QString::number(rotation()));
	skittleNode.setAttribute("startRotation", QString::number(mStartRotation));
	skittleNode.setAttribute("radius", QString::number(coordSystem->toUnit(mRadiusPx)));
	skittleNode.setAttribute("mass", QString::number(mMass));
	skittleNode.setAttribute("friction", QString::number(mFriction));
	skittleNode.setAttribute("restitution", QString::number(mRestitution));
	skittleNode.setAttribute("angularDamping", QString::number(mAngularDamping));
	skittleNode.setAttribute("linearDamping", QString::number(mLinearDamping));
	return skittleNode;
}

void SkittleItem::deserialize(const QDomElement &element)
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

void SkittleItem::saveStartPosition()
{
	if (this->editable()) {
		mStartPosition = pos();
		mStartRotation = rotation();
		emit x1Changed(x1());
	}
}

void SkittleItem::returnToStartPosition()
{
	setPos(mStartPosition);
	setRotation(mStartRotation);
	emit x1Changed(x1());
}

QPolygonF SkittleItem::collidingPolygon() const
{
	return QPolygonF(boundingRect().adjusted(1, 1, -1, -1).translated(scenePos()));
}

qreal SkittleItem::angularDamping() const
{
	return mAngularDamping;
}

qreal SkittleItem::linearDamping() const
{
	return mLinearDamping;
}

QPainterPath SkittleItem::path() const
{
	QPainterPath path;
	QPolygonF collidingPlgn = collidingPolygon();
	QMatrix m;
	m.rotate(rotation());

	const QPointF firstP = collidingPlgn.at(0);
	collidingPlgn.translate(-firstP.x(), -firstP.y());

	path.addEllipse(collidingPlgn.boundingRect());
	path = m.map(path);
	path.translate(firstP.x(), firstP.y());

	return path;
}

bool SkittleItem::isCircle() const
{
	return true;
}

qreal SkittleItem::mass() const
{
	return mMass;
}

qreal SkittleItem::friction() const
{
	return mFriction;
}

qreal SkittleItem::restitution() const
{
	return mRestitution;
}

SolidItem::BodyType SkittleItem::bodyType() const
{
	return SolidItem::DYNAMIC;
}

void SkittleItem::setRadius(const qreal radius)
{
	prepareGeometryChange();
	mRadiusPx = radius;
}
