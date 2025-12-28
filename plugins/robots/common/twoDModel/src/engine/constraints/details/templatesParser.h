#pragma once

#include <QObject>
#include <QDomElement>
#include <unordered_map>
#include "template.h"

namespace twoDModel {
namespace constraints {
namespace details {

class XmlTemplate;

class TemplatesParser: public QObject {
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(TemplatesParser)
public:
	TemplatesParser() = default;
	bool parse(const QDomElement &templatesXml);
	bool transform(const QDomElement &constraintsXml);
	QDomElement processTemplate(const QDomElement &elements);
	QStringList parsingErrors() const;
	QStringList substituionErrors() const;
private:
	void clear();
	void parseError(const QString& message);
	void substituteError(const QString& message);
	bool parseTemplate(const QDomElement &templateElement);
	std::unordered_map<QString, XmlTemplate> mTemplates;
	QStringList mParsingErrors;
	QStringList mSubstituionErrors;
};
}
}
}
