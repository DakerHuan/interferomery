#include "stdafx.h"
#include "TaskDisposeManager.h"


TaskDisposeManager::TaskDisposeManager()
{
}


TaskDisposeManager::~TaskDisposeManager()
{
}
TaskDispose* TaskDisposeManager::GetCurrentTaskObject()
{
	return TaskDisposeManager::m_pTask;
}