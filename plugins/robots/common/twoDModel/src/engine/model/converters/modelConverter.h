#pragma once

#include <qrkernel/version.h>

namespace twoDModel {
namespace model {
namespace converters {

class ModelConverters {
public:
	virtual ~ModelConverters();
	virtual void canBeConvert(const qReal::Version &fromVersion,
	                          const qReal::Version &toVersion) const = 0;
	virtual void checkToConvert() const = 0;
	virtual void errorMessage() const = 0;
};
}
}
}
