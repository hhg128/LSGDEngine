#pragma once

#include "HReflectionContext.h"

using namespace lsgd::reflect;

namespace lsgd
{
	// memory archive
	class HMemoryArchive : public reflect::HSerializeContext
	{
	public:
		HMemoryArchive(HArray<uint8>& InBuffer);
		virtual ~HMemoryArchive();

		virtual HReflectionContext& operator<<(class HName& Value) override;
		virtual HReflectionContext& operator<<(class HObject*& Value) override;

		virtual void Serialize(void* Value, int64 Length) override;

		HArray<uint8>& Memory;
	};
}