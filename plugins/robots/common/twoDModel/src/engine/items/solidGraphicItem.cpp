#include "solidGraphicItem.h"
#include <QMenu>
#include <QDialog>
#include "../view/parts/itemPropertiesDialog.h"

using namespace twoDModel::items;

SolidGraphicItem::SolidGraphicItem(twoDModel::view::ItemPropertiesDialog *dialog)
        : mPropertyDialog(dialog) {
	connect(this, &SolidGraphicItem::defaultParamsSetted,
	                mPropertyDialog.data(), &twoDModel::view::ItemPropertiesDialog::onDefaultItemSetted);
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
