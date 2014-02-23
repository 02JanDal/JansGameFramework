#include "BaseSaveManager.h"

using namespace Saves;

BaseSaveManager *BaseSaveManager::instance = 0;

BaseSaveManager::BaseSaveManager(QObject *parent, const QDir &savePath) : QObject(parent), m_savePath(savePath)
{
}

BaseSaveManager::~BaseSaveManager()
{

}
