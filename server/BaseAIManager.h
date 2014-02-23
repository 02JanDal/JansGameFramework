#pragma once

#include <QObject>

namespace AI
{
class BaseAIManager : public QObject
{
	Q_OBJECT
public:
	virtual ~BaseAIManager();
	static BaseAIManager *instance;

protected:
	BaseAIManager(QObject *parent = 0);
};
}
