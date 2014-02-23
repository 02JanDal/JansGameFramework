#pragma once

#include <QObject>

namespace Cache
{
class BaseCacheManager : public QObject
{
	Q_OBJECT
public:
	virtual ~BaseCacheManager();
	static BaseCacheManager *instance;

protected:
	BaseCacheManager(QObject *parent = 0);
};
}
