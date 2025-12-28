#pragma once

#include "elementContext.h"

namespace qReal {
class ErrorReporterInterface;
}

namespace twoDModel {
namespace model {
namespace details {

class RootContext: public ElementContext
{
public:
	explicit RootContext(qReal::ErrorReporterInterface &errorReporter);
	~RootContext() override = default;
	bool isAllowedAttr(const QDomAttr &attr) const override;
	bool validateAttrValue(const QDomAttr &attr) const override;
	NodeContext* next(const QDomNode &node) const override;
};
}

}
}
