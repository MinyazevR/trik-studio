#pragma once
#include <QDialog>
#include <qrkernel/settingsManager.h>
#include <QFormLayout>

namespace twoDModel {
namespace items {
        class SolidGraphicItem;
}

namespace view {

class ItemPropertiesDialog: public QDialog
{
	Q_OBJECT
public:
	ItemPropertiesDialog();
	void setDefaultValues(QMap<QString, QVariant> &defaultValues);

public Q_SLOTS:
	 void saveSettings();
	 void onDefaultItemSetted(twoDModel::items::SolidGraphicItem *);
private:
	QMap<QString, QVariant> mSettingsManager;
	QFormLayout* mLayout;
	QHash<QString, QWidget*> mWidgets;
};
}
}
