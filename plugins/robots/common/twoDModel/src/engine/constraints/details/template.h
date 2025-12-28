#pragma once

#include <QDomElement>
#include <unordered_map>
#include <map>

namespace twoDModel {
namespace constraints {
namespace details {

class XmlTemplate {
public:
	XmlTemplate(const QString &id, const QString &text);
	void emplaceParam(uint16_t startPosition, uint32_t length, const QString &name);
	QString proccess(const QHash<QString, QString> &paramsForReplace) const;

private:
	struct TemplateParamInfo {
		uint16_t mStartPosition;
		uint32_t mLength;
		QString mName;

		TemplateParamInfo(uint16_t startPosition,
			      uint32_t length,
			      const QString& name)
		    : mStartPosition(startPosition), mLength(length), mName(name) {}
	};
	QString mId;
	QString mText;
	std::map<int, TemplateParamInfo> mParams;
};
}
}
}
