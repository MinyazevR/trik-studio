#pragma once
#include <QDomDocument>

namespace qReal {
class ErrorReporterInterface;
}

namespace twoDModel {
namespace model {
namespace details {

class ModelLinter
{
public:
	void lint(const QDomDocument &document) const;
};
}

}
}
