/* Copyright 2007-2016 QReal Research Group, Yurii Litvinov
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

#include "graphicType.h"

#include "property.h"
#include "qrmc/diagram.h"
#include "qrmc/utils/nameNormalizer.h"
#include "shape.h"

#include <QtCore/QDebug>

using namespace qrmc;
using namespace qReal;

GraphicType::ContainerProperties::ContainerProperties()
	: isSortContainer(false)
	, sizeOfForestalling(0)
	, sizeOfChildrenForestalling(0)
	, isChildrenMovable(true)
	, isMinimizingToChildren(false)
	, isClass(false)
	, isMaximizingChildren(false)
{}

GraphicType::ResolvingHelper::ResolvingHelper(bool &resolvingFlag)
	: mResolvingFlag(resolvingFlag)
{
	mResolvingFlag = true;
}

GraphicType::GraphicType(
		const Diagram &diagram
		, const qrRepo::LogicalRepoApi &api
		, const qReal::Id &id
		, const QString &targetDirectory)
	: Type(false, diagram, api, id)
	, mShape("", targetDirectory)
	, mResolving(false)
	, mTargetDirectory(targetDirectory)
{
}

GraphicType::~GraphicType()
{
}

bool GraphicType::init(const QString &context)
{
	Type::init(context);
	mDescription = mApi.stringProperty(mId, "description");

	mIsVisible = false;
	if (mApi.hasProperty(mId, "shape")) {
		mIsVisible = !mApi.stringProperty(mId, "shape").isEmpty();
	}

	if (mApi.hasProperty(mId, "RequestBody")) {
		mIsVisible = !mApi.stringProperty(mId, "RequestBody").isEmpty();
	}

	const IdList outLinks = mApi.outgoingLinks(mId);
	for (const Id &outLink : outLinks) {
		if (outLink.element() == "Container") {
			const Id elementId = mApi.to(outLink);
			const QString typeName = mApi.name(elementId);
			mContains << typeName.split(",", QString::SkipEmptyParts);
		} else if (outLink.element() == "Inheritance") {
			const Id &elementId = mApi.to(outLink);
			const QString &childName = mApi.name(elementId);
			if (!mChildren.contains(childName)) {
				mChildren << childName.split(",", QString::SkipEmptyParts);
			}
		}
	}

	const IdList inLinks = mApi.incomingLinks(mId);
	for (const Id &inLink : inLinks) {
		if (inLink.element() == "Inheritance") {
			const Id elementId = mApi.from(inLink);
			const QString parentName = mApi.name(elementId);
			if (!mParents.contains(parentName)) {
				mParents << parentName.split(",", QString::SkipEmptyParts);
			}
		}
	}

	for (const Id &id : mApi.children(mId)) {
		if (!mApi.isLogicalElement(id)) {
			continue;
		}

		if (id.element() == metaEntityAttribute) {
			Property *property = new Property(mApi, id);
			if (!property->init()) {
				delete property;
				continue;
			}

			if (!addProperty(property)) {
				return false;
			}
		} else if (id.element() == metaEntityConnection) {
			mConnections << mApi.stringProperty(id, "type").section("::", -1);
		} else if (id.element() == metaEntityUsage) {
			mUsages << mApi.stringProperty(id, "type").section("::", -1);
		} else if (id.element() == metaEntityContextMenuField) {
			mContextMenuItems << mApi.name(id);
		}
	}

	initPossibleEdges();
	initShape();
	return true;
}

bool GraphicType::initPossibleEdges()
{
	const IdList children = mApi.children(mId);
	for (const Id &id : children) {
		if (!mApi.isLogicalElement(id) || id.element() != metaEntityPossibleEdge) {
			continue;
		}

		const QString beginName = mApi.stringProperty(id, "beginName");
		const QString endName = mApi.stringProperty(id, "endName");
		const QString directedField = mApi.stringProperty(id, "directed");
		bool directed = false;

		if (beginName.isEmpty() || endName.isEmpty() || ((directedField != "true") && (directedField != "false"))) {
				qDebug() << "Error: one of attributes is incorrect " <<
					"(perhaps, \"beginName\" or \"emptyName\" is empty or " <<
					"\"directed\" isn't \"true\" or \"false\".')" << qualifiedName();
				return false;
		}

		directed = (directedField == "true");
		const QString edgeName = NameNormalizer::normalize(qualifiedName());
		QPair<QPair<QString, QString>, QPair<bool, QString>> possibleEdge(
				qMakePair(beginName,endName),qMakePair(directed,edgeName));

		if (!mPossibleEdges.contains(possibleEdge)) {
				mPossibleEdges.append(possibleEdge);
		} else {
			// FIXME: ignoring for now
				//qDebug() << "ERROR: this edge is already in list " << qualifiedName();
//				return false;
		}
	}

	return true;
}

void GraphicType::initShape()
{
	if (mApi.hasProperty(mId, "shape")) {
		const QString shape = mApi.stringProperty(mId, "shape");
		if (shape.isEmpty()) {
			return;
		}

		mShape.init(shape);
	}
}

GraphicType::ResolvingHelper::~ResolvingHelper()
{
	mResolvingFlag = false;
}

void GraphicType::copyFields(Type *type) const
{
	Type::copyFields(type);
	/// @todo: Not good, requires non-const fields.
	auto graphicType = dynamic_cast<GraphicType*>(type);
	graphicType->mParents = mParents;
	graphicType->mIsVisible = mIsVisible;
	graphicType->mContainerProperties = mContainerProperties;
	graphicType->mContains = mContains;
	graphicType->mShape = mShape;
	graphicType->mTargetDirectory = mTargetDirectory;
}

bool GraphicType::resolve()
{
	if (mResolvingFinished) {
		return true;
	}

	ResolvingHelper helper(mResolving);
	mParents.removeDuplicates();
	for (const QString &parentName : mParents) {
		// searching for parents in native context. if it was imported, references will remain valid
		const QString qualifiedParentName = parentName.contains("::") ? parentName : nativeContext() + "::"
				+ parentName;

		Type *parent = mDiagram->findType(qualifiedParentName);
		if (!parent) {
			// didn't find in local context, try global
			parent = mDiagram->findType(parentName);
			if (!parent) {
				qDebug() << "ERROR: can't find parent" << parentName << "for" << qualifiedName();
				return false;
			}
		}

		if (parent->isResolving()) {
			qDebug() << "ERROR: circular inheritance between" << parentName << "and" << qualifiedName();
			return false;
		}

		if (!parent->isResolved()) {
			if (!parent->resolve()) {
				return false;
			}
		}

		for (const Property * const property : parent->properties().values()) {
			if (!addProperty(property->clone())) {
				return false;
			}
		}

		GraphicType* gParent = dynamic_cast<GraphicType*>(parent);
		if (gParent) {
			for (const PossibleEdge &pEdge : gParent->mPossibleEdges) {
				mPossibleEdges.append(qMakePair(pEdge.first,qMakePair(pEdge.second.first,name())));
			}
		}
	}

	mResolvingFinished = true;
	return true;
}

bool GraphicType::isResolving() const
{
	return mResolving;
}

bool GraphicType::addProperty(Property *property)
{
	const QString propertyName = property->name();
	if (mProperties.contains(propertyName)) {
		if (mProperties[propertyName] != property && *mProperties[propertyName] != *property) {
			qDebug() << "ERROR: property" << propertyName << "duplicated with different attributes";
			delete property;
			return false;
		}
	} else {
		mProperties[propertyName] = property;
	}

	return true;
}

bool GraphicType::isGraphicalType() const
{
	return mIsVisible;
}

QString GraphicType::generateProperties(const QString &lineTemplate) const
{
	if (!mIsVisible) {
		return "";
	}

	QString propertiesString;
	for (const Property * const property : mProperties) {
		// hack: don't generate pre-defined properties
		if (property->name() == "fromPort" || property->name() == "toPort"
			|| property->name() == "from" || property->name() == "to"
			|| property->name() == "name")
		{
			qDebug() << "ERROR: predefined property" << property->name()
				<< "shall not appear in metamodels, ignored";
			continue;
		}

		QString tmp = property->generatePropertyLine(lineTemplate) + endline;
		propertiesString += tmp.replace(elementNameTag, name());
	}

	return propertiesString;
}

QString GraphicType::generatePropertyDefaults(const QString &lineTemplate) const
{
	if (!mIsVisible) {
		return "";
	}

	QString defaultsString;
	for (const Property * const property : mProperties) {
		QString tmp = property->generateDefaultsLine(lineTemplate);
		if (!tmp.isEmpty()) {
			defaultsString += tmp.replace(elementNameTag, name()) + endline;
		}
	}

	return defaultsString;
}

QString GraphicType::generatePropertyDisplayedNames(const QString &lineTemplate) const
{
	if (!mIsVisible) {
		return "";
	}

	QString displayedNamesString;
	for (const Property * const property : mProperties) {
		QString tmp = property->generateDisplayedNameLine(lineTemplate);
		if (!tmp.isEmpty()) {
			displayedNamesString += tmp.replace(elementNameTag, name()).replace(diagramNameTag, mContext) + endline;
		}
	}

	return displayedNamesString;
}

QString GraphicType::generateElementDescription(const QString &lineTemplate) const
{
	if (mDescription.isEmpty()) {
		return "";
	}

	QString displayedNamesString;
	QString temp = generateElementDescriptionLine(lineTemplate);
	if (!temp.isEmpty()) {
		displayedNamesString += temp.replace(elementNameTag, name()).replace(diagramNameTag, mContext) + endline;
	}

	return displayedNamesString;
}

QString GraphicType::generateElementDescriptionLine(const QString &lineTemplate) const
{
	QString result = lineTemplate;
	result.replace(descriptionTag, mDescription);
	return result;
}

QString GraphicType::generateReferenceProperties(const QString &lineTemplate) const
{
	if (!mIsVisible) {
		return "";
	}

	QString referencePropertiesString = lineTemplate;
	QString referencePropertiesList;
	for (const Property * const property : mProperties) {
		if (property->isReferenceProperty()) {
			referencePropertiesList = referencePropertiesList + " << "  + "\"" + property->name() + "\"";
		}
	}

	if (referencePropertiesList.isEmpty()) {
		referencePropertiesString
				.replace(referencePropertiesListTag, "*/}//")
				.replace(elementNameTag, name() + "\"){/*");
	} else {
		referencePropertiesString
				.replace(referencePropertiesListTag, referencePropertiesList)
				.replace(elementNameTag, name());
	}

	return referencePropertiesString;
}

QString GraphicType::generatePortTypes(const QString &lineTemplate) const
{
	QString portTypesString = lineTemplate;
	QString portTypesList;

	const QList<Port*> portTypes = this->mShape.getPorts();
	QSet<QString> portTypesSet;
	for (Port * const port : portTypes) {
		portTypesSet.insert(port->type());
	}

	if (!portTypesSet.empty()) {
		for (const QString &type : portTypesSet) {
			portTypesList = portTypesList + "\"" + type + "\"";
		}

		if (portTypesList.isEmpty()) {
			portTypesString.replace(portTypesListTag, "*/}//").replace(elementNameTag, name());
		} else {
			portTypesString.replace(portTypesListTag, portTypesList).replace(elementNameTag, name());
		}
	} else {
		return "";
	}
	return portTypesString;
}

QString GraphicType::generatePropertyName(const QString &lineTemplate) const
{
	if (!mIsVisible) {
		return "";
	}

	QString propertyNameString = lineTemplate;
	QString propertyNameList ;

	for (const Property * const property: mProperties) {
		if (!property->isReferenceProperty()) {
			if (!propertyNameList.isEmpty()) {
				propertyNameList = propertyNameList + " << " + + "\"" + property->name() + "\"";
			} else {
				propertyNameList = propertyNameList + "\"" + property->name() + "\"";
			}
		}
	}

	if (propertyNameList.isEmpty()) {
		propertyNameString.replace(propertyNameListTag, "*/}//").replace(elementNameTag, name() + "\"){/*");
	} else {
		propertyNameString
				.replace(propertyNameListTag, propertyNameList + ";\n	}//")
				.replace(elementNameTag, name() + "\"){//");
	}

	return propertyNameString;
}

QString GraphicType::generateParents(const QString &lineTemplate) const
{
	QString parentsMapString;
	const QString diagramName = mContext + "::";
	QString parentName = qualifiedName().remove(diagramName);
	for (const QString &child : mChildren) {
		QString tmp = lineTemplate;
		parentsMapString += tmp
				.replace(parentNameTag, parentName)
				.replace(childNameTag, child)
				.replace(diagramNameTag, mContext)
				+ endline;
	}
	return parentsMapString;
}

QString GraphicType::generateContainers(const QString &lineTemplate) const
{
	if (!isGraphicalType() || mContains.isEmpty()) {
		return "";
	}

	QString containersList;
	QString line = lineTemplate;
	for (const QString &contains : mContains) {
		containersList += "<< \"" + contains + "\" ";
	}

	line.replace(containersListTag, containersList).replace(elementNameTag, name());
	return line;
}

QString GraphicType::generateConnections(const QString &lineTemplate) const
{
	if (!isGraphicalType() || mConnections.isEmpty()) {
		return "";
	}

	QString connectionsList;
	QString line = lineTemplate;
	for (const QString &connection : mConnections) {
		connectionsList += "<< \"" + connection + "\" ";
	}

	line.replace(connectionsListTag, connectionsList).replace(elementNameTag, name());
	return line;
}

QString GraphicType::generateUsages(const QString &lineTemplate) const
{
	if (!isGraphicalType() || mUsages.isEmpty()) {
		return "";
	}

	QString usagesList;
	QString line = lineTemplate;
	for (const QString &usage : mUsages) {
		usagesList += "<< \"" + usage + "\" ";
	}

	line.replace(usagesListTag, usagesList).replace(elementNameTag, name());
	return line;
}

QString GraphicType::generateEnums(const QString &lineTemplate) const
{
	Q_UNUSED(lineTemplate);
	return "";
}

QString GraphicType::generatePossibleEdges(const QString &lineTemplate) const
{
	if (mPossibleEdges.isEmpty()) {
		return "";
	}

	QString edgesList;
	QString line = lineTemplate;
	const char templ[] = "<< qMakePair(qMakePair(QString(\"%1\"),QString(\"%2\")),qMakePair(%3,QString(\"%4\"))) ";
	QString directed = "false";
	for (const PossibleEdge &edge : mPossibleEdges) {
		if (edge.second.first) {
			directed = "true";
		}
		edgesList += QString(templ).arg(edge.first.first, edge.first.second, directed, edge.second.second);
	}

	line.replace(possibleEdgesListTag, edgesList).replace(elementNameTag, name());
	return line;
}

QString GraphicType::targetDirectory() const
{
	return mTargetDirectory;
}
