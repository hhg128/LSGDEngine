#pragma once

#include "HTickFunction.h"

namespace lsgd
{
	class HTickTaskLevel
	{
	public:
		HTickTaskLevel() {}
		~HTickTaskLevel() {}

		struct HTickScheduleDetails
		{
			shared_ptr<HTickFunction> TickFunction;
			float CoolDown;
		};

		void StartFrame(HTickFunctionContext& InContext);
		void EndFrame();

		void QueueAllTicks();

		HArray<shared_ptr<HTickFunction>> AllEnabledTickFunctions;
		HArray<shared_ptr<HTickFunction>> AllCoolingDownTickFunctions;
		HArray<shared_ptr<HTickFunction>> AllDisabledTickFunctions;
		HArray<shared_ptr<HTickFunction>> NewlySpawnedTickFunctions;
		HTickFunctionContext TickContext;
	};
}