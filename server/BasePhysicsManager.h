#pragma once

#include <QObject>

namespace Physics
{
class BasePhysicsManager : public QObject
{
	Q_OBJECT
public:
	virtual ~BasePhysicsManager();
	static BasePhysicsManager *instance;

protected:
	BasePhysicsManager(QObject *parent = 0);
};
}
