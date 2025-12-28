#pragma once
#include "nodeContext.h"
#include <QDomAttr>
#include <unordered_set>

namespace qReal {
class ErrorReporterInterface;
}

namespace twoDModel {
namespace model {
namespace details {

class ElementContext: public NodeContext
{
public:
	explicit ElementContext(qReal::ErrorReporterInterface &errorReporter);
	~ElementContext() override = default;
	bool validate(const QDomNode &element) const override;
protected:
	virtual bool validateAttr(const QDomAttr &attr) const;
	virtual bool isAllowedAttr(const QDomAttr &attr) const = 0;
	virtual bool validateAttrValue(const QDomAttr &attr) const = 0;
//	virtual std::unordered_set<QString> requiredAttibutes() const {
//		return std::unordered_set<QString>{""};
//	}

	virtual bool validateText(const QDomText &text) const;
	virtual bool validateCDATASection(const QDomCDATASection &cDATASection) const;
	virtual bool validateComment(const QDomComment &commentNode) const;
	virtual bool validateEntityReference(const QDomEntityReference &entityReference) const;
};
}

}
}
