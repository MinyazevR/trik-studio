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
#include <twoDModel/engine/model/constants.h>

using namespace twoDModel::items;

namespace {
        static constexpr int defaultBallRadiusPx = 30;
	static constexpr double ballRadiusCm = 10.5;
	static constexpr qreal ballMass = 0.015f;
	static constexpr qreal ballFriction = 1.0f;
	static constexpr qreal ballRestituion = 0.8f;
	static constexpr qreal ballAngularDamping = 0.09f;
	static constexpr qreal ballLinearDamping = 0.09f;
}

BallItem::~BallItem() = default;

BallItem::BallItem(const QPointF &position)
        : mSvgRenderer(new QSvgRenderer),
          mRadiusPx(defaultBallRadiusPx),
          mMass(ballMass),
          mFriction(ballFriction),
          mRestitution(ballRestituion),
          mAngularDamping(ballAngularDamping),
          mLinearDamping(ballLinearDamping)
{
	mSvgRenderer->load(QString(":/icons/2d_ball.svg"));
	setPos(position);
	setZValue(ZValue::Moveable);
	setTransformOriginPoint(boundingRect().center());
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
	                      , QSize{mRadiusPx, mRadiusPx});
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
	ballNode.setTagName(QStringLiteral("ball"));
	ballNode.setAttribute(QStringLiteral("x"), QString::number(x1() + scenePos().x()));
	ballNode.setAttribute(QStringLiteral("y"), QString::number(y1() + scenePos().y()));
	ballNode.setAttribute(QStringLiteral("markerX"), QString::number(x1() + mStartPosition.x()));
	ballNode.setAttribute(QStringLiteral("markerY"), QString::number(y1() + mStartPosition.y()));
	ballNode.setAttribute(QStringLiteral("rotation"), QString::number(rotation()));
	ballNode.setAttribute(QStringLiteral("startRotation"), QString::number(mStartRotation));

	/* todo: add attributes to xml if user set this params manually and they equals default values
	 * ballNode.setAttribute(QStringLiteral("radius"), QString::number(mRadiusPx));
	 * ballNode.setAttribute(QStringLiteral("mass"), QString::number(mMass));
	 * ballNode.setAttribute(QStringLiteral("friction"), QString::number(mFriction));
	 * ballNode.setAttribute(QStringLiteral("restitution"), QString::number(mRestitution));
	 * ballNode.setAttribute(QStringLiteral("angularDumping"), QString::number(mAngularDamping));
	 * ballNode.setAttribute(QStringLiteral("linearDumping"), QString::number(mLinearDamping));
	*/

	return ballNode;
}

void BallItem::deserialize(const QDomElement &element)
{
	AbstractItem::deserialize(element);
	qreal x = AbstractItem::toPx(element.attribute("x", "0"));
	qreal y = AbstractItem::toPx(element.attribute("y", "0"));

	qreal markerX = AbstractItem::toPx(element.attribute("markerX", "0"));
	qreal markerY = AbstractItem::toPx(element.attribute("markerY", "0"));
	qreal rotation = AbstractItem::toPx(element.attribute("rotation", "0"));
	mStartRotation = AbstractItem::toPx(element.attribute("startRotation", "0"));
	qDebug() << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n";
	qDebug() << x << y << markerX << markerY << rotation;
	setPos(QPointF(x, y));
	setTransformOriginPoint(boundingRect().center());
	mStartPosition = {markerX, markerY};
	setRotation(rotation);
	Q_EMIT x1Changed(x1());
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

void BallItem::onPixelsInCmChanged(const qreal pixelsInCm) {
	mRadiusPx = pixelsInCm * ballRadiusCm;
	AbstractItem::onPixelsInCmChanged(pixelsInCm);
}

qreal BallItem::angularDamping() const
{
	return 0.09f;
}

qreal BallItem::linearDamping() const
{
	return 0.09f;
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
	return 0.015f;
}

qreal BallItem::friction() const
{
	return 1.0f;
}

SolidItem::BodyType BallItem::bodyType() const
{
	return SolidItem::DYNAMIC;
}
