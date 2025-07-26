#pragma once
#include <QtCore/QObject>

namespace twoDModel {
namespace model {

class PhysicsItemSettings : public QObject
{
	Q_OBJECT

public:
	Settings() = default;

	bool realisticPhysics() const;

	bool realisticSensors() const;

	bool realisticMotors() const;

	void serialize(QDomElement &parent) const;

	void deserialize(const QDomElement &parent);

	void setRealisticPhysics(bool set);

	void setRealisticSensors(bool set);

	void setRealisticMotors(bool set);

signals:
	/// Emitted each time when user modifies physical preferences.
	void physicsChanged(bool isRealistic);

private:
	bool mRealisticPhysics { false };
	bool mRealisticSensors { false };
	bool mRealisticMotors { false };
};

}
}
