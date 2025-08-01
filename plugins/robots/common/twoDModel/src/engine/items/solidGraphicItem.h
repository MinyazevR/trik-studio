#pragma once

#include "solidItem.h"
#include <qrutils/graphicsUtils/abstractItem.h>
#include <qrkernel/settingsManager.h>

namespace twoDModel {
namespace view {
        class ItemPropertiesDialog;
}

namespace items {

class SolidGraphicItem: public graphicsUtils::AbstractItem, public SolidItem
{
	Q_OBJECT
	Q_DISABLE_COPY(SolidGraphicItem)
public:
	SolidGraphicItem(twoDModel::view::ItemPropertiesDialog *dialog);
	virtual ~SolidGraphicItem() = 0;
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
	twoDModel::view::ItemPropertiesDialog* propertyDialog();
public Q_SLOTS:
	virtual void onDialogAccepted();
Q_SIGNALS:
	void defaultParamsSetted(SolidGraphicItem *);
	void itemParamsChanged(SolidGraphicItem *);
	void allItemParamsChanged(SolidGraphicItem *);

private:
	QScopedPointer<twoDModel::view::ItemPropertiesDialog> mPropertyDialog;
};
}
}
