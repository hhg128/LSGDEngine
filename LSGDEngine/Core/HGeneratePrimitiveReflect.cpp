#include "HCorePCH.h"
#include "HGeneratePrimitiveReflect.h"
#include "HReflectImplement.h"

using namespace lsgd;
using namespace lsgd::reflect;

// declare basic primitive types and register types
IMPLEMENT_PRIMITIVE_TYPE(int8)
IMPLEMENT_PRIMITIVE_TYPE(int16)
IMPLEMENT_PRIMITIVE_TYPE(int32)
IMPLEMENT_PRIMITIVE_TYPE(int64)

IMPLEMENT_PRIMITIVE_TYPE(uint8)
IMPLEMENT_PRIMITIVE_TYPE(uint16)
IMPLEMENT_PRIMITIVE_TYPE(uint32)
IMPLEMENT_PRIMITIVE_TYPE(uint64)

IMPLEMENT_PRIMITIVE_TYPE(bool)
IMPLEMENT_PRIMITIVE_TYPE(float)
IMPLEMENT_PRIMITIVE_TYPE(double)

IMPLEMENT_PRIMITIVE_TYPE(HString)