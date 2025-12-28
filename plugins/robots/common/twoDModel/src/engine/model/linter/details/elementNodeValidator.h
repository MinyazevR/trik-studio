#pragma once
#include "xmlNodeValidator.h"
#include "elementContext.h"

namespace qReal {
class ErrorReporterInterface;
}

namespace twoDModel {
namespace model {
namespace details {

class EntityContext;

class ElementNodeValidator: public XmlNodeValidator
{
public:
	explicit ElementNodeValidator(qReal::ErrorReporterInterface &errorReporter,
				  const EntityContext& context);
	virtual ~ElementNodeValidator() override = default;
	virtual void validate() const override;
};
}
}
}
