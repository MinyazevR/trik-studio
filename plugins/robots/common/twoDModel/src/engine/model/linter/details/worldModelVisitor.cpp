#pragma once
#include "worldModelVisitor.h"
#include "elementContext.h"
#include <qrgui/plugins/toolPluginInterface/usedInterfaces/errorReporterInterface.h>

using namespace twoDModel::model::details;

ElementVisitor::ElementVisitor(qReal::ErrorReporterInterface &errorReporter,
				     VisitorChooser<const QDomNode> &visitorChooser,
				     ElementContext &context)
	: XmlNodeVistor(errorReporter, context)
	, mVisitorChooser(visitorChooser) {}

void ElementVisitor::visitElementNode(const QDomElement &element) const
{
	const auto &attrs = element.attributes();
	for (int i = 0; i < attrs.count(); ++i) {
	    QDomAttr attr = attrs.item(i).toAttr();
	    visitAttributeNode(attr);
	}

	auto node = element.firstChild();

	while (!node.isNull()) {
		const auto* visitor = mVisitorChooser(node);
		if (!visitor) {
			visit(node);
		} else {
			visitor->visit(element);
		}
		node = node.nextSibling();
	}
}
