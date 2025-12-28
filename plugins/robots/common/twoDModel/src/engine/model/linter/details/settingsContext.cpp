#include "settingsContext.h"
#include "visitorChooser.h"
#include <qrgui/plugins/toolPluginInterface/usedInterfaces/errorReporterInterface.h>
#include <algorithm>

using namespace twoDModel::model::details;

SettingsContext::SettingsContext(qReal::ErrorReporterInterface &errorReporter):
	ElementContext(errorReporter) {}

NodeContext* SettingsContext::next(const QDomNode &node) const
{
	return visitorChooser<SettingsContext>(node, mErrorReporter);
}

bool SettingsContext::isAllowedAttr(const QDomAttr &attr) const
{
	return attr.name() == "" || attr.name() == "" || attr.name() == "";
}

bool SettingsContext::validateAttrValue(const QDomAttr &attr) const
{
	if (attr.name() == "") {

	} else if (attr.name() == "") {

	}
	return false;
}

