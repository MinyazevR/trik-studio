#include "modelContext.h"
#include "visitorChooser.h"

using namespace twoDModel::model::details;

ModelContext::ModelContext(qReal::ErrorReporterInterface &errorReporter):
	NodeContext(errorReporter) {}

NodeContext* ModelContext::next(const QDomNode &element) const
{
	return visitorChooser<ModelContext>(element, mErrorReporter);
}

bool ModelContext::validate(const QDomNode &document) const
{
	if (document.isComment()) {
		return validateComment(document.toComment());
	}
	if (document.isDocumentType()) {
		return validateDocumentType(document.toDocumentType());
	}
	if (document.isProcessingInstruction()) {
		return validateProcessingInstruction(document.toProcessingInstruction());
	}
	if (!document.isDocument()) {
		return false;
	}
	return NodeContext::validate(document);
}

bool ModelContext::validateProcessingInstruction(const QDomProcessingInstruction &documentType) const
{
	Q_UNUSED(documentType)
	return true;
}

bool ModelContext::validateDocumentType(const QDomDocumentType &documentType) const
{
	Q_UNUSED(documentType)
	return true;
}

bool ModelContext::validateComment(const QDomComment &commentNode) const
{
	Q_UNUSED(commentNode)
	return true;
}

