#pragma once

#include "elementContext.h"

namespace qReal {
class ErrorReporterInterface;
}

namespace twoDModel {
namespace model {
namespace details {

class SettingsContext: public ElementContext
{
public:
	explicit SettingsContext(qReal::ErrorReporterInterface &errorReporter);
	~SettingsContext() override = default;
	bool isAllowedAttr(const QDomAttr &attr) const override;
	bool validateAttrValue(const QDomAttr &attr) const override;
	NodeContext* next(const QDomNode &node) const override;
};
}

}
}
