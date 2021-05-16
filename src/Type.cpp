#include "monopp/Type.h"
#include "monopp/Assembly.h"
#include "monopp/MonoException.h"

#include "monopp/Domain.h"
#include "monopp/Field.h"
#include "monopp/Method.h"
#include "monopp/Object.h"
#include "monopp/Property.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/debug-helpers.h>
END_MONO_INCLUDE

namespace Mono
{
Type::Type() = default;

Type::Type(MonoReflectionType* reflectionType) :
	_type(mono_reflection_type_get_type(reflectionType)),
	_class (mono_class_from_mono_type(_type))
{

	GenerateMeta();
}

Type::Type(MonoImage* image, const std::string& name) :
	Type(image, "", name)
{
}

Type::Type(MonoImage* image, const std::string& namespaceName, const std::string& name)
{
	_class = mono_class_from_name(image, namespaceName.c_str(), name.c_str());

	if (!_class) throw MonoException("NATIVE::Could not get class : " + namespaceName + "." + name);

	GenerateMeta();
}

Type::Type(MonoClass* cls)
{
	_class = cls;
	if (!_class) throw MonoException("NATIVE::Could not get class");

	GenerateMeta();
}

Type::Type(MonoType* type)
{
	_class = mono_class_from_mono_type(type);
	if (!_class) throw MonoException("NATIVE::Could not get class");

	GenerateMeta();
}

auto Type::operator==(const Type& rhs) const -> bool
{
	return GetFullname() == rhs.GetFullname();
}

auto Type::Valid() const -> bool
{
	return _class != nullptr;
}

auto Type::NewInstance() const -> Object
{
	const auto& domain = Domain::GetCurrentDomain();
	return Object(domain, *this);
}

auto Type::GetMethod(const std::string& name_with_args) const -> Method
{
	return Method(*this, name_with_args);
}

auto Type::GetMethod(const std::string& name, int argc) const -> Method
{
	return Method(*this, name, argc);
}

auto Type::GetField(const std::string& name) const -> Field
{
	return Field(*this, name);
}

auto Type::GetProperty(const std::string& name) const -> Property
{
	return Property(*this, name);
}

auto Type::GetFields() const -> std::vector<Field>
{
	void* iter = nullptr;
	auto field = mono_class_get_fields(_class, &iter);
	std::vector<Field> fields;
	while (field)
	{
		std::string name = mono_field_get_name(field);

		fields.emplace_back(GetField(name));

		field = mono_class_get_fields(_class, &iter);
	}
	return fields;
}

auto Type::GetProperties() const -> std::vector<Property>
{
	void* iter = nullptr;
	auto prop = mono_class_get_properties(_class, &iter);
	std::vector<Property> props;
	while (prop)
	{
		std::string name = mono_property_get_name(prop);

		props.emplace_back(GetProperty(name));

		prop = mono_class_get_properties(_class, &iter);
	}
	return props;
}

auto Type::GetMethods() const -> std::vector<Method>
{
	void* iter = nullptr;
	auto method = mono_class_get_methods(_class, &iter);
	std::vector<Method> methods;

	while (method != nullptr)
	{
		const auto sig = mono_method_signature(method);
		std::string signature = mono_signature_get_desc(sig, false);
		std::string name = mono_method_get_name(method);
		std::string fullname = name + "(" + signature + ")";
		methods.emplace_back(GetMethod(fullname));
		method = mono_class_get_methods(_class, &iter);
	}

	return methods;
}

auto Type::HasBaseType() const -> bool
{
	return mono_class_get_parent(_class) != nullptr;
}

auto Type::GetBaseType() const -> Type
{
	const auto base = mono_class_get_parent(_class);
	return Type(base);
}

auto Type::GetNestedTypes() const -> std::vector<Type>
{
	void* iter = nullptr;
	const auto nested = mono_class_get_nested_types(_class, &iter);
	std::vector<Type> nested_clases;
	while (nested)
	{
		nested_clases.emplace_back(Type(nested));
	}
	return nested_clases;
}

auto Type::GetInternalPtr() const -> MonoClass*
{
	return _class;
}

auto Type::Exists(MonoImage* image, const std::string& name) -> bool
{
	return Exists(image, "", name);
}

auto Type::Exists(MonoImage* image, const std::string& namespaceName, const std::string& name) -> bool
{
	return mono_class_from_name(image, namespaceName.c_str(), name.c_str());
}

void Type::GenerateMeta()
{
	_type = mono_class_get_type(_class);
	_namespace = mono_class_get_namespace(_class);
	_name = mono_class_get_name(_class);
	_fullname = _namespace.empty() ? _name : _namespace + "." + _name;
	_rank = mono_class_get_rank(_class);
	_valuetype = !!mono_class_is_valuetype(_class);
	_sizeOf = static_cast<std::uint32_t>(mono_class_value_size(_class, &_alignOf));
}

auto Type::IsDerivedFrom(const Type& type) const -> bool
{
	return mono_class_is_subclass_of(_class, type.GetInternalPtr(), false) != 0;
}

auto Type::GetNamespace() const -> const std::string&
{
	return _namespace;
}

auto Type::GetName() const -> const std::string&
{
	return _name;
}

auto Type::GetFullname() const -> const std::string&
{
	return _fullname;
}

auto Type::IsValuetype() const -> bool
{
	return _valuetype;
}

auto Type::GetRank() const -> int
{
	return _rank;
}

uint32_t Type::GetSizeof() const
{
	return _sizeOf;
}

uint32_t Type::GetAlignof() const
{
	return _alignOf;
}
}
