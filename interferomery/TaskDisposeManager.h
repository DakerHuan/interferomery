#pragma once
#include "TaskDispose.h"
class TaskDisposeManager
{
public:
	TaskDisposeManager();
	~TaskDisposeManager();
	static TaskDispose*	m_pTask;
	static TaskDispose*	GetCurrentTaskObject();
};

