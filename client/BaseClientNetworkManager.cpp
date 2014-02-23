#include "BaseClientNetworkManager.h"

#include <QTcpSocket>
#include <QCoreApplication>
#include <QDateTime>
#include <QHostAddress>

#include "ClientLogging.h"
#include "BaseWindow.h"

using namespace Network;

BaseClientNetworkManager *BaseClientNetworkManager::instance = 0;

void ChatNetworkHandler::handleMessage(const QString &code, const QByteArray &payload, BaseClientNetworkManager *manager)
{
	GUI::BaseWindow::instance->chatReceived(QString::fromUtf8(payload));
}
void PingResponseNetworkHandler::handleMessage(const QString &code, const QByteArray &payload, BaseClientNetworkManager *manager)
{
	GUI::BaseWindow::instance->pingReceived(QDateTime::currentMSecsSinceEpoch() - payload.toULongLong());
}
void PropertySetNetworkHandler::handleMessage(const QString &code, const QByteArray &payload, BaseClientNetworkManager *manager)
{

}
void CloseConnectionNetworkHandler::handleMessage(const QString &code, const QByteArray &payload, BaseClientNetworkManager *manager)
{
	const QString reason = QString::fromUtf8(payload);
	// TODO display the reason
	GUI::BaseWindow::instance->setIngame(false);
}
void OpenGuiNetworkHandler::handleMessage(const QString &code, const QByteArray &payload, BaseClientNetworkManager *manager)
{
	const QString message = QString::fromUtf8(payload);
	if (!message.contains('#'))
	{
		qCWarning(LOG_NETWORK) << "Open GUI message doesn't contain a delimiter";
		return;
	}
	const int delimiterPos = message.indexOf('#');
	const QString id = message.left(delimiterPos);
	const QString data = message.mid(delimiterPos + 1);
	GUI::BaseWindow::instance->openGui(id, data);
}

BaseClientNetworkManager::BaseClientNetworkManager(QObject *parent) : QObject(parent), m_socket(new QTcpSocket(this))
{
	connect(m_socket, &QTcpSocket::readyRead, this, &BaseClientNetworkManager::dataReady);
	connect(m_socket, &QTcpSocket::disconnected, this, &BaseClientNetworkManager::connectionClosed);

	registerNetworkHandler(ServerSentCodes::chat, new ChatNetworkHandler);
	registerNetworkHandler(ServerSentCodes::pingResponse, new PingResponseNetworkHandler);
	registerNetworkHandler(ServerSentCodes::closeConnection, new CloseConnectionNetworkHandler);
	registerNetworkHandler(ServerSentCodes::propertySet, new PropertySetNetworkHandler);
	registerNetworkHandler(ServerSentCodes::openGui, new OpenGuiNetworkHandler);
}
BaseClientNetworkManager::~BaseClientNetworkManager()
{
	qDeleteAll(m_handlers);
}

void BaseClientNetworkManager::registerNetworkHandler(const QString &code, BaseNetworkHandler *handler)
{
	m_handlers.insert(code, handler);
}

bool Network::BaseClientNetworkManager::connectTo(const QHostAddress &address, const qint64 port)
{
	if (m_socket->isOpen())
	{
		return false;
	}
	qCDebug(LOG_NETWORK) << "Connecting to server at" << address.toString() << "," << port;
	m_socket->connectToHost(QHostAddress(address), port);
	if (!m_socket->waitForConnected(5000))
	{
		qCDebug(LOG_NETWORK) << "Connecting to server failed:" << m_socket->errorString();
		return false;
	}
	qCDebug(LOG_NETWORK) << "Connection to server established";
	login();
	return true;
}

void BaseClientNetworkManager::disconnectFromServer()
{
	m_socket->disconnectFromHost();
}

void BaseClientNetworkManager::send(const QString &code, const QByteArray &payload)
{
	if (!m_socket->isOpen())
	{
		return;
	}
	QByteArray message = code.toUtf8() + payload;
	const unsigned int size = payload.size();
	message.prepend(size & 0xFF);
	message.prepend((size >> 8) & 0xFF);
	m_socket->write(message);
}

void BaseClientNetworkManager::sendChat(const QString &message)
{
	send(ClientSentCodes::chat, message.toUtf8());
}
void BaseClientNetworkManager::pingRequest()
{
	send(ClientSentCodes::pingRequest, QString::number(QDateTime::currentMSecsSinceEpoch()).toUtf8());
}

void BaseClientNetworkManager::login()
{
	send(ClientSentCodes::login, GUI::BaseWindow::instance->username().toUtf8());
}
void BaseClientNetworkManager::logout()
{
	send(ClientSentCodes::logout);
}

void BaseClientNetworkManager::connectionClosed()
{
	m_socket->close();
	GUI::BaseWindow::instance->setIngame(false);
}

void BaseClientNetworkManager::dataReady()
{
	while (m_socket->bytesAvailable() < 5)
	{
		qApp->processEvents();
	}
	const QByteArray base = m_socket->read(5);
	const int size = (base[0] << 8) | base[1];
	const QString code = QString::fromUtf8(base.right(3));
	while (m_socket->bytesAvailable() < size)
	{
		qApp->processEvents();
	}
	const QByteArray payload = m_socket->read(size);

	BaseNetworkHandler *handler = m_handlers.value(code, 0);
	if (handler == 0)
	{
		qCWarning(LOG_NETWORK) << "Unknown network code" << code;
	}
	else
	{
		handler->handleMessage(code, payload, this);
	}
}
