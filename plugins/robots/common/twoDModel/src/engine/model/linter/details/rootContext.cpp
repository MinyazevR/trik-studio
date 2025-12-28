#include "rootContext.h"
#include "visitorChooser.h"
#include <qrgui/plugins/toolPluginInterface/usedInterfaces/errorReporterInterface.h>
#include <algorithm>

using namespace twoDModel::model::details;

namespace {
	const std::vector<QString> worldModelListAvailableVersions = {"20251219"};

}

RootContext::RootContext(qReal::ErrorReporterInterface &errorReporter):
	ElementContext(errorReporter) {}

NodeContext* RootContext::next(const QDomNode &node) const
{
	return visitorChooser<RootContext>(node, mErrorReporter);
}

bool RootContext::isAllowedAttr(const QDomAttr &attr) const
{
	return attr.name() == "version";
}

bool RootContext::validateAttrValue(const QDomAttr &attr) const
{
	const auto &it = std::find(worldModelListAvailableVersions.begin(),
		worldModelListAvailableVersions.end(), attr.name());

	if (it == worldModelListAvailableVersions.end()) {
		mErrorReporter.addInformation(
			QString("Ivalid value %1 for attribute with a name %2, line:column %3:%4")
			.arg(attr.value(), attr.name(), QString(attr.lineNumber()), QString(attr.columnNumber())));
		return false;
	}
	return true;
}

