#include <utility>

#include "monopp/Object.h"
#include "monopp/Domain.h"

namespace Mono
{
Object::Object(MonoObject* object) :
	_type(mono_object_get_class(object)),
	_object(object)
{
}

Object::Object(const Domain& domain, Type type) :
	_type(std::move(type)),
	_object(mono_object_new(domain.GetInternalPtr(), type.GetInternalPtr()))
{
	mono_runtime_object_init(_object);
}

auto Object::GetType() const -> const Type&
{
	return _type;
}

auto Object::Valid() const -> bool
{
	return _object != nullptr;
}

auto Object::GetInternalPtr() const -> MonoObject*
{
	return _object;
}
}
