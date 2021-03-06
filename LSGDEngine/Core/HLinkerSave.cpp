#include "HCorePCH.h"
#include "HLinkerSave.h"

// HObject
#include "HObject.h"
// HPackage
#include "HPackage.h"
// file archive
#include "HFileArchive.h"

using namespace lsgd;
using namespace reflect;

HLinkerSave::HLinkerSave()
	: SrcPackageToSave(nullptr)
{

}

HLinkerSave::~HLinkerSave()
{

}

struct HTagExportObjectsRecursive : public HReflectionContext
{
	HTagExportObjectsRecursive(HPackage* InPackage)
		: OutermostPackage(InPackage)
	{}

	virtual HReflectionContext& operator<<(class HObject*& Value) override
	{
		if (Value->Package == OutermostPackage)
		{
			// tag the current object as export object
			Value->State.TagObjectFlags(EObjectFlags::TagExp);

			// do the recursive tagging
			Value->Serialize(*this);

			// add tagged object
			TaggedObjects.push_back(Value);
		}

		return *this;
	}

	typedef HObject* TaggedObjectType;

	HArray<TaggedObjectType> TaggedObjects;
	HPackage* OutermostPackage;
};

struct HTagNameRecursive : public HReflectionContext
{
	HTagNameRecursive(HArray<HName>& OutNameMap)
		: NameMap(OutNameMap)
	{}

	virtual HReflectionContext& operator<<(class HName& Value) override
	{
		NameMap.push_back(Value);
		return *this;
	}

	virtual HReflectionContext& operator<<(class HObject*& Value) override
	{
		if (((Value->State.ObjectFlags) & (EObjectFlags::TagExp | EObjectFlags::TagImp)) != 0)
		{
			// only when object is tagged by import or export object, serialize HName
			*this << Value->Name;

			// serialize recursively
			Value->Serialize(*this);
		}

		return *this;
	}

	HArray<HName>& NameMap;
};

bool HLinkerSave::SavePackage(HLinkerSaveContext& InContext)
{
	// create file archive
	HString FilePath = HGenericPlatformMisc::GetGameDir() + InContext.OutermostPkg->Name.ToString();
	FileArchive = make_unique<fileIO::HFileArchive, fileIO::HFileArchiveWrite>(FilePath);

	// setting attributes
	LinkerRoot = InContext.OutermostPkg;

	HArray<HObject*>& ObjectsToSave = InContext.ObjectsToSave;

	// tag archive
	HTagExportObjectsRecursive TagExportArchive(LinkerRoot);

	// tag the HObjects to save as export recursively
	for (HObject* Object : ObjectsToSave)
	{
		TagExportArchive << Object;
	}

	// tag import objects from export objects
	for (HObject* Object : TagExportArchive.TaggedObjects)
	{
		// @todo - need to tag 'TagImp'
		// note that do NOT tag import recursively
	}

	// create export map
	for (HObject* Object : TagExportArchive.TaggedObjects)
	{
		int32 OutIndex = ExportMap.size();
		ExportMap.push_back(HObjectExport());

		ExportMap[OutIndex].OuterIndex = OutIndex + 1;
 
		ExportMap[OutIndex].Object = Object;
		ExportMap[OutIndex].ObjectName = Object->Name;

		// create type info ans set it to HObjectExport
		HTypeInfo TypeInfo;
		TypeInfo.TypeName = Object->Class->Name;

		ExportMap[OutIndex].TypeInfo = TypeInfo;
	}

	// create import map
	// @todo

	// add name to name-map by HTagNameRecursive
	HTagNameRecursive SerializeNameArchive(NameMap);
	for (HObjectExport& ObjectExport : ExportMap)
	{
		SerializeNameArchive << ObjectExport.Object;
	}
	
	// initialize TOC
	int64 TOCOffset = FileArchive->Tell();
	(*FileArchive) << TOC;

	// serialize export/import map
	TOC.ExportOffset = static_cast<int32>(FileArchive->Tell());
	TOC.ExportCount = ExportMap.size();
	(*FileArchive) << ExportMap;

	TOC.ImportOffset = static_cast<int32>(FileArchive->Tell());
	TOC.ImportCount = ImportMap.size();
	(*FileArchive) << ImportMap;

	TOC.NameOffset = static_cast<int32>(FileArchive->Tell());
	TOC.NameCount = NameMap.size();
	(*FileArchive) << NameMap;

	TOC.DependsOffset = static_cast<int32>(FileArchive->Tell());
	(*FileArchive) << DependsMap;

	// mark as header-end
	TOC.TotalHeaderSize = static_cast<int32>(FileArchive->Tell()) - (int32)TOCOffset;

	// real object export serialization
	for (HObjectExport& ObjectExport : ExportMap)
	{
		ObjectExport.SerialOffset = FileArchive->Tell();

		// serialize real object
		(*FileArchive) << ObjectExport.Object;

		ObjectExport.SerialSize = FileArchive->Tell() - ObjectExport.SerialOffset;
	}

	// override TOC information
	FileArchive->Move(TOCOffset);
	(*FileArchive) << TOC;

	// override export map information
	check(FileArchive->Tell() == TOC.ExportOffset);
	(*FileArchive) << ExportMap;

	check(FileArchive->Tell() == TOC.ImportOffset);

	// move the archive offset to the end
	FileArchive->Move(TOC.TotalHeaderSize);

	// @todo - temporary
	FileArchive.reset();

	return true;
}