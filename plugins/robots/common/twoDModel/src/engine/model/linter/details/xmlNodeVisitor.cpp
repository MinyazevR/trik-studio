#include "xmlNodeVisitor.h"
#include "elementContext.h"
#include <qrgui/plugins/toolPluginInterface/usedInterfaces/errorReporterInterface.h>

using namespace twoDModel::model::details;

XmlNodeVistor::XmlNodeVistor(qReal::ErrorReporterInterface &errorReporter, ElementContext &context)
	: mErrorReporter(errorReporter)
	, mEntityContext(context) {}

void XmlNodeVistor::visit(const QDomNode &node) const {
	if (node.isElement()) {
		visitElementNode(node.toElement());
	}
	if (node.isAttr()) {
		visitAttributeNode(node.toAttr());
	}
	if (node.isText()) {
		visitTextNode(node.toText());
	}
	if (node.isCDATASection()) {
		visitCDATASectionNode(node.toCDATASection());
	}
	if (node.isEntityReference()) {
		visitEntityReferenceNode(node.toEntityReference());
	}
	if (node.isEntity()){
		visitEntityNode(node.toEntity());
	}
	if (node.isProcessingInstruction()) {
		visitProcessingInstructionNode(node.toProcessingInstruction());
	}
	if (node.isComment()) {
		visitCommentNode(node.toComment());
	}
	if (node.isDocument()) {
		visitDocumentNode(node.toDocument());
	}
	if (node.isDocumentType()) {
		visitDocumentTypeNode(node.toDocumentType());
	}
	if (node.isDocumentFragment()) {
		visitDocumentFragmentNode(node.toDocumentFragment());
	}
	if (node.isNotation()) {
		visitNotationNode(node.toNotation());
	}
}

void XmlNodeVistor::visitElementNode(const QDomElement &element) const
{
	mErrorReporter.addError(QString("Tag %1 not supported in this context, with column:line ")
				.arg(element.tagName(), element.columnNumber(), element.lineNumber()));
}

void XmlNodeVistor::visitAttributeNode(const QDomAttr &attr) const
{
	mEntityContext.validateAttr(attr);
}

void XmlNodeVistor::visitTextNode(const QDomText &text) const
{
	mErrorReporter.addError(QString("Text %1 not supported in this context, with column:line ")
				.arg(text.data(), text.columnNumber(), text.lineNumber()));

}

void XmlNodeVistor::visitCDATASectionNode(const QDomCDATASection &cDATASection) const
{
	mErrorReporter.addError(QString("CDATASection not supported in this context, with column:line ")
				.arg(cDATASection.columnNumber(), cDATASection.lineNumber()));
}

void XmlNodeVistor::visitEntityReferenceNode(const QDomEntityReference &entityReference) const
{
	mErrorReporter.addError(QString("EntityReference not supported in this context, with column:line ")
				.arg(entityReference.columnNumber(), entityReference.lineNumber()));
}

void XmlNodeVistor::visitEntityNode(const QDomEntity &entity) const
{
	mErrorReporter.addError(QString("Entity not supported in this context, with column:line ")
				.arg(entity.columnNumber(), entity.lineNumber()));
}

void XmlNodeVistor::visitProcessingInstructionNode(const QDomProcessingInstruction &processingInstruction) const
{
	mErrorReporter.addError(QString("ProcessingInstruction not supported in this context, with column:line ")
				.arg(processingInstruction.columnNumber(), processingInstruction.lineNumber()));
}

void XmlNodeVistor::visitCommentNode(const QDomComment &commentNode) const
{
	Q_UNUSED(commentNode)
}

void XmlNodeVistor::visitDocumentNode(const QDomDocument &documentNode) const
{
	mErrorReporter.addError(QString("Document not supported in this context, with column:line ")
				.arg(documentNode.columnNumber(), documentNode.lineNumber()));
}

void XmlNodeVistor::visitDocumentTypeNode(const QDomDocumentType &documentType) const
{
	mErrorReporter.addError(QString("DocumentType not supported in this context, with column:line ")
				.arg(documentType.columnNumber(), documentType.lineNumber()));
}

void XmlNodeVistor::visitDocumentFragmentNode(const QDomDocumentFragment &documentFragment) const
{
	mErrorReporter.addError(QString("DocumentFragment not supported in this context, with column:line ")
				.arg(documentFragment.columnNumber(), documentFragment.lineNumber()));
}

void XmlNodeVistor::visitNotationNode(const QDomNotation &notation) const
{
	mErrorReporter.addError(QString("Notation not supported in this context, with column:line ")
				.arg(notation.columnNumber(), notation.lineNumber()));
}
