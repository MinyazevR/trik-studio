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

class XmlNodeValidator
{
public:
	explicit XmlNodeValidator(qReal::ErrorReporterInterface &errorReporter);
	virtual ~XmlNodeValidator() = default;
	virtual void validate() const = 0;
};
}
}
}
