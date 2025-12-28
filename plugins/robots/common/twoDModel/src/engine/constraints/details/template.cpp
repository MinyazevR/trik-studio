#include "template.h"
#include <QHash>

using namespace twoDModel::constraints::details;

XmlTemplate::XmlTemplate(const QString &id, const QString &text)
	: mId(id), mText(text) {};

void XmlTemplate::emplaceParam(uint16_t startPosition, uint32_t length, const QString &name)
{
	mParams.emplace(startPosition, XmlTemplate::TemplateParamInfo(startPosition, length, name));
};

QString XmlTemplate::proccess(const QHash<QString, QString> &paramsForReplace) const
{
	auto result = mText;
	for (auto it = mParams.rbegin(); it != mParams.rend(); ++it) {
		auto replaceIt = paramsForReplace.find(it->second.mName);
		if (replaceIt == paramsForReplace.end()) {
			return {};
		}

		const auto &value = replaceIt.value();
		result.replace(it->second.mStartPosition, it->second.mLength, value);
	}
	return result;
}
