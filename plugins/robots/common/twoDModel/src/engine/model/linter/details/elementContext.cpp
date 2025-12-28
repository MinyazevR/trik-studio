#include "elementContext.h"
#include "visitorChooser.h"

#include <qrgui/plugins/toolPluginInterface/usedInterfaces/errorReporterInterface.h>

using namespace twoDModel::model::details;

ElementContext::ElementContext(qReal::ErrorReporterInterface &errorReporter)
	: NodeContext(errorReporter) {};

bool ElementContext::validateText(const QDomText &text) const
{
	mErrorReporter.addError(QString("Text %1 not supported in this context, with column:line ")
				.arg(text.data(), text.columnNumber(), text.lineNumber()));
	return false;
}

bool ElementContext::validateCDATASection(const QDomCDATASection &cDATASection) const
{
	mErrorReporter.addError(QString("CDATASection not supported in this context, with column:line ")
				.arg(cDATASection.columnNumber(), cDATASection.lineNumber()));
	return false;
}

bool ElementContext::validateComment(const QDomComment &commentNode) const
{
	Q_UNUSED(commentNode)
	return true;
}

bool ElementContext::validateEntityReference(const QDomEntityReference &entityReference) const
{
	mErrorReporter.addError(QString("EntityReference not supported in this context, with column:line ")
				.arg(entityReference.columnNumber(), entityReference.lineNumber()));
	return false;
}

bool ElementContext::validate(const QDomNode &element) const
{
	if (element.isText()) {
		return validateText(element.toText());
	}
	if (element.isCDATASection()) {
		return validateCDATASection(element.toCDATASection());
	}
	if (element.isEntityReference()) {
		return validateEntityReference(element.toEntityReference());
	}
	if (element.isComment()) {
		return validateComment(element.toComment());
	}
	if (!element.isElement()) {
		return false;
	}

	const auto &attrs = element.attributes();
	// auto requiredAttrs = requiredAttibutes();
	for (int i = 0; i < attrs.count(); ++i) {
	    QDomAttr attr = attrs.item(i).toAttr();
	    validateAttr(attr);
	    // requiredAttrs.erase(attr.name());
	}

//	for (auto &&attrubuteName: requiredAttrs) {
//		mErrorReporter.addError(
//			QString("No required name attribute %1 is specified for the tag %2, with line:column")
//			.arg(attrubuteName, element.toElement().tagName(), QString(element.lineNumber()), QString(element.columnNumber())));
//	}

	return NodeContext::validate(element);
}

bool ElementContext::validateAttr(const QDomAttr &attr) const
{
	const auto &attributeName = attr.name();
	if (!isAllowedAttr(attr)) {
		mErrorReporter.addInformation(
			QString("Using an attribute with a name %1 is not allowed in this context, line:column %2:%3")
			.arg(attributeName, attr.lineNumber(), attr.columnNumber()));
	}
	return validateAttrValue(attr);
}
