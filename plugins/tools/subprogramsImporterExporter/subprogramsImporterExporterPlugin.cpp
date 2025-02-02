/* Copyright 2018 CyberTech Labs Ltd.
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

#include "subprogramsImporterExporterPlugin.h"

#include <QtCore/QStandardPaths>
#include <QtCore/QEventLoop>
#include <QtCore/QString>
#include <QMessageBox>

#include <algorithm>

#include <widgets/qRealFileDialog.h>
#include <qrgui/plugins/toolPluginInterface/usedInterfaces/errorReporterInterface.h>

#include "subprogramsCollectionDialog.h"


const QString PROGRAM_DIRECTORY = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
const QString SUBPROGRAMS_COLLECTION_DIRECTORY = "subprogramsCollection";
const QMap<QString, QString> KIT_ID_TO_FRIENDLY_NAME = {
		{"trikV62Kit", "TRIK"}, {"ev3Kit", "Lego EV3"}, {"nxtKit", "Lego NXT"}};

using namespace subprogramsImporterExporter;

SubprogramsImporterExporterPlugin::SubprogramsImporterExporterPlugin()
	: mMenu("&" + tr("Subprograms"))
	, mFirstSeparatorAction(nullptr)
	, mSecondSeparatorAction(nullptr)
	, mImportToProjectAction("&" + tr("Import from file..."), &mMenu)
	, mExportAction("&" + tr("Export to file..."), &mMenu)
	, mSaveToCollection("&" + tr("Save to collection..."), &mMenu)
	, mImportFromCollection("&" + tr("Load from collection"), &mMenu)
	, mClearCollection("&" + tr("Clear collection"), &mMenu)
	, mRepo(nullptr)
	, mMainWindowInterpretersInterface(nullptr)
	, mGraphicalModel(nullptr)
	, mLogicalModel(nullptr)
	, mProjectManager(nullptr)
{
	connect(&mExportAction, &QAction::triggered, this, &SubprogramsImporterExporterPlugin::exportToFile);
	connect(&mImportToProjectAction, &QAction::triggered, this, &SubprogramsImporterExporterPlugin::importToProject);
	connect(&mSaveToCollection, &QAction::triggered, this
			, &SubprogramsImporterExporterPlugin::saveToCollectionTriggered);
	connect(&mImportFromCollection, &QAction::triggered, this
			, &SubprogramsImporterExporterPlugin::importFromCollectionTriggered);
	connect(&mClearCollection, &QAction::triggered, this
			, &SubprogramsImporterExporterPlugin::clearCollectionTriggered);
}

SubprogramsImporterExporterPlugin::~SubprogramsImporterExporterPlugin()
{
}

QList<qReal::ActionInfo> SubprogramsImporterExporterPlugin::actions()
{
	mFirstSeparatorAction.setSeparator(true);
	mSecondSeparatorAction.setSeparator(true);
	mMenu.addAction(&mExportAction);
	mMenu.addAction(&mImportToProjectAction);
	mMenu.addSeparator();
	mMenu.addAction(&mSaveToCollection);
	mMenu.addAction(&mImportFromCollection);
	mMenu.addSeparator();
	mMenu.addAction(&mClearCollection);
	return { qReal::ActionInfo(&mFirstSeparatorAction, "", "tools")
			, qReal::ActionInfo(&mMenu, "tools")
			, qReal::ActionInfo(&mSecondSeparatorAction, "", "tools") };
}

void SubprogramsImporterExporterPlugin::init(qReal::PluginConfigurator const &configurator)
{
	mRepo = &configurator.repoControlInterface();
	mMainWindowInterpretersInterface = &configurator.mainWindowInterpretersInterface();
	mGraphicalModel = &configurator.graphicalModelApi();
	mLogicalModel = &configurator.logicalModelApi();
	mProjectManager = &configurator.projectManager();
}

void SubprogramsImporterExporterPlugin::exportToFile() const
{
	if (not checkOpenedProject() || not checkSubprogramsForUniqueNames()) {
		return;
	}

	const QString fileLocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
	QString fileName = utils::QRealFileDialog::getSaveFileName("ExportSubprograms"
			, mMainWindowInterpretersInterface->currentTab()
			, tr("Select subprograms file (name for new one)")
			, fileLocation, tr("QReal Save File(*.qrs)"));

	if (fileName.isEmpty()) {
		return;
	}

	if (!fileName.isEmpty() && !fileName.endsWith(".qrs", Qt::CaseInsensitive)) {
		fileName += ".qrs";
	}

	qReal::IdList subprograms = mLogicalModel->logicalRepoApi().elementsByType("SubprogramDiagram", true, false);
	QSet<QString> uniqueNames;
	QMap<QString, qReal::Id> nameToId;

	for (const qReal::Id &id : subprograms) {
		uniqueNames.insert(mGraphicalModel->name(id));
		nameToId[mGraphicalModel->name(id)] = id;
	}

	uniqueNames.remove("");
	if (uniqueNames.isEmpty()) {
		mMainWindowInterpretersInterface->errorReporter()->addInformation(tr("There are no subprograms"
				" in your project."));
		return;
	}

	nameToId.remove("");
	QHash<QString, qReal::IdList> toSave;
	toSave.insert(fileName, nameToId.values());

	mRepo->saveDiagramsById(toSave);
}

void SubprogramsImporterExporterPlugin::importToProject() const
{
	if (not checkOpenedProject() || not checkSubprogramsForUniqueNames()) {
		return;
	}

	const QString fileLocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
	QString fileName = utils::QRealFileDialog::getOpenFileName("ExportSubprograms"
			, mMainWindowInterpretersInterface->currentTab()
			, tr("Select subprograms file")
			, fileLocation, tr("QReal Save File(*.qrs)"));

	if (fileName.isEmpty()) {
		return;
	}

	const auto &openedDiagrams = mMainWindowInterpretersInterface->openedDiagrams();
	QHash<QString, QVariant> oldMeta;
	for (const auto &metaKey : mLogicalModel->logicalRepoApi().metaInformationKeys()) {
		oldMeta[metaKey] = mLogicalModel->logicalRepoApi().metaInformation(metaKey);
	}
	mRepo->importFromDisk(fileName);
	mMainWindowInterpretersInterface->reinitModels();
	for (const auto &diagram : openedDiagrams) {
		mMainWindowInterpretersInterface->activateItemOrDiagram(diagram);
	}
	mLogicalModel->mutableLogicalRepoApi().clearMetaInformation();
	for (const auto &metaKey : oldMeta.keys()) {
		mLogicalModel->mutableLogicalRepoApi().setMetaInformation(metaKey, oldMeta[metaKey]);
	}
	mProjectManager->afterOpen(mRepo->workingFile());
	mProjectManager->setUnsavedIndicator(true);

	checkSubprogramsForUniqueNames();
}

void SubprogramsImporterExporterPlugin::saveToCollectionTriggered() const
{
	if (not checkOpenedProject() || not checkSubprogramsForUniqueNames()) {
		return;
	}

	const QString currentPath = QDir::currentPath();

	// Directories are depend on kit
	const QString path = PROGRAM_DIRECTORY + QDir::separator() + SUBPROGRAMS_COLLECTION_DIRECTORY;
	if (not QDir(path).exists()) {
		QDir().mkdir(path);
	}

	QDir collectionDirectory(path);
	const QString kit = mLogicalModel->logicalRepoApi().metaInformation("lastKitId").toString();
	if (not collectionDirectory.cd(kit)) {
		collectionDirectory.mkdir(kit);
		collectionDirectory.cd(kit);
	}

	qReal::IdList subprograms = mLogicalModel->logicalRepoApi().elementsByType("SubprogramDiagram", true, false);
	QSet<QString> uniqueNames;

	for (const qReal::Id &id : subprograms) {
		uniqueNames.insert(mGraphicalModel->name(id));
	}

	uniqueNames.remove("");
	if (uniqueNames.isEmpty()) {
		mMainWindowInterpretersInterface->errorReporter()->addInformation(tr("There are not subprograms"
				" in your project"));
		QDir().cd(currentPath);
		return;
	}

	QMap<QString, qReal::Id> nameToId;
	for (const qReal::Id &id : subprograms) {
		nameToId[mGraphicalModel->name(id)] = id;
	}

	QMap<QString, bool> map = markLeftExistedInRight(uniqueNames.toList(), currentlySavedSubprograms());
	SubprogramsCollectionDialog dialog(map);
	dialog.showWarningLabel(true);
	dialog.exec();

	if (dialog.result() == QDialog::Accepted) {
		// todo: save to independent project.... dependences????? another subprograms... needs to be copied if it was
		// inside in selected subprogram
		QHash<QString, qReal::IdList> toSave;
		for (const auto &key : map.keys()) {
			if (map[key]) {
				qReal::IdList innerSPs = { nameToId[key] };
				innerSubprograms(nameToId[key], innerSPs);
				toSave.insert(collectionDirectory.path() + QDir::separator() + key + ".qrs", innerSPs);
			}
		}

		mRepo->saveDiagramsById(toSave);
	}
}

void SubprogramsImporterExporterPlugin::importFromCollectionTriggered() const
{
	if (not checkOpenedProject() || not checkSubprogramsForUniqueNames()) {
		return;
	}

	QStringList currentlySavedSPs= currentlySavedSubprograms();
	if (currentlySavedSPs.isEmpty()) {
		QString kitId = mLogicalModel->logicalRepoApi().metaInformation("lastKitId").toString();
		mMainWindowInterpretersInterface->errorReporter()->addInformation(tr("There are no subprograms"
				" in your collection for %1 robot.").arg(KIT_ID_TO_FRIENDLY_NAME.value(kitId, QString())));
		return;
	}

	QMap<QString, bool> map;
	for (auto const &str :  currentlySavedSPs){
		map[str] = false;
	}

	SubprogramsCollectionDialog dialog(map);
	dialog.exec();
	if (dialog.result() == QDialog::Accepted) {
		QHash<QString, QVariant> oldMeta;
		for (auto const &metaKey : mLogicalModel->logicalRepoApi().metaInformationKeys()) {
			oldMeta[metaKey] = mLogicalModel->logicalRepoApi().metaInformation(metaKey);
		}
		const auto &openedDiagrams = mMainWindowInterpretersInterface->openedDiagrams();
		const QString directoryPath = PROGRAM_DIRECTORY + QDir::separator() + SUBPROGRAMS_COLLECTION_DIRECTORY
				+ QDir::separator() + mLogicalModel->logicalRepoApi().metaInformation("lastKitId").toString()
				+ QDir::separator();
		for (auto const &key : map.keys()) {
			if (map[key]) {
				mRepo->importFromDisk(directoryPath + key + ".qrs");
			}
		}

		mMainWindowInterpretersInterface->reinitModels();
		for (const auto &diagram : openedDiagrams) {
			mMainWindowInterpretersInterface->activateItemOrDiagram(diagram);
		}
		mLogicalModel->mutableLogicalRepoApi().clearMetaInformation();
		for (const auto &metaKey : oldMeta.keys()) {
			mLogicalModel->mutableLogicalRepoApi().setMetaInformation(metaKey, oldMeta[metaKey]);
		}
		Q_EMIT mProjectManager->afterOpen(mRepo->workingFile());
		mProjectManager->setUnsavedIndicator(true);

		checkSubprogramsForUniqueNames();
	}
}

void SubprogramsImporterExporterPlugin::clearCollectionTriggered() const
{
	QDir dir = PROGRAM_DIRECTORY + QDir::separator() + SUBPROGRAMS_COLLECTION_DIRECTORY;
	if (dir.exists()
			&& QMessageBox::No != QMessageBox::warning(nullptr
													   , tr("Clear the collection")
													   , tr("Remove all subprograms for all kits from the collection?")
													   , QMessageBox::Yes | QMessageBox::No
													   , QMessageBox::No)
			) {
		dir.removeRecursively();
	}
}

bool SubprogramsImporterExporterPlugin::checkOpenedProject() const
{
	if (not mProjectManager->somethingOpened()) {
		mMainWindowInterpretersInterface->errorReporter()->addError(tr("There is no opened project"));
		return false;
	}

	return true;
}

bool SubprogramsImporterExporterPlugin::checkSubprogramsForUniqueNames() const
{
	qReal::IdList subprograms = mLogicalModel->logicalRepoApi().elementsByType("SubprogramDiagram", true, false);
	QMap<qReal::Id, QString> idToName;

	for (const qReal::Id &id : subprograms) {
		QString name = mGraphicalModel->name(id);
		if (not name.isNull()) {
			idToName[id] = name;
		}
	}

	if (idToName.size() == idToName.values().toSet().size()) {
		return true;
	} else {
		mMainWindowInterpretersInterface->errorReporter()->addInformation(tr("There are different subprograms"
				" with the same name in your project. Please make them unique."));
		return false;
	}
}

void SubprogramsImporterExporterPlugin::innerSubprograms(const qReal::Id &id, qReal::IdList &list) const
{
	qReal::IdList children = mGraphicalModel->graphicalRepoApi().children(id);
	for (auto const &chId : children) {
		qReal::Id logicalId = mGraphicalModel->graphicalRepoApi().logicalId(chId);
		qReal::Id outgoingExplosion = mLogicalModel->logicalRepoApi().outgoingExplosion(logicalId);
		if (outgoingExplosion.element() == "SubprogramDiagram") {
			qReal::IdList diagrams = mGraphicalModel->graphicalIdsByLogicalId(outgoingExplosion);
			if (!list.contains(diagrams.first())) {
				list.append(diagrams.first());
				innerSubprograms(diagrams.first(), list);
			}
		}
	}
}

QStringList SubprogramsImporterExporterPlugin::currentlySavedSubprograms() const
{
	const QString tmpPath = PROGRAM_DIRECTORY + QDir::separator() + SUBPROGRAMS_COLLECTION_DIRECTORY;
	const QString kit = mLogicalModel->logicalRepoApi().metaInformation("lastKitId").toString();
	const QString path = tmpPath + QDir::separator() + kit;

	QStringList list = QDir(path).entryList({ "*.qrs" });
	std::transform(list.begin(), list.end(), list.begin(), [](QString &str){ str.chop(4); return str; });

	return list;
}

QMap<QString, bool> SubprogramsImporterExporterPlugin::markLeftExistedInRight(const QStringList &left
		, const QStringList &right) const
{
	QMap<QString, bool> answer;
	for (auto const &s : left) {
		answer[s] = right.contains(s);
	}

	return answer;
}
