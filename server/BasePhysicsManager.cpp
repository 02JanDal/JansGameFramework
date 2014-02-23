#include "BasePhysicsManager.h"

using namespace Physics;

BasePhysicsManager *BasePhysicsManager::instance = 0;

BasePhysicsManager::BasePhysicsManager(QObject *parent) : QObject(parent)
{
}

BasePhysicsManager::~BasePhysicsManager()
{

}
