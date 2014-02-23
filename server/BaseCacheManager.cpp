#include "BaseCacheManager.h"

using namespace Cache;

BaseCacheManager *BaseCacheManager::instance = 0;

BaseCacheManager::BaseCacheManager(QObject *parent) : QObject(parent)
{
}

BaseCacheManager::~BaseCacheManager()
{

}
