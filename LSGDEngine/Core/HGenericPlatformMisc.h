#pragma once

#include "HThreadRunnable.h"

namespace lsgd {
	
	enum EFileUsageFlag
	{
		Read = (1 << 0),
		Write = (1 << 1),
	};

	class HPlatformFileIO
	{
	public:
		virtual void OpenFile(const HString& InFilename, int32 InFileUsage) = 0;
		virtual void WriteFile(void* InBuffer, int64 InSize, int64& OutSize) = 0;
		virtual void ReadFile(void* OutBuffer, int64 InMaxSize, int64& OutSize) = 0;
		virtual void CloseFile() = 0;

		// filename
		HString Filename;

		// file usage flag (read | write)
		int32 FileUsageFlag;
	};

	class HPlatformThread
	{
	public:
		explicit HPlatformThread(unique_ptr<lsgd::thread::HThreadRunnable>& InRunnable)
			: Runnable(move(InRunnable))
		{}

		virtual ~HPlatformThread(){}

		virtual bool Create(uint32 CpuCoreAffinity) = 0;
		virtual void Destroy() = 0;

		// thread slot; ThreadRunnable
		unique_ptr<lsgd::thread::HThreadRunnable> Runnable;
	};

	// note that all definition of this class is reside in each platform misc.cpp (e.g. HWindowsPlatformMisc.cpp)
	class HGenericPlatformMisc
	{
	public:
		// get game directory
		static HString GetGameDir();

		static unique_ptr<HPlatformFileIO> CreatePlatformFileIO();
	};

}