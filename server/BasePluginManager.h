#pragma once

#include <QObject>
#include <QDir>
#include <QUrl>
#include <QStringList>
#include <QMap>

#define BASEPLUGININTERFACE_iid "jan.dalheimer.jgf.BasePluginInterface"

namespace Plugins
{
struct Resource
{
	QString id;
	QUrl url;
	enum Type
	{
		Image,
		Gui,
		Sound,
		Native
	};
	Type type;
	enum Os
	{
		Windows,
		Linux,
		MacOSX,
		Android,
		iOS,
		BlackBerry
	};
	Os os;
};

class BasePluginInterface : public QObject
{
	Q_OBJECT
public:
	explicit BasePluginInterface(QObject *parent = 0);
	virtual ~BasePluginInterface();

	virtual bool init() = 0;
	virtual QString name() = 0;
	virtual QString version() = 0;

	enum Permission
	{
		Yes,
		No,
		DontCare
	};

	virtual Permission hasPermission(const QString &user, const QString &permission) const { return DontCare; }
	virtual QStringList commands() const { return QStringList(); }
	virtual bool handleCommand(const QString &user, const QString &command, const QString &full) { return true; }
	virtual QList<Resource> resources() const { return QList<Resource>(); }
};
class BaseArchivePlugin : public BasePluginInterface
{
	Q_OBJECT
public:
	BaseArchivePlugin(const QJsonObject &metadata, const QUrl &root, QObject *parent = 0);

	bool init() override;
	QString name() override { return m_name; }
	QString version() override { return m_version; }

	Permission hasPermission(const QString &user, const QString &permission) const override;
	QStringList commands() const override { return m_commandHandlers.keys(); }
	bool handleCommand(const QString &user, const QString &command, const QString &full) override;
	QList<Resource> resources() const override { return m_resources; }

	struct Callback
	{

	};
	struct CommandCallback
	{
		static CommandCallback fromJson(const QJsonObject &object);
		bool call(const QString &user, const QString &command, const QString &full);
	};
	struct PermissionCallback
	{
		static PermissionCallback fromJson(const QJsonObject &object);
		Permission call(const QString &user, const QString &permission) const;
	};

protected:
	QString m_name;
	QString m_version;
	QUrl m_root;
	QMap<QString, CommandCallback> m_commandHandlers;
	PermissionCallback m_permissionHandler;
	QList<Resource> m_resources;
};

class BasePluginManager : public QObject
{
	Q_OBJECT
public:
	virtual ~BasePluginManager();
	static BasePluginManager *instance;

	void discoverPlugins();

	bool hasPermission(const QString &user, const QString &permission) const;
	QStringList commands() const;
	bool handleCommand(const QString &user, const QString &command, const QString &full);

	QList<Resource> resources() const;

	virtual BaseArchivePlugin *createArchivePlugin(const QJsonObject &metadata, const QUrl &root, QObject *parent) const;

protected:
	BasePluginManager(QObject *parent, const QDir &pluginPath);

	QDir m_pluginPath;
	QStringList m_archiveEndings;

	QList<BasePluginInterface *> m_plugins;
};
}

Q_DECLARE_INTERFACE(Plugins::BasePluginInterface, BASEPLUGININTERFACE_iid)
