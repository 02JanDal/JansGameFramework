#include "BasePluginManager.h"

#include <QPluginLoader>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>

#include "Logging.h"

using namespace Plugins;

BasePluginInterface::BasePluginInterface(QObject *parent)
	: QObject(parent)
{

}
BasePluginInterface::~BasePluginInterface()
{

}

BaseArchivePlugin::BaseArchivePlugin(const QJsonObject &metadata, const QUrl &root, QObject *parent)
	: BasePluginInterface(parent), m_root(root)
{
	m_name = metadata.value("name").toString();
	m_version = metadata.value("version").toString();
	const QJsonObject commandhandlers = metadata.value("commandHandlers").toObject();
	for (auto it = commandhandlers.begin(); it != commandhandlers.end(); ++it)
	{
		m_commandHandlers.insert(it.key(), CommandCallback::fromJson(it.value().toObject()));
	}
	m_permissionHandler = PermissionCallback::fromJson(metadata.value("permissionHandler").toObject());
	const QJsonObject resources = metadata.value("resources").toObject();
	for (auto it = resources.begin(); it != resources.end(); ++it)
	{
		const QJsonObject resObj = it.value().toObject();
		Resource res;
		res.id = it.key();
		res.url = root.resolved(resObj.value("url").toString());
		const QString type = resObj.value("type").toString();
		if (type == "image")
		{
			res.type = Resource::Image;
		}
		else if (type == "gui")
		{
			res.type = Resource::Gui;
		}
		else if (type == "sound")
		{
			res.type = Resource::Sound;
		}
		else if (type == "native")
		{
			res.type = Resource::Native;
		}
		else
		{
			qCWarning(LOG_PLUGIN) << "Unknown resource type in" << m_name;
		}
		const QString os = resObj.value("os").toString();
		if (os == "windows")
		{
			res.os = Resource::Windows;
		}
		else if (os == "linux")
		{
			res.os = Resource::Linux;
		}
		else if (os == "macosx")
		{
			res.os = Resource::MacOSX;
		}
		else if (os == "android")
		{
			res.os = Resource::Android;
		}
		else if (os == "ios")
		{
			res.os = Resource::iOS;
		}
		else if (os == "blackberry")
		{
			res.os = Resource::BlackBerry;
		}
		else
		{
			qCWarning(LOG_PLUGIN) << "Unknown OS in" << m_name;
		}
		m_resources.append(res);
	}
}
bool BaseArchivePlugin::init()
{
	return true;
}
BasePluginInterface::Permission BaseArchivePlugin::hasPermission(const QString &user, const QString &permission) const
{
	return m_permissionHandler.call(user, permission);
}
bool BaseArchivePlugin::handleCommand(const QString &user, const QString &command, const QString &full)
{
	return m_commandHandlers[command].call(user, command, full);
}

BasePluginManager *BasePluginManager::instance = 0;
BasePluginManager::BasePluginManager(QObject *parent, const QDir &pluginPath) : QObject(parent), m_pluginPath(pluginPath)
{
	m_archiveEndings << ".zip" << ".tar.gz";
}
BasePluginManager::~BasePluginManager()
{

}

bool BasePluginManager::hasPermission(const QString &user, const QString &permission) const
{
	foreach (const BasePluginInterface *plugin, m_plugins)
	{
		switch (plugin->hasPermission(user, permission))
		{
		case BasePluginInterface::DontCare:
			continue;
		case BasePluginInterface::Yes:
			return true;
		case BasePluginInterface::No:
			return false;
		}
	}
	return true;
}
QStringList BasePluginManager::commands() const
{
	QStringList cmds;
	foreach (const BasePluginInterface *plugin, m_plugins)
	{
		cmds.append(plugin->commands());
	}
	return cmds;
}
bool BasePluginManager::handleCommand(const QString &user, const QString &command, const QString &full)
{
	foreach (BasePluginInterface *plugin, m_plugins)
	{
		if (plugin->commands().contains(command))
		{
			return plugin->handleCommand(user, command, full);
		}
	}
	return true;
}
QList<Resource> BasePluginManager::resources() const
{
	QList<Resource> out;
	foreach (const BasePluginInterface *plugin, m_plugins)
	{
		out.append(plugin->resources());
	}
	return out;
}

BaseArchivePlugin *BasePluginManager::createArchivePlugin(const QJsonObject &metadata, const QUrl &root, QObject *parent) const
{
	return new BaseArchivePlugin(metadata, root, parent);
}
void BasePluginManager::discoverPlugins()
{
	qCDebug(LOG_PLUGIN) << "Discovering plugins...";
	QList<QFileInfo> archivePlugins;
	QList<QFileInfo> nativePlugins;
	// checkout all potential plugins and categorize them
	foreach (const QFileInfo &file, m_pluginPath.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot))
	{
		bool recognized = false;
		if (file.isDir() && QDir(file.absoluteFilePath()).exists("plugin.json"))
		{
			recognized = true;
			archivePlugins.append(file);
		}
		else if (QLibrary::isLibrary(file.absoluteFilePath()))
		{
			recognized = true;
			nativePlugins.append(file);
		}
		else
		{
			foreach (const QString &ending, m_archiveEndings)
			{
				if (file.fileName().endsWith(ending))
				{
					recognized = true;
					archivePlugins.append(file);
					break;
				}
			}
		}

		if (!recognized)
		{
			qCWarning(LOG_PLUGIN) << "Didn't recognize" << file.absoluteFilePath();
		}
	}

	qCDebug(LOG_PLUGIN) << "Loading native plugins...";
	foreach (const QFileInfo &file, nativePlugins)
	{
		QPluginLoader loader(file.absoluteFilePath(), this);
		if (!loader.load())
		{
			qCWarning(LOG_PLUGIN) << "Unable to load" << file.absoluteFilePath() << ":" << loader.errorString();
			continue;
		}
		BasePluginInterface *interface = qobject_cast<BasePluginInterface *>(loader.instance());
		if (interface == 0)
		{
			qCWarning(LOG_PLUGIN) << "Unable to load" << file.absoluteFilePath() << ", it does not seem to be a JGF plugin";
			continue;
		}
		m_plugins.append(interface);
	}

	qCDebug(LOG_PLUGIN) << "Loading archive plugins...";
	foreach (const QFileInfo &file, archivePlugins)
	{
		if (file.isDir())
		{
			qCWarning(LOG_PLUGIN) << file.fileName() << "is a directory. It will be loaded, but this way of loading should only be used for testing purposes";
			QFile metafile(QDir(file.absoluteFilePath()).absoluteFilePath("plugin.json"));
			if (!metafile.open(QFile::ReadOnly))
			{
				qCCritical(LOG_PLUGIN) << "Couldn't open plugin.json of" << file.fileName() << ":" << metafile.errorString() << ", that plugin will not be loaded";
				continue;
			}
			QJsonParseError error;
			QJsonDocument doc = QJsonDocument::fromJson(metafile.readAll(), &error);
			if (error.error != QJsonParseError::NoError)
			{
				qCCritical(LOG_PLUGIN) << "There was an error while parsing plugin.json of" << file.fileName() << ":" << error.errorString() << ", that plugin will not be loaded";
				continue;
			}
			m_plugins.append(createArchivePlugin(doc.object(), QUrl::fromLocalFile(file.absoluteFilePath()), this));
		}
		else
		{
			// TODO load archived plugins
		}
	}

	qCDebug(LOG_PLUGIN) << "Initializing plugins...";
	foreach (BasePluginInterface *plugin, m_plugins)
	{
		if (!plugin->init())
		{
			qCWarning(LOG_PLUGIN) << "Error while trying to initialize" << plugin->name() << plugin->version();
			qApp->exit(-1);
		}
	}

	qCDebug(LOG_PLUGIN) << "Plugin loading finished, the following plugins where loaded:";
	foreach (BasePluginInterface *plugin, m_plugins)
	{
		qCDebug(LOG_PLUGIN) << "   " << plugin->name() << plugin->version();
	}
}

BaseArchivePlugin::CommandCallback BaseArchivePlugin::CommandCallback::fromJson(const QJsonObject &object)
{
	return CommandCallback();
}
bool BaseArchivePlugin::CommandCallback::call(const QString &user, const QString &command, const QString &full)
{
	return true;
}
BaseArchivePlugin::PermissionCallback BaseArchivePlugin::PermissionCallback::fromJson(const QJsonObject &object)
{
	return PermissionCallback();
}
BasePluginInterface::Permission BaseArchivePlugin::PermissionCallback::call(const QString &user, const QString &permission) const
{
	return BasePluginInterface::DontCare;
}
