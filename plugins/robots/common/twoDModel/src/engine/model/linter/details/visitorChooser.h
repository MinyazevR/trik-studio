#pragma once
#include "xmlNodeVisitor.h"
#include "worldModelVisitor.h"
#include "modelContext.h"
#include "rootContext.h"
#include "settingsContext.h"

namespace qReal {
class ErrorReporterInterface;
}

namespace twoDModel {
namespace model {
namespace details {

template<typename T>
inline NodeContext* visitorChooser(const QDomNode &node,
	qReal::ErrorReporterInterface &errorReporter);

template<>
inline NodeContext* visitorChooser<ModelContext>(const QDomNode &node,
	qReal::ErrorReporterInterface &errorReporter)
{
	if (node.isElement()) {
		auto &&element = node.toElement();
		if (element.tagName() == "root") {
			return new RootContext(errorReporter);
		}
	}
	return nullptr;
}

template<>
inline NodeContext* visitorChooser<RootContext>(const QDomNode &node,
	qReal::ErrorReporterInterface &errorReporter)
{
	if (node.isElement()) {
		auto &&element = node.toElement();
		if (element.tagName() == "settings") {
			return new SettingsContext(errorReporter);
		}
	}
	return nullptr;
}

template<>
inline NodeContext* visitorChooser<SettingsContext>(const QDomNode &node,
	qReal::ErrorReporterInterface &errorReporter)
{
	Q_UNUSED(node)
	Q_UNUSED(errorReporter)
	return nullptr;
}

}

}
}
