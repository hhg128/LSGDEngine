#include "HCorePCH.h"
#include "HTaskGraphInterface.h"

// task thread runnable
#include "HTaskThreadBase.h"

using namespace lsgd;
using namespace lsgd::async;

HTaskGraphInterface* GTaskGraphInterface = nullptr;

void HTaskGraph1::Initialize()
{
	// generate task graph threads
	uint32 NumProcssors = HGenericPlatformMisc::GetHardwareThreadNum();
	
	// generate task threads 2x than number of processors
	uint32 TaskThreadNum = NumProcssors * 2; 
	for (uint32 TaskThreadIndex = 0; TaskThreadIndex < TaskThreadNum; ++TaskThreadIndex)
	{
		HTaskThreadBase::TaskThreadSharedContext.CreateTaskThread();
	}

	// set the global task graph interface
	GTaskGraphInterface = this;
}

void HTaskGraph1::Destroy()
{

}

class FTriggerEventGraphTasks
{
public:	
	FTriggerEventGraphTasks(shared_ptr<HEvent> InEvent)
		: Event(InEvent)
	{}

	void Execute()
	{
		Event->Trigger();
	}

	shared_ptr<HEvent> Event;
};

void HTaskGraph1::DispatchAndWaitUntilTaskComplete(shared_ptr<HBaseGraphTask>& Task, bool InbNamedThread, const HString& NamedThreadName)
{
	HScopedEvent Event;
	HArray<shared_ptr<HGraphEvent>> Tasks;
	Tasks.push_back(Task->GetCompletionEvent());
	
	HGraphTask<FTriggerEventGraphTasks>::CreateTask(Tasks, InbNamedThread, NamedThreadName).ConstructAndHold(Event.Get());
	Task->Unlock();
}

void HTaskGraph1::WaitUntilTasksComplete(HArray<shared_ptr<HGraphEvent>>& Tasks, bool InbNamedThread, const HString& NamedThreadName)
{
	HScopedEvent Event;		
	HGraphTask<FTriggerEventGraphTasks>::CreateTask(Tasks, InbNamedThread, NamedThreadName).ConstructAndDispatchWhenReady(Event.Get());
}

void HTaskGraph1::WaitUntilTaskComplete(shared_ptr<HGraphEvent>& Task, bool InbNamedThread, const HString& NamedThreadName)
{
	HScopedEvent Event;
	HArray<shared_ptr<HGraphEvent>> Tasks;
	Tasks.push_back(Task);
	HGraphTask<FTriggerEventGraphTasks>::CreateTask(Tasks, InbNamedThread, NamedThreadName).ConstructAndDispatchWhenReady(Event.Get());
}