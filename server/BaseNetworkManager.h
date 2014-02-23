#pragma once
#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <QObject>

#include <QTcpServer>
#include <QUuid>

namespace Network
{
namespace ClientSentCodes
{
	static constexpr char *pingRequest = "PRQ";
	static constexpr char *chat        = "CHT";
	static constexpr char *login       = "LOG";
	static constexpr char *logout	   = "BYE";
}
namespace ServerSentCodes
{
	static constexpr char *pingResponse    = "PRS";
	static constexpr char *chat            = "CHT";
	static constexpr char *propertySet     = "PRO";
	static constexpr char *closeConnection = "BYE";
	static constexpr char *openGui         = "GUI";
}

class BaseNetworkManager;

class BaseConnection : public QObject
{
	Q_OBJECT
public:
	BaseConnection(QTcpSocket *socket);
	virtual ~BaseConnection();

	QTcpSocket *socket;

private slots:
	void disconnected();
	void dataReady();

signals:
	void connectionClosed();
	void data(const QString &code, const QByteArray &payload);
};

// QUESTION this is only one function, maybe use a function pointer instead?
class BaseNetworkHandler
{
public:
	virtual ~BaseNetworkHandler() {}
	virtual void handleMessage(const QUuid &from, const QString &code, const QByteArray &payload, BaseNetworkManager *manager) = 0;
};
class ChatNetworkHandler : public BaseNetworkHandler
{
public:
	void handleMessage(const QUuid &from, const QString &code, const QByteArray &payload, BaseNetworkManager *manager) override;
};
class PingNetworkHandler : public BaseNetworkHandler
{
public:
	void handleMessage(const QUuid &from, const QString &code, const QByteArray &payload, BaseNetworkManager *manager) override;
};
class LoginNetworkHandler : public BaseNetworkHandler
{
public:
	void handleMessage(const QUuid &from, const QString &code, const QByteArray &payload, BaseNetworkManager *manager) override;
};
class LogoutNetworkHandler : public BaseNetworkHandler
{
public:
	void handleMessage(const QUuid &from, const QString &code, const QByteArray &payload, BaseNetworkManager *manager) override;
};

class BaseNetworkManager : public QObject
{
	Q_OBJECT
	friend class LoginNetworkHandler;
	friend class LogoutNetworkHandler;
public:
	virtual ~BaseNetworkManager();
	static BaseNetworkManager *instance;

	void startServer();

	void registerNetworkHandler(const QString &code, BaseNetworkHandler *handler);

	QString username(const QUuid &id) const;

public slots:
	void broadcast(const QString &code, const QByteArray &payload);
	void sendTo(const QUuid &user, const QString &code, const QByteArray &payload);
	void sendTo(const QList<QUuid> &users, const QString &code, const QByteArray &payload);

	void sendChat(const QList<QUuid> &to, const QString &message);
	void openGui(const QUuid &user, const QString &id, const QString &data);
	void login(const QUuid &id, const QString &username);
	void logout(const QUuid &user);

	void prepareShutdown(const QString &reason);
	void shutdown();

protected:
	BaseNetworkManager(QObject *parent, const int port, const QString &player);

	int m_port;
	QString m_player;

	QTcpServer *m_server;
	QHash<QUuid, BaseConnection *> m_connections;
	QHash<QUuid, QString> m_users;

	QHash<QString, BaseNetworkHandler *> m_handlers;

private slots:
	void newConnection();
	void connectionClosed();
	void clientData(const QString &code, const QByteArray &payload);
};
}
