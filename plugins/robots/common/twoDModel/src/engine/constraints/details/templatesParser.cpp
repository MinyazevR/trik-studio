#include "templatesParser.h"
#include "template.h"
#include <unordered_set>
#include <QRegularExpression>
#include <stack>
#include <QDebug>

using namespace twoDModel::constraints::details;

QDomElement TemplatesParser::processTemplate(const QDomElement &elements)
{
	if (elements.tagName().toLower() != "use") {
		return elements;
	}

	auto &&templateName = elements.attribute("template");
	if (templateName.isEmpty()) {
		substituteError(QObject::tr("The &lt;use&gt; tag must contain a \"template\" attribute"));
		return {};
	}

	auto it = mTemplates.find(templateName);
	if (it == mTemplates.end()) {
		substituteError(QObject::tr("The &lt;use&gt; tag contains a \"template\" attribute that is not the name of a declared template"));
		return {};
	}

	QHash<QString, QString> paramsForReplace;
	for (QDomElement element = elements.firstChildElement()
		; !element.isNull()
		; element = element.nextSiblingElement())
	{
		if (element.tagName().toLower() != "with") {
			substituteError(QObject::tr("The &lt;use&gt; tag can only contain a child tag &lt;with&gt;"));
			return {};
		}

		auto &&param = element.attribute("param");
		auto &&value = element.attribute("value");
		if (param.isEmpty() || value.isEmpty()) {
			substituteError(QObject::tr("The \"param\" and \"value\" attributes of the &lt;with&gt; tag must not be empty"));
			return {};
		}
		paramsForReplace.insert(param, value);
	}

	auto &&xmlTemplate = it->second;
	auto &&stringResult = xmlTemplate.proccess(paramsForReplace);
	auto &&wrappedXml = QString("<root>%1</root>").arg(stringResult);
	QDomDocument result;
	result.setContent(wrappedXml);
	return result.documentElement();
}

bool TemplatesParser::transform(const QDomElement& constraintsXml) {
	std::stack<QDomElement> elementStack;

	auto&& child = constraintsXml.firstChildElement();
	while (!child.isNull()) {
		elementStack.push(child);
		child = child.nextSiblingElement();
	}

	while (!elementStack.empty()) {
		auto&& current = elementStack.top();
		elementStack.pop();

		if (current.tagName() != "use") {
			auto&& child = current.firstChildElement();
			while (!child.isNull()) {
				elementStack.push(child);
				child = child.nextSiblingElement();
			}
			continue;
		}

		auto&& replacement = processTemplate(current);
		auto&& parent = current.parentNode().toElement();

		if (parent.isNull()) {
			continue;
		}

		auto&& nextSibling = current.nextSibling();

		if (replacement.isNull()) {
			parent.removeChild(current);
			continue;
		}

		parent.removeChild(current);

		auto&& replChild = replacement.firstChildElement();
		std::vector<QDomElement> newlyInserted;

		while (!replChild.isNull()) {
			auto&& cloned = replChild.cloneNode(true).toElement();

			if (nextSibling.isNull()) {
				parent.appendChild(cloned);
			} else {
				parent.insertBefore(cloned, nextSibling);
			}

			newlyInserted.push_back(cloned);
			replChild = replChild.nextSiblingElement();
		}

		for (auto it = newlyInserted.rbegin(); it != newlyInserted.rend(); ++it) {
			elementStack.push(*it);
		}
	}
	return true;
}

bool TemplatesParser::parse(const QDomElement &templatesXml)
{
	clear();
	auto firstChildElement = templatesXml.firstChildElement();

	while (!firstChildElement.isNull()) {

		if (firstChildElement.tagName().toLower() != "template") {
			parseError(QObject::tr("the &lt;templates&gt; tag can only contain the &lt;template&gt; tag as a child tag"));
			return false;
		}

		if (!parseTemplate(firstChildElement)) {
			return false;
		}

		firstChildElement = firstChildElement.nextSiblingElement();
	}

	return true;
}

bool TemplatesParser::parseTemplate(const QDomElement &templateElement)
{
	auto &&templateName = templateElement.attribute("name", "");

	if (templateName.isEmpty()) {
		parseError(QObject::tr("The &lt;template&gt; tag was provided, but the required \"name\" attribute was missing."));
		return false;
	}

	auto &&it = mTemplates.find(templateName);

	if (it != mTemplates.end()) {
		parseError(QObject::tr("Redefinition a template %1 that already exists").arg(it->first));
		return false;
	}

	auto &&params = templateElement.firstChildElement("params");
	std::unordered_set<QString> templateParams;

	if (!params.isNull()) {
		auto &&firstParamDecl = params.firstChildElement("param");
		while (!firstParamDecl.isNull()) {
			if (firstParamDecl.tagName() != "param") {
				parseError(QObject::tr(
					"the &lt;params&gt; tag can only contain the &lt;param&gt; tag as a child tag"));
				return false;
			}

			auto &&paramName = firstParamDecl.attribute("name");

			if (paramName.isEmpty()) {
				parseError(QObject::tr(
					"The &lt;param&gt; tag was provided, but the required \"name\" attribute was missing."));
				return false;
			}

			templateParams.emplace(std::move(paramName));
			firstParamDecl = firstParamDecl.nextSiblingElement();
		}
	}

	auto &&contentDecl = templateElement.firstChildElement("content");

	if (contentDecl.isNull()) {
		parseError(QObject::tr(
			"The &lt;template&gt; tag was provided, but the required child tag &lt;content&gt; was missing."));
		return false;
	}

	auto &&content = contentDecl.firstChild();
	if(!content.isCDATASection()) {
		parseError(QObject::tr(
			"Currently, this method of setting &lt;content&gt; tag for the template element is not supported."));
		return false;
	}

	auto &&text = content.toCDATASection().data();
	static const QRegularExpression paramRegex("#\\{([^}]+)\\}");
	auto offset = 0;
	QRegularExpressionMatch match;

	auto &&xmlTemplate = XmlTemplate(templateName, text);

	qDebug() << "templateName" << templateName << text;
	while ((match = paramRegex.match(text, offset)).hasMatch()) {
		auto &&fullMatch = match.captured(0);
		auto &&paramName = match.captured(1);

		auto &&paramIt = templateParams.find(paramName);

		if (paramIt == templateParams.end()) {
			auto &&startPos = match.capturedStart(0);
			parseError(QObject::tr(
				"When defining the template,"
				" the syntax %1 was used to substitute an"
				" offset %2 for an undeclared parameter %3.").arg(fullMatch, QString(startPos), paramName));

		}

		xmlTemplate.emplaceParam(match.capturedStart(), fullMatch.length(), paramName);
		offset = match.capturedEnd();
	}

	mTemplates.emplace(templateName, std::move(xmlTemplate));
	return true;
}

void TemplatesParser::clear()
{
	mTemplates.clear();
	mParsingErrors.clear();
	mSubstituionErrors.clear();
}

void TemplatesParser::parseError(const QString& message)
{
	mParsingErrors << message;
}

void TemplatesParser::substituteError(const QString& message)
{
	mSubstituionErrors << message;
}

QStringList TemplatesParser::parsingErrors() const
{
	return mParsingErrors;
}

QStringList TemplatesParser::substituionErrors() const
{
	return mSubstituionErrors;
}
