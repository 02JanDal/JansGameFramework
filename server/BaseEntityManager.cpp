#include "BaseEntityManager.h"

using namespace Entities;

BaseEntityManager *BaseEntityManager::instance = 0;

BaseEntityManager::BaseEntityManager(QObject *parent) : QObject(parent)
{
}


BaseEntityManager::~BaseEntityManager()
{

}
