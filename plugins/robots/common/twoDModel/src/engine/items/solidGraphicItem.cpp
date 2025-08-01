#include "solidGraphicItem.h"
#include <QMenu>
#include <QDialog>
#include "../view/parts/itemPropertiesDialog.h"
#include <QDebug>
using namespace twoDModel::items;

SolidGraphicItem::SolidGraphicItem(twoDModel::view::ItemPropertiesDialog *dialog)
        : mPropertyDialog(dialog) {
	connect(this, &SolidGraphicItem::defaultParamsSetted,
	                mPropertyDialog.data(), &twoDModel::view::ItemPropertiesDialog::onDefaultItemSetted);
	connect(mPropertyDialog.data(), &twoDModel::view::ItemPropertiesDialog::accepted,
	                this, &SolidGraphicItem::onDialogAccepted);
}

SolidGraphicItem::~SolidGraphicItem() {}

twoDModel::view::ItemPropertiesDialog* SolidGraphicItem::propertyDialog() {
	return mPropertyDialog.data();
}

void SolidGraphicItem::onDialogAccepted() {
	qDebug() << "onDialogAccepted";
	auto settings = mPropertyDialog->currentSettings();
	for (auto &&key: settings.keys()) {
		auto value = settings[key];
		if (key == "Restitution") {
			setRestitution(value.toDouble());
		}
		if (key == "Mass") {
			setMass(value.toDouble());
		}
		if (key == "Friction") {
			setFriction(value.toDouble());
		}
		if (key == "Angular Damping") {
			setAngularDamping(value.toDouble());
		}
		if (key == "Linear Damping") {
			setLinearDamping(value.toDouble());
		}
	}

}

void SolidGraphicItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	if (!editable()) {
		return;
	}

	if (!isSelected()) {
		scene()->clearSelection();
		setSelected(true);
	}

	event->accept();

	QMenu menu;
	QAction *removeAction = menu.addAction(QObject::tr("Remove"));
	QAction *showPropertiesAction = menu.addAction(QStringLiteral("Properties"));
	QAction *selectedAction = menu.exec(event->screenPos());

	if (selectedAction == removeAction) {
		Q_EMIT deletedWithContextMenu();
	} else if (selectedAction == showPropertiesAction) {
		if (mPropertyDialog->exec() == QDialog::Rejected) {
			return;
		}
	}
}
