#include "monopp/Method.h"
#include "monopp/MonoException.h"
#include "monopp/Type.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/debug-helpers.h>
END_MONO_INCLUDE

namespace Mono
{
Method::Method(MonoMethod* method)
{
	_method = method;
	if (!_method)
	{
		throw MonoException("NATIVE::Could not create method");
	}
	GenerateMeta();
}

Method::Method(const Type& type, const std::string& name_with_args)
{
	const auto desc = mono_method_desc_new((":" + name_with_args).c_str(), 0);
	_method = mono_method_desc_search_in_class(desc, type.GetInternalPtr());
	mono_method_desc_free(desc);

	if (!_method)
	{
		const auto& type_name = type.GetName();
		throw MonoException("NATIVE::Could not create method : " + name_with_args + " for class " + type_name);
	}
	GenerateMeta();
}

Method::Method(const Type& type, const std::string& name, int argc)
{
	_method = mono_class_get_method_from_name(type.GetInternalPtr(), name.c_str(), argc);

	if (!_method)
	{
		const auto& type_name = type.GetName();
		throw MonoException("NATIVE::Could not create method : " + name + " for class " + type_name);
	}
	GenerateMeta();
}

void Method::GenerateMeta()
{
	_signature = mono_method_signature(_method);
	_name = mono_method_get_name(_method);
	_fullname = mono_method_full_name(_method, true);
	const std::string storage = (IsStatic() ? " static " : " ");
	_fullDeclname = ToString(GetVisibility()) + storage + _fullname;
}

auto Method::GetReturnType() const -> Type
{
	const auto type = mono_signature_get_return_type(_signature);
	return Type(type);
}

std::vector<Type> Method::GetParamTypes() const
{
	void* iter = nullptr;
	auto type = mono_signature_get_params(_signature, &iter);
	std::vector<Type> params;
	while (type)
	{
		params.emplace_back(Type(type));

		type = mono_signature_get_params(_signature, &iter);
	}

	return params;
}

auto Method::GetName() const -> const std::string&
{
	return _name;
}

auto Method::GetFullname() const -> const std::string&
{
	return _fullname;
}

auto Method::GetFullDeclname() const -> const std::string&
{
	return _fullDeclname;
}

auto Method::GetVisibility() const -> Visibility
{
	const uint32_t flags = mono_method_get_flags(_method, nullptr) & MONO_METHOD_ATTR_ACCESS_MASK;

	if (flags == MONO_METHOD_ATTR_PRIVATE) return Visibility::Private;
	if (flags == MONO_METHOD_ATTR_FAM_AND_ASSEM) return Visibility::ProtectedInternal;
	if (flags == MONO_METHOD_ATTR_ASSEM) return Visibility::Internal;
	if (flags == MONO_METHOD_ATTR_FAMILY) return Visibility::Protected;
	if (flags == MONO_METHOD_ATTR_PUBLIC) return Visibility::Public;

	assert(false);

	return Visibility::Private;
}

auto Method::GetVirtualImpl(const Object& object) const -> Method
{
	return Method(mono_object_get_virtual_method(object.GetInternalPtr(), _method));
}

auto Method::IsStatic() const -> bool
{
	const uint32_t flags = mono_method_get_flags(_method, nullptr);
	return (flags & MONO_METHOD_ATTR_STATIC) != 0;
}

auto Method::IsVirtual() const -> bool
{
	const uint32_t flags = mono_method_get_flags(_method, nullptr);
	return (flags & MONO_METHOD_ATTR_VIRTUAL) != 0;
}
}
