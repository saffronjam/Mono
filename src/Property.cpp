#include "monopp/Property.h"
#include "monopp/Assembly.h"
#include "monopp/MonoException.h"
#include "monopp/Method.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/debug-helpers.h>
END_MONO_INCLUDE

namespace Mono
{
Property::Property(const Type& type, const std::string& name) :
	property_(mono_class_get_property_from_name(type.GetInternalPtr(), name.c_str())),
	name_(name)
{
	if (!property_) throw MonoException("NATIVE::Could not get property : " + name + " for class " + type.GetName());

	GenerateMeta();
}

auto Property::GetInternalPtr() const -> MonoProperty*
{
	return property_;
}

auto Property::GetName() const -> const std::string&
{
	return name_;
}

auto Property::GetFullname() const -> const std::string&
{
	return fullname_;
}

auto Property::GetFullDeclname() const -> const std::string&
{
	return full_declname_;
}

auto Property::GetType() const -> const Type&
{
	return type_;
}

auto Property::GetGetMethod() const -> Method
{
	const auto method = mono_property_get_get_method(property_);
	return Method(method);
}

auto Property::GetSetMethod() const -> Method
{
	const auto method = mono_property_get_set_method(property_);
	return Method(method);
}

auto Property::GetVisibility() const -> Visibility
{
	auto getter_vis = Visibility::Public;
	try
	{
		const auto getter = GetGetMethod();
		getter_vis = getter.GetVisibility();
	}
	catch (const MonoException&)
	{
	}
	auto setter_vis = Visibility::Public;
	try
	{
		const auto setter = GetSetMethod();
		setter_vis = setter.GetVisibility();
	}
	catch (const MonoException&)
	{
	}
	if (static_cast<int>(getter_vis) < static_cast<int>(setter_vis))
	{
		return getter_vis;
	}

	return setter_vis;
}

auto Property::IsStatic() const -> bool
{
	const auto getter = GetGetMethod();
	return getter.IsStatic();
}

void Property::GenerateMeta()
{
	const auto get_method = GetGetMethod();
	type_ = Type(get_method.GetReturnType());
	fullname_ = name_;
	const std::string storage = (IsStatic() ? " static " : " ");
	full_declname_ = ToString(GetVisibility()) + storage + fullname_;
}
}
