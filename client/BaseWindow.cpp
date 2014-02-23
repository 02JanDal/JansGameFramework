#include "BaseWindow.h"

#include <QProcess>
#include <QGuiApplication>
#include <QDir>
#include <QDateTime>
#include <QHostAddress>

#include "BaseClientNetworkManager.h"
#include "ClientLogging.h"

using namespace GUI;

BaseWindow *BaseWindow::instance = 0;

BaseWindow::BaseWindow(QWindow *parent)
	: QQuickView(parent), m_serverProcess(0), m_ingame(false)
{
	m_username = "02JanDal";
}

BaseWindow::~BaseWindow()
{
	if (m_serverProcess)
	{
		if (m_serverProcess->isOpen())
		{
			m_serverProcess->terminate();
		}
		delete m_serverProcess;
		m_serverProcess = 0;
	}
}

void BaseWindow::play()
{
	if (m_serverProcess)
	{
		return;
	}
	const QDir current = QDir::current();
	const QString plugins = current.absoluteFilePath("plugins");
	const QString configs = current.absoluteFilePath("configs");
	const QString save = current.absoluteFilePath("save");
	m_serverProcess = new QProcess(this);
	m_serverProcess->setProgram(qApp->applicationDirPath() + "/IcecreamServer");
	m_serverProcess->setWorkingDirectory(QDir::currentPath());
	m_serverProcess->setArguments(QStringList()
								  << "--pluginspath" << plugins
								  << "--configspath" << configs
								  << "--save" << save
								  << "--singleplayer" << m_username);
	m_serverProcess->setStandardErrorFile(current.absoluteFilePath("ServerLog.txt"));
	connect(m_serverProcess, &QProcess::readyRead, [this]()
	{
		while (!m_serverProcess->canReadLine())
		{
			qApp->processEvents();
		}
		const quint64 port = m_serverProcess->readLine().replace('\n', "").toULongLong();
		qCDebug(LOG_GENERAL) << "Server started at" << port;
		Network::BaseClientNetworkManager::instance->connectTo(QHostAddress::LocalHost, port);
		setIngame(true);
	});
	qCDebug(LOG_GENERAL) << "Server starting...";
	m_serverProcess->start(QProcess::ReadWrite);
}

void BaseWindow::stopPlaying()
{
	Network::BaseClientNetworkManager::instance->disconnectFromServer();
	if (m_serverProcess)
	{
		m_serverProcess->write("shutdown\n");
		connect(m_serverProcess, SIGNAL(finished(int)), m_serverProcess, SLOT(deleteLater()));
	}
}

void BaseWindow::chatReceived(const QString &message)
{
	emit chat(message);
}
void BaseWindow::pingReceived(const unsigned int time)
{
	emit chat(tr("Ping took %1ms").arg(time));
}

void BaseWindow::openGui(const QString &id, const QString &data)
{

}
