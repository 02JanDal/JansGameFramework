#pragma once

#include <QObject>

namespace GUI
{
class BaseResourceCacheManager : public QObject
{
	Q_OBJECT
public:
	static BaseResourceCacheManager *instance;

protected:
	BaseResourceCacheManager(QObject *parent = 0);
};
}
