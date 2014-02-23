#pragma once

#include <QObject>

namespace Entities
{
class BaseEntityManager : public QObject
{
	Q_OBJECT
public:
	virtual ~BaseEntityManager();
	static BaseEntityManager *instance;

protected:
	BaseEntityManager(QObject *parent = 0);
};
}
