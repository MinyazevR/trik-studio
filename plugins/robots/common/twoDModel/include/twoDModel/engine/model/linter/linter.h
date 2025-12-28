#pragma once

#include <QObject>
#include <QDomDocument>
#include "twoDModel/twoDModelDeclSpec.h"

namespace qReal {
class ErrorReporterInterface;
}

namespace twoDModel {
namespace model {
namespace linter {

class TWO_D_MODEL_EXPORT ModelLinter: public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(ModelLinter)
public:
	ModelLinter() = default;
	~ModelLinter() override = default;
	void lint(const QDomDocument &model, qReal::ErrorReporterInterface &errorReporter);
};
}
}
}
