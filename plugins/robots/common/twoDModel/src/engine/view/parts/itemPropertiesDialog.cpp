#include "itemPropertiesDialog.h"
#include <qrkernel/settingsManager.h>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <src/engine/items/solidGraphicItem.h>
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>

using namespace twoDModel::view;

ItemPropertiesDialog::ItemPropertiesDialog()
        : mVBoxLayout(new QVBoxLayout(this))
        , mFormLayout(new QFormLayout()) {

	this->setWindowTitle("Item properties");
	mVBoxLayout->addLayout(mFormLayout);
	mVBoxLayout->addStretch();

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	QPushButton *saveButton = new QPushButton("Save");
	QPushButton *cancelButton = new QPushButton("Cancel");
	buttonLayout->addWidget(saveButton);
	buttonLayout->addWidget(cancelButton);
	mVBoxLayout->addLayout(buttonLayout);
	connect(saveButton, &QPushButton::pressed, this, &ItemPropertiesDialog::saveSettings);
	connect(cancelButton, &QPushButton::pressed, this, &ItemPropertiesDialog::reject);
}

QMap<QString, QVariant> &ItemPropertiesDialog::currentSettings() {
	return mCurrentValues;
}

void ItemPropertiesDialog::saveSettings() {

	for (auto &&key: mDoubleSpinWidgets.keys()) {
		auto spinWidget = mDoubleSpinWidgets[key];
		mCurrentValues[key] = spinWidget->value();
	}

	accept();
}

void ItemPropertiesDialog::reject() {

	for (auto &&key: mDoubleSpinWidgets.keys()) {
		auto prevValue = mCurrentValues[key];
		mDoubleSpinWidgets[key]->setValue(prevValue.toDouble());
	}

	QDialog::reject();
}

void ItemPropertiesDialog::onDefaultItemSetted(twoDModel::items::SolidItem *item) {

	auto defaultParams = item->defaultParams();
	for(auto &&key: defaultParams.keys()) {
		auto value = defaultParams[key];
		if (value.canConvert<double>()) {
			mDoubleSpinWidgets[key] = new QDoubleSpinBox(this);
			mDoubleSpinWidgets[key]->setSingleStep(0.01);
			mDoubleSpinWidgets[key]->setDecimals(4);
			mDoubleSpinWidgets[key]->setValue(value.toDouble());
			mCurrentValues[key] = value;
			mFormLayout->addRow(key + ":", mDoubleSpinWidgets[key]);
		}
	}
}
