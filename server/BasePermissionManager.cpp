#include "BasePermissionManager.h"

#include <QStringList>
#include <QCoreApplication>

#include "BaseNetworkManager.h"
#include "BasePluginManager.h"
#include "Logging.h"

using namespace Permissions;

BasePermissionManager *BasePermissionManager::instance = 0;

BasePermissionManager::BasePermissionManager(QObject *parent) : QObject(parent)
{
}

BasePermissionManager::~BasePermissionManager()
{

}

bool BasePermissionManager::hasPermission(const QString &user, const QString &permission)
{
	qCDebug(LOG_PERMISSION) << "Checking" << permission << "for" << user;
	return Plugins::BasePluginManager::instance->hasPermission(user, permission);
}

void BasePermissionManager::command(const QUuid &user, const QString &command)
{
	QString cmdString = command;
	if (cmdString.startsWith('/'))
	{
		cmdString.remove(0, 1);
	}
	QStringList split = cmdString.split(' '); // TODO better split ("s etc.)
	if (split.isEmpty())
	{
		return;
	}
	const QString cmd = split.takeFirst();
	if (hasPermission(Network::BaseNetworkManager::instance->username(user), QString("command.%1").arg(cmd)))
	{
		// builtin commands
		{
			if (cmd == "shutdown")
			{
				Network::BaseNetworkManager::instance->prepareShutdown(split.join(' '));
				QCoreApplication::exit(0);
				return;
			}
		}

		if (!Plugins::BasePluginManager::instance->commands().contains(cmd))
		{
			Network::BaseNetworkManager::instance->sendChat(QList<QUuid>() << user, tr("Unknown command %1").arg(cmd));
		}
		Plugins::BasePluginManager::instance->handleCommand(Network::BaseNetworkManager::instance->username(user), cmd, cmdString);
	}
}
