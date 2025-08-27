#pragma once

#include "solidItem.h"
#include <qrutils/graphicsUtils/abstractItem.h>
#include <qrkernel/settingsManager.h>

namespace twoDModel {
namespace view {
	class ItemPropertiesDialog;
}

namespace items {

class SolidAbstractItem: public graphicsUtils::AbstractItem, public SolidItem
{
	Q_OBJECT
	Q_DISABLE_COPY(SolidAbstractItem)
public:
//	SolidAbstractItem(twoDModel::view::ItemPropertiesDialog *dialog);
	SolidAbstractItem() = default;
	virtual ~SolidAbstractItem() = default;
//	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
//	twoDModel::view::ItemPropertiesDialog* propertyDialog();
//public Q_SLOTS:
//	virtual void onDialogAccepted();
Q_SIGNALS:
//	void defaultParamsSetted(SolidAbstractItem *);
//	void itemParamsChanged(SolidAbstractItem *);
	void physicItemParamsChanged(SolidAbstractItem *);

//private:
//	QScopedPointer<twoDModel::view::ItemPropertiesDialog> mPropertyDialog;
};
}
}
