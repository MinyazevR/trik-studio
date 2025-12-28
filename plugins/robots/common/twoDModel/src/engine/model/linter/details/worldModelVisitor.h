#pragma once
#include "xmlNodeVisitor.h"

namespace twoDModel {
namespace model {
namespace details {
class ElementVisitor: public XmlNodeVistor
{
public:
	explicit ElementVisitor(qReal::ErrorReporterInterface &errorReporter,
				   ElementContext &context);
	void visitElementNode(const QDomElement &element) const override final;
};
}

}
}
