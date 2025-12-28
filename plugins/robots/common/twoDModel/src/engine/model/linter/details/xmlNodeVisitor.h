#pragma once
#include <QDomNode>
#include <functional>
namespace qReal {
class ErrorReporterInterface;
}

namespace twoDModel {
namespace model {
namespace details {

class ElementContext;

class XmlNodeVistor
{
public:
	template<typename T>
	using VisitorChooser = std::function<XmlNodeVistor*(T&)>;
	explicit XmlNodeVistor(qReal::ErrorReporterInterface &errorReporter,
			       ElementContext &context);
	virtual ~XmlNodeVistor() = default;
	virtual void visit(const QDomNode &node) const;
	virtual void visitElementNode(const QDomElement &element) const;
	virtual void visitAttributeNode(const QDomAttr &attr) const;
	virtual void visitTextNode(const QDomText &text) const;
	virtual void visitCDATASectionNode(const QDomCDATASection &cDATASection) const;
	virtual void visitEntityReferenceNode(const QDomEntityReference &entityReference) const;
	virtual void visitEntityNode(const QDomEntity &entity) const;
	virtual void visitProcessingInstructionNode(const QDomProcessingInstruction &processingInstruction) const;
	virtual void visitCommentNode(const QDomComment &commentNode) const;
	virtual void visitDocumentNode(const QDomDocument &documentNode) const;
	virtual void visitDocumentTypeNode(const QDomDocumentType &documentType) const;
	virtual void visitDocumentFragmentNode(const QDomDocumentFragment &documentFragment) const;
	virtual void visitNotationNode(const QDomNotation &notation) const;
	qReal::ErrorReporterInterface &errorReporter() const;
	ElementContext &entityContext() const;
private:
	qReal::ErrorReporterInterface &mErrorReporter;
	ElementContext &mEntityContext;

};
}
}
}
