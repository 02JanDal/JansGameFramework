#include "BaseResourceCacheManager.h"

using namespace GUI;

BaseResourceCacheManager *BaseResourceCacheManager::instance = 0;

BaseResourceCacheManager::BaseResourceCacheManager(QObject *parent) : QObject(parent)
{
}
