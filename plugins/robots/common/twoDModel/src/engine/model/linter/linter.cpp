#include "twoDModel/engine/model/linter/linter.h"
#include "details/modelContext.h"

using namespace twoDModel::model::linter;
using namespace twoDModel::model::details;

void ModelLinter::lint(const QDomDocument &model,
			qReal::ErrorReporterInterface &errorReporter)
{
	ModelContext modelContext(errorReporter);
	modelContext.validate(model);
}
