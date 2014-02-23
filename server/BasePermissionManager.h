#pragma once

#include <QObject>

namespace Permissions
{
class BasePermissionManager : public QObject
{
	Q_OBJECT
public:
	virtual ~BasePermissionManager();
	static BasePermissionManager *instance;

	bool hasPermission(const QString &user, const QString &permission);

public slots:
	void command(const QUuid &user, const QString &command);

protected:
	BasePermissionManager(QObject *parent = 0);
};
}
