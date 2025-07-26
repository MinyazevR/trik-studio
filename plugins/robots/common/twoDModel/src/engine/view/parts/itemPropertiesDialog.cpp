#include "itemPropertiesDialog.h"
#include <qrkernel/settingsManager.h>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <src/engine/items/solidGraphicItem.h>
#include <QDebug>
using namespace twoDModel::view;

ItemPropertiesDialog::ItemPropertiesDialog()
        : mLayout(new QFormLayout(this)) {

	QDialogButtonBox* buttons = new QDialogButtonBox(
	    QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
	    this
	);

	connect(buttons, &QDialogButtonBox::accepted, this, &ItemPropertiesDialog::saveSettings);
	connect(buttons, &QDialogButtonBox::rejected, this, &ItemPropertiesDialog::reject);

	mLayout->addRow(buttons);
}

void ItemPropertiesDialog::saveSettings() {

	qDebug() << "SAVE SEttINGS";
	for (auto it = mWidgets.begin(); it != mWidgets.end(); ++it) {
		const QString& key = it.key();

	    QWidget* widget = it.value();

	    if (auto spinBox = qobject_cast<QSpinBox*>(widget)) {
		mSettingsManager[key] = spinBox->value();
	    }
	    else if (auto doubleSpinBox = qobject_cast<QDoubleSpinBox*>(widget)) {
		mSettingsManager[key] = doubleSpinBox->value();
	    } else if (auto checkBox = qobject_cast<QCheckBox*>(widget)) {
		    mSettingsManager[key] = checkBox->isChecked();
	    } else if (auto lineEdit = qobject_cast<QLineEdit*>(widget)) {
		    mSettingsManager[key] = lineEdit->text();
	    }
	}

	accept();
}

void ItemPropertiesDialog::reject() {
    // Явно восстанавливаем старые значения в виджетах (опционально)
//    mRestitutionSpin->setValue(mTmpRestitution);
    QDialog::reject();
}

void ItemPropertiesDialog::onDefaultItemSetted(twoDModel::items::SolidGraphicItem *item) {
	auto spin = new QDoubleSpinBox(this);
	auto restitutionValue = item->restitution();
	spin->setValue(restitutionValue);
	mLayout->addRow(QStringLiteral("restituition") + ":", spin);
}
