#pragma once
#include "nodeContext.h"

namespace twoDModel {
namespace model {
namespace details {

class ModelContext: public NodeContext
{
public:
	explicit ModelContext(qReal::ErrorReporterInterface &errorReporter);
	virtual ~ModelContext() = default;
	virtual bool validate(const QDomNode &document) const override;
	virtual NodeContext* next(const QDomNode &element) const override;
protected:
	virtual bool validateProcessingInstruction(const QDomProcessingInstruction &documentType) const;
	virtual bool validateDocumentType(const QDomDocumentType &documentType) const;
	virtual bool validateComment(const QDomComment &commentNode) const;
};
}

}
}
