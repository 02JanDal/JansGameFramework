#pragma once
#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <QObject>
#include <QHash>

class QTcpSocket;
class QHostAddress;

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

class BaseClientNetworkManager;

class BaseNetworkHandler
{
public:
	virtual ~BaseNetworkHandler() {}
	virtual void handleMessage(const QString &code, const QByteArray &payload, BaseClientNetworkManager *manager) = 0;
};
class ChatNetworkHandler : public BaseNetworkHandler
{
public:
	void handleMessage(const QString &code, const QByteArray &payload, BaseClientNetworkManager *manager) override;
};
class PingResponseNetworkHandler : public BaseNetworkHandler
{
public:
	void handleMessage(const QString &code, const QByteArray &payload, BaseClientNetworkManager *manager) override;
};
class PropertySetNetworkHandler : public BaseNetworkHandler
{
public:
	void handleMessage(const QString &code, const QByteArray &payload, BaseClientNetworkManager *manager) override;
};
class CloseConnectionNetworkHandler : public BaseNetworkHandler
{
public:
	void handleMessage(const QString &code, const QByteArray &payload, BaseClientNetworkManager *manager) override;
};
class OpenGuiNetworkHandler : public BaseNetworkHandler
{
public:
	void handleMessage(const QString &code, const QByteArray &payload, BaseClientNetworkManager *manager) override;
};

class BaseClientNetworkManager : public QObject
{
	Q_OBJECT
public:
	virtual ~BaseClientNetworkManager();
	static BaseClientNetworkManager *instance;

	void registerNetworkHandler(const QString &code, BaseNetworkHandler *handler);

	bool connectTo(const QHostAddress &address, const qint64 port);
	void disconnectFromServer();

protected:
	BaseClientNetworkManager(QObject *parent = 0);

	QTcpSocket *m_socket;
	QHash<QString, BaseNetworkHandler *> m_handlers;

public slots:
	void send(const QString &code, const QByteArray &payload = QByteArray());

	void sendChat(const QString &message);
	void login();
	void logout();
	void pingRequest();

private slots:
	void connectionClosed();
	void dataReady();
};
}
