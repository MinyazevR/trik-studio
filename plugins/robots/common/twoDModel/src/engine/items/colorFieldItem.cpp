/* Copyright 2007-2015 QReal Research Group
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

#include "colorFieldItem.h"

using namespace twoDModel::items;

ColorFieldItem::ColorFieldItem(QGraphicsItem* parent)
	: graphicsUtils::AbstractItem(parent)
{
	setZValue(ZValue::Shape);
	mStrokePen.setWidthF(1.75);
}

ColorFieldItem::~ColorFieldItem()
{
}

QColor ColorFieldItem::color() const
{
	return pen().color();
}

void ColorFieldItem::setColor(const QColor &color)
{
	setPenColor(color.name());
	setBrushColor(color.name());
	update();
}

int ColorFieldItem::thickness() const
{
	return pen().width();
}

void ColorFieldItem::setThickness(int thickness)
{
	setPenWidth(thickness);
	update();
}
