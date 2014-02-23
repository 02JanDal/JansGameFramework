#include "BaseAIManager.h"

using namespace AI;

BaseAIManager *BaseAIManager::instance = 0;

BaseAIManager::BaseAIManager(QObject *parent) : QObject(parent)
{
}

BaseAIManager::~BaseAIManager()
{

}
