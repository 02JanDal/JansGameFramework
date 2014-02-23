#pragma once

#include <QObject>
#include <QDir>

namespace Saves
{
class BaseSaveManager : public QObject
{
	Q_OBJECT
public:
	virtual ~BaseSaveManager();
	static BaseSaveManager *instance;

protected:
	BaseSaveManager(QObject *parent, const QDir &savePath);

	QDir m_savePath;
};
}
