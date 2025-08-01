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
#if (QT_VERSION <= QT_VERSION_CHECK(6, 0, 0))
        #include <QtWidgets/QAction>
#else
        #include <QtGui/QAction>
#endif

#include <QtSvg/QSvgRenderer>
#include <twoDModel/engine/model/constants.h>
#include "../view/parts/itemPropertiesDialog.h"

using namespace twoDModel::items;

SkittleItem::SkittleItem(const QPointF &position)
        : SolidGraphicItem(new twoDModel::view::ItemPropertiesDialog()),
          mRadius(skittleRadius),
          mMass(skittleMass),
          mFriction(skittleFriction),
          mRestitution(skittleRestituion),
          mAngularDamping(skittleAngularDamping),
          mLinearDamping(skittleLinearDamping),
          mSvgRenderer(new QSvgRenderer)
{
	mSvgRenderer->load(QStringLiteral(":/icons/2d_can.svg"));
	setPos(position);
	setZValue(ZValue::Moveable);
	setTransformOriginPoint(boundingRect().center());
	emit defaultParamsSetted(this);
}

SkittleItem::~SkittleItem()
{
	delete mSvgRenderer;
}

QAction *SkittleItem::skittleTool()
{
	QAction * const result = new QAction(QIcon(QStringLiteral(":/icons/2d_can.svg")), tr("Can (C)"), nullptr);
	result->setShortcuts({QKeySequence(Qt::Key_C), QKeySequence(Qt::Key_3)});
	result->setCheckable(true);
	return result;
}

QRectF SkittleItem::boundingRect() const
{
	return QRectF({-static_cast<qreal>(mRadius) / 2, -static_cast<qreal>(mRadius) / 2}
	              , QSize{mRadius, mRadius});
}
// 	Q_EMIT itemParamsChanged(this);
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
	skittleNode.setTagName(QStringLiteral("skittle"));
	skittleNode.setAttribute(QStringLiteral("x"), QString::number(x1() + scenePos().x()));
	skittleNode.setAttribute(QStringLiteral("y"), QString::number(y1() + scenePos().y()));
	skittleNode.setAttribute(QStringLiteral("markerX"), QString::number(x1() + mStartPosition.x()));
	skittleNode.setAttribute(QStringLiteral("markerY"), QString::number(y1() + mStartPosition.y()));
	skittleNode.setAttribute(QStringLiteral("rotation"), QString::number(rotation()));
	skittleNode.setAttribute(QStringLiteral("startRotation"), QString::number(mStartRotation));
	skittleNode.setAttribute(QStringLiteral("mass"), QString::number(mMass));
	skittleNode.setAttribute(QStringLiteral("radius"), QString::number(mRadius));
	skittleNode.setAttribute(QStringLiteral("friction"), QString::number(mFriction));
	skittleNode.setAttribute(QStringLiteral("restitution"), QString::number(mRestitution));
	skittleNode.setAttribute(QStringLiteral("angularDumping"), QString::number(mAngularDamping));
	skittleNode.setAttribute(QStringLiteral("linearDumping"), QString::number(mLinearDamping));
	return skittleNode;
}

void SkittleItem::deserialize(const QDomElement &element)
{
	AbstractItem::deserialize(element);

	qreal x = element.attribute(QStringLiteral("x"), QStringLiteral("0")).toDouble();
	qreal y = element.attribute(QStringLiteral("y"), QStringLiteral("0")).toDouble();
	qreal markerX = element.attribute(QStringLiteral("markerX"), QStringLiteral("0")).toDouble();
	qreal markerY = element.attribute(QStringLiteral("markerY"), QStringLiteral("0")).toDouble();
	qreal rotation = element.attribute(QStringLiteral("rotation"), QStringLiteral("0")).toDouble();
	mStartRotation = element.attribute(QStringLiteral("startRotation"), QStringLiteral("0")).toDouble();

	if (element.hasAttribute(QStringLiteral("radius"))) {
		mRadius = element.attribute(QStringLiteral("radius"), QStringLiteral("0")).toDouble();
	}

	if (element.hasAttribute(QStringLiteral("friction"))) {
		mFriction = element.attribute(QStringLiteral("friction"), QStringLiteral("0")).toDouble();
	}

	if (element.hasAttribute(QStringLiteral("restitution"))) {
		mRestitution = element.attribute(QStringLiteral("restitution"), QStringLiteral("0")).toDouble();
	}

	if (element.hasAttribute(QStringLiteral("angularDumping"))) {
		mAngularDamping = element.attribute(QStringLiteral("angularDamping"), QStringLiteral("0")).toDouble();
	}

	if (element.hasAttribute(QStringLiteral("linearDumping"))) {
		mLinearDamping = element.attribute(QStringLiteral("linearDamping"), QStringLiteral("0")).toDouble();
	}

	setPos(QPointF(x, y));
	setTransformOriginPoint(boundingRect().center());
	mStartPosition = {markerX, markerY};
	setRotation(rotation);
	Q_EMIT x1Changed(x1());
}

void SkittleItem::saveStartPosition()
{
	if (this->editable()) {
		mStartPosition = pos();
		mStartRotation = rotation();
		Q_EMIT x1Changed(x1());
	}
}

void SkittleItem::returnToStartPosition()
{
	setPos(mStartPosition);
	setRotation(mStartRotation);
	Q_EMIT x1Changed(x1());
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

void SkittleItem::onDialogAccepted() {
	auto currentSettings = propertyDialog()->currentSettings();

	for (auto &&key: currentSettings.keys()) {
		auto value = currentSettings[key];
		if (key == "Radius") {
			setRadius(value.toDouble());
		}
	}
	SolidGraphicItem::onDialogAccepted();
}


void SkittleItem::setRadius(const int radius)
{
	mRadius = radius;
}

qreal SkittleItem::friction() const
{
	return mFriction;
}

void SkittleItem::setFriction(const qreal friction)
{
	mFriction = friction;
}

void SkittleItem::setRestitution(const qreal restitution)
{
	mRestitution = restitution;
}

void SkittleItem::setMass(const qreal mass)
{
	mMass =  mass;
}

QMap<QString, QVariant> SkittleItem::defaultParams() const
{
	return {
		{"Radius", mRadius},
		{"Restitution", mRestitution},
		{"Friction", mFriction},
		{"Mass", mMass},
		{"Angular damping", mAngularDamping},
		{"Linear damping", mLinearDamping}
	};
}


qreal SkittleItem::restitution() const
{
	return mRestitution;
}

SolidItem::BodyType SkittleItem::bodyType() const
{
	return SolidItem::DYNAMIC;
}
