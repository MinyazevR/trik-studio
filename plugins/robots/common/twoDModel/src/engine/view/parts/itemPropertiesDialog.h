#pragma once
#include <QDialog>
#include <qrkernel/settingsManager.h>
#include <QFormLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>

namespace twoDModel {
namespace items {
        class SolidGraphicItem;
	class SolidItem;
}

namespace view {

class ItemPropertiesDialog: public QDialog
{
	Q_OBJECT
public:
	ItemPropertiesDialog();
	void setDefaultValues(QMap<QString, QVariant> &defaultValues);
	void reject() override;
	QMap<QString, QVariant> &currentSettings();

public Q_SLOTS:
	 void saveSettings();
	 void onDefaultItemSetted(twoDModel::items::SolidItem *);
private:
	QMap<QString, QVariant> mCurrentValues;
	QMap<QString, QDoubleSpinBox *> mDoubleSpinWidgets;
	QVBoxLayout *mVBoxLayout;
	QFormLayout *mFormLayout;
};
}
}
