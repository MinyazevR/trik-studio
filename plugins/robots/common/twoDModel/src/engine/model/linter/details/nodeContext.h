#pragma once

#include <QDomAttr>

namespace qReal {
class ErrorReporterInterface;
}

namespace twoDModel {
namespace model {
namespace details {

class NodeContext
{
public:
	explicit NodeContext(qReal::ErrorReporterInterface &errorReporter);
	virtual ~NodeContext() = default;
	virtual bool validate(const QDomNode &element) const;
	virtual NodeContext* next(const QDomNode &element) const = 0;
protected:
	qReal::ErrorReporterInterface &mErrorReporter;
};
}

}
}
