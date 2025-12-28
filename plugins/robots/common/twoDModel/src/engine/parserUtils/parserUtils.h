#pragma once
#include <QObject>

class QXmlStreamReader;

namespace qReal {
class ErrorReporterInterface;
}

namespace twoDModel {
namespace utils {

struct TagEntry
{
    QString tagName;
    std::function<void(QXmlStreamReader&)> handler;
};

class ParserUtils: public QObject
{
	Q_OBJECT
public:
	static void checkTags(
		QXmlStreamReader &reader,
		qReal::ErrorReporterInterface &errorReporter,
		std::initializer_list<TagEntry> expectedTags
	);

};

}
}
