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

#include "label.h"

#include <QtGui/QTextCursor>

#include "editor/nodeElement.h"
#include "editor/edgeElement.h"
#include "brandManager/brandManager.h"

using namespace qReal;
using namespace qReal::gui::editor;

Label::Label(models::GraphicalModelAssistApi &graphicalAssistApi
		, models::LogicalModelAssistApi &logicalAssistApi
		, const Id &elementId
		, const QSharedPointer<LabelProperties> &properties)
	: mIsStretched(false)
	, mWasMoved(false)
	, mShouldMove(false)
	, mId(elementId)
	, mGraphicalModelAssistApi(graphicalAssistApi)
	, mLogicalModelAssistApi(logicalAssistApi)
	, mProperties(properties)
{
	setTextInteractionFlags(Qt::NoTextInteraction);
	if (properties->isStatic()) {
		setText(properties->text());
	}

	init();
	setAcceptDrops(true);
}

Label::~Label()
{
}

const LabelProperties &Label::info() const
{
	return *mProperties;
}

void Label::init()
{
	QGraphicsTextItem::setFlags(ItemIsSelectable);
	QGraphicsTextItem::setDefaultTextColor(Qt::black);
	connect(document(), &QTextDocument::contentsChanged, this, &Label::saveToRepo);
	reinitFont();
	setRotation(mProperties->rotation());
	if (!mProperties->isStatic()) {
		QString propertyName = mProperties->binding();
		if (!mProperties->nameForRoleProperty().isEmpty()) {
			propertyName = mProperties->nameForRoleProperty();
		}

		const QList<QPair<QString, QString>> values = mGraphicalModelAssistApi
				.editorManagerInterface().enumValues(mId, propertyName);
		for (const QPair<QString, QString> &pair : values) {
			mEnumValues[pair.first] = pair.second;
		}
	}
}

void Label::moveToParentCenter()
{
	if (mWasMoved) {
		// now it has user defined position, don't center automatically
		return;
	}

	if (orientation() == Qt::Horizontal) {
		qreal parentCenter = mParentContents.x() + mParentContents.width() / 2;
		qreal titleCenter = x() + mContents.width() / 2;
		qreal diff = parentCenter - titleCenter;

		setX(x() + diff);
	} else if (orientation() == Qt::Vertical) {
		qreal parentCenter = mParentContents.y() + mParentContents.height() / 2;
		qreal titleCenter = y() - mContents.width() / 2;
		qreal diff = parentCenter - titleCenter;

		setY(y() + diff);
	}
}

Qt::Orientation Label::orientation()
{
	if (qAbs(rotation()) == 90) {
		return Qt::Vertical;
	}

	return Qt::Horizontal;
}

void Label::setText(const QString &text)
{
	setPlainText(text);
}

void Label::setTextFromRepo(const QString &text)
{
	const QString friendlyText = mEnumValues.contains(text) ? mEnumValues[text] :
			mEnumValues.isEmpty() || textInteractionFlags() & Qt::TextEditorInteraction
					? text
					: enumText(text);
	QString plainText = toPlainText();
	if (friendlyText != plainText) {
		QGraphicsTextItem::setPlainText(friendlyText);
		setText(toPlainText());
		updateData();
	}
}

void Label::setParentContents(const QRectF &contents)
{
	mParentContents = contents;
	scaleCoordinates(contents);
}

void Label::setShouldCenter(bool shouldCenter)
{
	mWasMoved = !shouldCenter;
}

void Label::scaleCoordinates(const QRectF &contents)
{
	if (mWasMoved) {
		return;
	}

	const qreal x = mProperties->x() * (!mProperties->scalingX() ? mContents.width() : contents.width());
	const qreal y = mProperties->y() * (!mProperties->scalingY() ? mContents.height() : contents.height());

	setPos(x, y);
}

void Label::setFlags(GraphicsItemFlags flags)
{
	QGraphicsTextItem::setFlags(flags);
}

void Label::setTextInteractionFlags(Qt::TextInteractionFlags flags)
{
	QGraphicsTextItem::setTextInteractionFlags(flags);
}

void Label::setHtml(const QString &html)
{
	QGraphicsTextItem::setHtml(html);
}

void Label::setPlainText(const QString &text)
{
	QString currentText = toPlainText();
	if (currentText != text) {
		QGraphicsTextItem::setPlainText(text);
	}
}

void Label::setPrefix(const QString &text)
{
	mProperties->setPrefix(text);
}

void Label::setSuffix(const QString &text)
{
	mProperties->setSuffix(text);
}

QString Label::location() const
{
	return mProperties->binding();
}

void Label::updateData(bool withUndoRedo)
{
	const QString value = toPlainText();
	Element * const parent = dynamic_cast<Element *>(parentItem());
	if (!mProperties->nameForRoleProperty().isEmpty()) {
		if (mEnumValues.isEmpty()) {
			parent->setLogicalProperty(mProperties->nameForRoleProperty()
					, mTextBeforeTextInteraction
					, value
					, withUndoRedo
			);
		} else {
			const QString repoValue = mEnumValues.values().contains(value)
					? mEnumValues.key(value)
					: (withUndoRedo ? enumText(value) : value);
			parent->setLogicalProperty(mProperties->nameForRoleProperty()
					, mTextBeforeTextInteraction
					, repoValue
					, withUndoRedo
			);
		}
	} else if (mProperties->binding() == "name") {
		if (value != parent->name()) {
			parent->setName(value, withUndoRedo);
		}
	} else if (mEnumValues.isEmpty()) {
		const QString properties = mLogicalModelAssistApi.mutableLogicalRepoApi().property(mGraphicalModelAssistApi.
				logicalId(mId), "dynamicProperties").toString();
		if (!properties.isEmpty()) {
			QDomDocument dynamicProperties;
			dynamicProperties.setContent(properties);

			for (QDomElement element
					= dynamicProperties.firstChildElement("properties").firstChildElement("property")
					; !element.isNull()
					; element = element.nextSiblingElement("property"))
			{
				if (element.attribute("name") == mProperties->binding()) {
					element.setAttribute("dynamicPropertyValue", value);
					break;
				}
			}

			mLogicalModelAssistApi.mutableLogicalRepoApi().setProperty(mGraphicalModelAssistApi.logicalId(mId),
					"dynamicProperties", dynamicProperties.toString(4));
		}

		parent->setLogicalProperty(mProperties->binding(), mTextBeforeTextInteraction, value, withUndoRedo);
	} else {
		const QString repoValue = mEnumValues.values().contains(value)
				? mEnumValues.key(value)
				: (withUndoRedo ? enumText(value) : value);
		parent->setLogicalProperty(mProperties->binding(), mTextBeforeTextInteraction, repoValue, withUndoRedo);
		disconnect(document(), &QTextDocument::contentsChanged, this, &Label::saveToRepo);
		if (repoValue == mTextBeforeTextInteraction) {
			setText(repoValue);
		}

		connect(document(), &QTextDocument::contentsChanged, this, &Label::saveToRepo, Qt::UniqueConnection);
	}

	mGraphicalModelAssistApi.setLabelPosition(mId, mProperties->index(), pos());
	mGraphicalModelAssistApi.setLabelSize(mId, mProperties->index(), boundingRect().size());
}

void Label::reinitFont()
{
	setFont(BrandManager::fonts()->sceneLabelsFont());
}

void Label::saveToRepo()
{
	updateData(false);
}

void Label::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (dynamic_cast<EdgeElement *>(parentItem())) {
		// Passing event to edge because users usially want to edit its property when clicking on it.
		QGraphicsItem::mousePressEvent(event);
		return;
	}

	mIsStretched = (event->pos().x() >= boundingRect().right() - 10
			&& event->pos().y() >= boundingRect().bottom() - 10);

	QGraphicsTextItem::mousePressEvent(event);
	parentItem()->setSelected(true);
	event->accept();
	setSelected(true);
}

void Label::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (!mShouldMove) {
		setSelected(false);
		return;
	}

	mWasMoved = true;

	if (!labelMovingRect().contains(event->pos())) {
		event->ignore();
		return;
	}

	QGraphicsTextItem::mouseMoveEvent(event);
	event->accept();
}

void Label::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	mShouldMove = true;

	updateData();

	QGraphicsTextItem::mouseReleaseEvent(event);
	parentItem()->setSelected(true);
	setSelected(true);
}

void Label::init(const QRectF &contents)
{
	mContents = contents;
	mParentContents = contents;

	if (mGraphicalModelAssistApi.hasLabel(mId, mProperties->index())) {
		const QPointF currentPos = mGraphicalModelAssistApi.labelPosition(mId, mProperties->index());
		mProperties->setX(currentPos.x() / mContents.width());
		mProperties->setY(currentPos.y() / mContents.height());
		setPos(currentPos);
	} else {
		const qreal x = mProperties->x() * mContents.width();
		const qreal y = mProperties->y() * mContents.height();
		setPos(x, y);
		mGraphicalModelAssistApi.createLabel(mId, mProperties->index(), QPointF(x, y), boundingRect().size());
	}
}

void Label::setScaling(bool scalingX, bool scalingY)
{
	mProperties->setScalingX(scalingX);
	mProperties->setScalingY(scalingY);
}

void Label::setBackground(const QColor &background)
{
	mProperties->setBackground(background);
}

bool Label::isHard() const
{
	return mProperties->isHard();
}

void Label::setHard(bool hard)
{
	mProperties->setHard(hard);
}

bool Label::isReadOnly() const
{
	return mProperties->isReadOnly();
}

void Label::focusOutEvent(QFocusEvent *event)
{
	if (event->reason() != Qt::PopupFocusReason) {
		// Clear selection and focus
		QGraphicsTextItem::focusOutEvent(event);
		setTextInteractionFlags(Qt::NoTextInteraction);
		QTextCursor cursor = textCursor();
		cursor.clearSelection();
		setTextCursor(cursor);
		unsetCursor();
	}

	if (isReadOnly()) {
		return;
	}

	if (mTextBeforeTextInteraction != toPlainText()) {
		updateData(true);
	}

}

void Label::keyPressEvent(QKeyEvent *event)
{
	const int keyEvent = event->key();
	if (keyEvent == Qt::Key_Escape) {
		// Restore previous text and loose focus
		setText(mTextBeforeTextInteraction);
		clearFocus();
		return;
	}

	if ((event->modifiers() & Qt::ShiftModifier) && (event->key() == Qt::Key_Return)) {
		// Line feed
		QTextCursor cursor = textCursor();
		QString currentText = toPlainText();
		const int oldPos = cursor.position();
		currentText.insert(oldPos, "\n");
		setText(currentText);
		cursor.movePosition(QTextCursor::Start);
		cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, oldPos + 1);
		setTextCursor(cursor);
		return;
	}

	if (keyEvent == Qt::Key_Enter || keyEvent == Qt::Key_Return) {
		// Loose focus: new name will be applied in focusOutEvent
		clearFocus();
		return;
	}

	QGraphicsTextItem::keyPressEvent(event);
}

void Label::startTextInteraction()
{
	// Already interacting?
	if (hasFocus()) {
		return;
	}

	mTextBeforeTextInteraction = toPlainText();

	setTextInteractionFlags(isReadOnly() ? Qt::TextBrowserInteraction : Qt::TextEditorInteraction);
	setFocus(Qt::OtherFocusReason);

	// Set full text selection
	QTextCursor cursor = QTextCursor(document());
	cursor.select(QTextCursor::Document);
	setTextCursor(cursor);
	setCursor(Qt::IBeamCursor);
}

void Label::updateDynamicData()
{
	const QString properties = mLogicalModelAssistApi.mutableLogicalRepoApi().property(
			mGraphicalModelAssistApi.logicalId(mId), "dynamicProperties").toString();
	if (!properties.isEmpty()) {
		QDomDocument dynamicProperties;
		dynamicProperties.setContent(properties);

		for (QDomElement element = dynamicProperties.firstChildElement("properties").firstChildElement("property")
				; !element.isNull()
				; element = element.nextSiblingElement("property"))
		{
			if (element.attribute("textBinded") == mProperties->binding()) {
				setText(element.attribute("value"));
				break;
			}
		}
	}
}

void Label::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (toPlainText().isEmpty() && !parentItem()->isSelected()
			&& !isSelected() && dynamic_cast<EdgeElement *>(parentItem())) {
		/// @todo: Why label decides it? Why not edge element itself?
		return;
	}

	painter->save();
	painter->setBrush(mProperties->background());

	if (isSelected()) {
		painter->setPen(QPen(Qt::DashLine));
	} else {
		painter->setPen(QPen(Qt::lightGray, 1, Qt::DotLine));
	}

	painter->drawRoundedRect(QGraphicsTextItem::boundingRect(), 2, 2);
	painter->restore();

	painter->save();
	painter->setFont(font());
	drawText(painter, prefixRect(), mProperties->prefix());
	drawText(painter, suffixRect(), mProperties->suffix());
	painter->restore();

	// Default dashed frame is drawn arround the whole bounding rect (arround prefix and suffix too). Disabling it.
	const_cast<QStyleOptionGraphicsItem *>(option)->state &= ~QStyle::State_Selected & ~QStyle::State_HasFocus;
	
	QGraphicsTextItem::paint(painter, option, widget);
}

void Label::drawText(QPainter *painter, const QRectF &rect, const QString &text)
{
	painter->drawText(rect, Qt::AlignCenter, text);
}

QRectF Label::prefixRect() const
{
	const QRectF thisRect = QGraphicsTextItem::boundingRect();
	QRectF textRect = this->textRect(mProperties->prefix());
	const qreal x = thisRect.left() - textRect.width();
	const qreal y = thisRect.top() + (thisRect.height() - textRect.height()) / 2;
	textRect.moveTo(x, y);
	return textRect;
}

QRectF Label::suffixRect() const
{
	const QRectF thisRect = QGraphicsTextItem::boundingRect();
	QRectF textRect = this->textRect(mProperties->suffix());
	const qreal x = thisRect.right();
	const qreal y = thisRect.top() + (thisRect.height() - textRect.height()) / 2;
	textRect.moveTo(x, y);
	return textRect;
}

QRectF Label::textRect(const QString &text) const
{
	return QFontMetrics(font()).boundingRect(text).adjusted(-3, 0, 3, 0);
}

QRectF Label::boundingRect() const
{
	return QGraphicsTextItem::boundingRect().united(prefixRect()).united(suffixRect());
}

void Label::updateRect(QPointF newBottomRightPoint)
{
	mContents.setBottomRight(newBottomRightPoint);
	setTextWidth(mContents.width());
}

void Label::clearMoveFlag()
{
	mShouldMove = false;
}

QRectF Label::labelMovingRect() const
{
	const int distance = SettingsManager::value("LabelsDistance").toInt();
	return mapFromItem(parentItem(), parentItem()->boundingRect()).boundingRect()
			.adjusted(-distance, -distance, distance, distance);
}

QString Label::enumText(const QString &enumValue) const
{
	return mGraphicalModelAssistApi.editorManagerInterface().isEnumEditable(mId, mProperties->binding())
			? enumValue
			: mTextBeforeTextInteraction;
}
