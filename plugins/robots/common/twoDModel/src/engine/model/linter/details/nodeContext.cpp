#include "nodeContext.h"
#include <qrgui/plugins/toolPluginInterface/usedInterfaces/errorReporterInterface.h>


using namespace twoDModel::model::details;

NodeContext::NodeContext(qReal::ErrorReporterInterface &errorReporter)
	: mErrorReporter(errorReporter) {}

bool NodeContext::validate(const QDomNode &element) const
{
	auto node = element.firstChild();
	while (!node.isNull()) {
		const auto* nextValidator = next(node);
		if (!nextValidator) {
			validate(node);
		} else {
			nextValidator->validate(element);
		}
		node = node.nextSibling();
	}
}
