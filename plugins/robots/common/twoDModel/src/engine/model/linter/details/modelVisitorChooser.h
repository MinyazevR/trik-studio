#pragma once
#include "visitorChooser.h"

namespace twoDModel {
namespace model {
namespace details {
class ModelVisitorChooser: VisitorChooser
{
public:
	~ModelVisitorChooser() override;
	XmlNodeVistor* choose(const QDomNode &node) const override final;
};
}

}
}
