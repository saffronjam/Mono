#pragma once

#include "Config.h"
#include "Type.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/object.h>
END_MONO_INCLUDE

namespace Mono
{
class Domain;

class Object
{
public:
	explicit Object(MonoObject* object);
	explicit Object(const Domain& domain, Type type);
	
	auto GetType() const -> const Type&;

	auto Valid() const -> bool;

	auto GetInternalPtr() const -> MonoObject*;

protected:
	Type _type;
	NonOwningPtr<MonoObject> _object = nullptr;
};
}
