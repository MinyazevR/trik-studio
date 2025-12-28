#include "modelVisitorChooser.h"
#include "worldModelVisitor.h"
// #include "worldModelVisitorChooser.h"

using namespace twoDModel::model::details;

XmlNodeVistor* ModelVisitorChooser::choose(const QDomNode &node) const
{
	if (node.isElement()) {
		const auto &currentElement = node.toElement();
		if (currentElement.tagName() == "settings") {
			return new SettingsVisitor();
		}
//		if (currentElement.tagName() == "world") {
			return new ElementVisitor();
//		}
//		if (currentElement.tagName() == "robots") {

//		}
//		if (currentElement.tagName() == "constraints") {

//		}
//		if (currentElement.tagName() == "blobs") {

//		}
	} else {
		return new XmlNodeVistor;
	}
}
