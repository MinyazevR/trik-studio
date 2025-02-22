/* Copyright 2015 CyberTech Labs Ltd.
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

#include <qrutils/graphicsUtils/lineImpl.h>

#include "colorFieldItem.h"
#include "markerItem.h"

namespace twoDModel {
namespace items {

class CurveItem : public ColorFieldItem
{
	Q_OBJECT

public:
	CurveItem(const QPointF &begin, const QPointF &end);

	AbstractItem *clone() const override;

	/// Creates and returns colored line item for 2D model palette.
	/// Transfers ownership.
	static QAction *curveTool();

	QPainterPath shape() const override;
	QRectF boundingRect() const override;

	void drawItem(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
	void drawExtractionForItem(QPainter *painter) override;

	void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

	void resizeItem(QGraphicsSceneMouseEvent *event) override;
	QPainterPath resizeArea() const override;

	QDomElement serialize(QDomElement &parent) const override;
	void deserialize(const QDomElement &element) override;

	void setEditable(bool editable) override;

private:
	void setPrivateData();
	QPainterPath curveLine() const;

	void deserializePenBrush(const QDomElement &element);
	QPointF deserializePoint(const QString &string) const;

	QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

	MarkerItem mMarker1;
	MarkerItem mMarker2;
};

}
}
