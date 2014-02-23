#pragma once

#include <QQuickView>

class QProcess;

namespace GUI
{
class BaseWindow : public QQuickView
{
	Q_OBJECT
	Q_PROPERTY(bool ingame READ ingame WRITE setIngame NOTIFY ingameChanged)

public:
	virtual ~BaseWindow();
	static BaseWindow *instance;

	QString username() const
	{
		return m_username;
	}
	bool ingame() const
	{
		return m_ingame;
	}

public slots:
	virtual void play();
	virtual void stopPlaying();

	void chatReceived(const QString &message);
	void pingReceived(const unsigned int time);
	void openGui(const QString &id, const QString &data);

	void setIngame(bool arg)
	{
		if (m_ingame != arg) {
			m_ingame = arg;
			emit ingameChanged(arg);
		}
	}

signals:
	void ingameChanged(bool arg);
	void chat(const QString &msg);

protected:
	explicit BaseWindow(QWindow *parent = 0);

	QProcess *m_serverProcess;
	QString m_username;
	bool m_ingame;
};
}
