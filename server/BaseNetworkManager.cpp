#include "BaseNetworkManager.h"

#include <QTcpSocket>
#include <QDateTime>
#include <QCoreApplication>
#include <iostream>

#include "Logging.h"
#include "BasePermissionManager.h"

using namespace Network;

BaseNetworkManager *BaseNetworkManager::instance = 0;

BaseConnection::BaseConnection(QTcpSocket *socket) : QObject(socket), socket(socket)
{
	connect(socket, &QTcpSocket::disconnected, this, &BaseConnection::disconnected);
	connect(socket, &QTcpSocket::readyRead, this, &BaseConnection::dataReady);
}
BaseConnection::~BaseConnection()
{
	delete socket;
}

void BaseConnection::disconnected()
{
	emit connectionClosed();
}
void BaseConnection::dataReady()
{
	while (socket->bytesAvailable() < 5)
	{
		qApp->processEvents();
	}
	const QByteArray base = socket->read(5);
	const int size = (base[0] << 8) | base[1];
	const QString code = QString::fromUtf8(base.right(3));
	while (socket->bytesAvailable() < size)
	{
		qApp->processEvents();
	}
	const QByteArray payload = socket->read(size);
	emit data(code, payload);
}

void ChatNetworkHandler::handleMessage(const QUuid &from, const QString &code,
									   const QByteArray &payload, BaseNetworkManager *manager)
{
	if (payload.startsWith('/'))
	{
		Permissions::BasePermissionManager::instance->command(from, QString::fromUtf8(payload));
	}
	else
	{
		manager->broadcast(ServerSentCodes::chat,
						   QString("<%1> %2")
							   .arg(manager->username(from), QString::fromUtf8(payload))
							   .toUtf8());
	}
}
void PingNetworkHandler::handleMessage(const QUuid &from, const QString &code,
									   const QByteArray &payload, BaseNetworkManager *manager)
{
	manager->sendTo(from, ServerSentCodes::pingResponse, payload);
}
void LoginNetworkHandler::handleMessage(const QUuid &from, const QString &code,
										const QByteArray &payload, BaseNetworkManager *manager)
{
	manager->login(from, QString::fromUtf8(payload));
}
void LogoutNetworkHandler::handleMessage(const QUuid &from, const QString &code,
										 const QByteArray &payload, BaseNetworkManager *manager)
{
	manager->logout(from);
}

void BaseNetworkManager::registerNetworkHandler(const QString &code,
												BaseNetworkHandler *handler)
{
	m_handlers.insert(code, handler);
}

QString BaseNetworkManager::username(const QUuid &id) const
{
	return m_users[id];
}

BaseNetworkManager::BaseNetworkManager(QObject *parent, const int port, const QString &player)
	: QObject(parent), m_port(port), m_player(player), m_server(new QTcpServer(this))
{
	connect(qApp, &QCoreApplication::aboutToQuit, this, &BaseNetworkManager::shutdown);
	connect(m_server, &QTcpServer::newConnection, this, &BaseNetworkManager::newConnection);

	registerNetworkHandler(ClientSentCodes::chat, new ChatNetworkHandler);
	registerNetworkHandler(ClientSentCodes::pingRequest, new PingNetworkHandler);
	registerNetworkHandler(ClientSentCodes::login, new LoginNetworkHandler);
	registerNetworkHandler(ClientSentCodes::logout, new LogoutNetworkHandler);
}

BaseNetworkManager::~BaseNetworkManager()
{
	qDeleteAll(m_handlers);
}

void BaseNetworkManager::startServer()
{
	qCDebug(LOG_NETWORK) << "Starting server...";
	if (m_server->listen(QHostAddress::Any, m_port))
	{
		qCDebug(LOG_NETWORK) << "Server started, listening on port" << m_server->serverPort();
		std::cout << QByteArray::number(m_server->serverPort()).constData() << '\n'
				  << std::flush;
	}
	else
	{
		qCCritical(LOG_NETWORK) << "Couldn't start server:" << m_server->errorString();
		exit(-1);
	}
}

void BaseNetworkManager::broadcast(const QString &code, const QByteArray &payload)
{
	QByteArray message = code.toUtf8() + payload;
	const unsigned int size = payload.size();
	message.prepend(size & 0xFF);
	message.prepend((size >> 8) & 0xFF);
	foreach(BaseConnection * connection, m_connections.values())
	{
		connection->socket->write(message);
	}
}
void BaseNetworkManager::sendTo(const QUuid &user, const QString &code,
								const QByteArray &payload)
{
	sendTo(QList<QUuid>() << user, code, payload);
}
void BaseNetworkManager::sendTo(const QList<QUuid> &users, const QString &code,
								const QByteArray &payload)
{
	QByteArray message = code.toUtf8() + payload;
	const unsigned int size = payload.size();
	message.prepend(size & 0xFF);
	message.prepend((size >> 8) & 0xFF);
	foreach(const QUuid & user, users)
	{
		m_connections[user]->socket->write(message);
	}
}

void BaseNetworkManager::sendChat(const QList<QUuid> &to, const QString &message)
{
	sendTo(to, ServerSentCodes::chat, message.toUtf8());
}

void BaseNetworkManager::openGui(const QUuid &user, const QString &id, const QString &data)
{
	sendTo(user, ServerSentCodes::openGui, id.toUtf8() + "#" + data.toUtf8());
}

void BaseNetworkManager::login(const QUuid &id, const QString &name)
{
	if (m_player.isNull() || name == m_player ||
		Permissions::BasePermissionManager::instance->hasPermission(name, "login"))
	{
		// TODO check if the username is correct
		m_users.insert(id, name);
		sendChat(QList<QUuid>() << id, "Welcome!");
		return;
	}
	logout(id);
}

void BaseNetworkManager::logout(const QUuid &user)
{
	sendTo(user, ServerSentCodes::closeConnection, "Logout");
	BaseConnection *connection = m_connections.take(m_users.take(user));
	connection->socket->disconnectFromHost();
	connection->socket->waitForDisconnected(1000);
	connection->socket->close();
	connection->deleteLater();
}

void BaseNetworkManager::prepareShutdown(const QString &reason)
{
	qCDebug(LOG_NETWORK) << "Sending shutdown command and stopping to accept new connections";
	m_server->close(); // stop listening for new connections
	broadcast(ServerSentCodes::closeConnection, reason.toUtf8());
}

void BaseNetworkManager::shutdown()
{
	const QList<QUuid> ids = m_users.keys();
	QList<BaseConnection *> connections;
	for (int i = 0; i < ids.size(); ++i)
	{
		BaseConnection *connection = m_connections.take(m_users.take(ids.at(i)));
		connection->socket->disconnectFromHost();
		connections.append(connection);
	}
	qCDebug(LOG_NETWORK) << "Waiting for all connections to close, timing out after 5 seconds";
	qint64 time = QDateTime::currentMSecsSinceEpoch() + 5 * 1000;
	foreach(BaseConnection * connection, connections)
	{
		connection->socket->waitForDisconnected(
			qMax((qint64)1, time - QDateTime::currentMSecsSinceEpoch()));
		connection->socket->close();
		connection->deleteLater();
	}
}

void BaseNetworkManager::newConnection()
{
	while (m_server->hasPendingConnections())
	{
		BaseConnection *connection = new BaseConnection(m_server->nextPendingConnection());
		connect(connection, &BaseConnection::connectionClosed, this,
				&BaseNetworkManager::connectionClosed);
		connect(connection, &BaseConnection::data, this, &BaseNetworkManager::clientData);
		m_connections.insert(QUuid::createUuid(), connection);
		qCDebug(LOG_NETWORK) << "New connection from" << connection->socket->peerAddress();
	}
}
void BaseNetworkManager::connectionClosed()
{
	BaseConnection *connection = qobject_cast<BaseConnection *>(sender());
	const QUuid id = m_connections.key(connection);
	m_connections.remove(id);
	m_users.remove(id);
	connection->deleteLater();
}

void BaseNetworkManager::clientData(const QString &code, const QByteArray &payload)
{
	const QUuid uid = m_connections.key(qobject_cast<BaseConnection *>(sender()));
	if (code != ClientSentCodes::login && !m_users.contains(uid))
	{
		return;
	}
	BaseNetworkHandler *handler = m_handlers.value(code, 0);
	if (handler == 0)
	{
		qCWarning(LOG_NETWORK) << "Unknown network code" << code;
	}
	else
	{
		handler->handleMessage(uid, code, payload, this);
	}
}
