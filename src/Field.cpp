#include "monopp/Field.h"
#include "monopp/Domain.h"
#include "monopp/MonoException.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/debug-helpers.h>
END_MONO_INCLUDE

namespace Mono
{
Field::Field(const Type& type, const std::string& name) :
	_field(mono_class_get_field_from_name(type.GetInternalPtr(), name.c_str())),
	_name(name)
{
	if (!_field)
	{
		throw MonoException("NATIVE::Could not get field : " + name + " for class " + type.GetName());
	}
	const auto& domain = Domain::GetCurrentDomain();

	GenerateMeta();
	if (IsStatic())
	{
		_owningTypeVtable = mono_class_vtable(domain.GetInternalPtr(), type.GetInternalPtr());
		mono_runtime_class_init(_owningTypeVtable);
	}
}

void Field::GenerateMeta()
{
	auto* const type = mono_field_get_type(_field);
	_fieldType = ConvertToMonoFieldType(type);
	_type = Type(mono_class_from_mono_type(type));
	_fullname = mono_field_full_name(_field);
	const std::string storage = IsStatic() ? " static " : " ";
	_fullDeclname = ToString(GetVisibility()) + storage + _fullname;
}

auto Field::IsValuetype() const -> bool
{
	return GetType().IsValuetype();
}

auto Field::GetName() const -> const std::string&
{
	return _name;
}

auto Field::GetFullname() const -> const std::string&
{
	return _fullname;
}

auto Field::GetFullDeclname() const -> const std::string&
{
	return _fullDeclname;
}

auto Field::GetType() const -> const Type&
{
	return _type;
}

auto Field::GetVisibility() const -> Visibility
{
	const uint32_t flags = mono_field_get_flags(_field) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;

	if (flags == MONO_FIELD_ATTR_PRIVATE) return Visibility::Private;
	if (flags == MONO_FIELD_ATTR_FAM_AND_ASSEM) return Visibility::ProtectedInternal;
	if (flags == MONO_FIELD_ATTR_ASSEMBLY) return Visibility::Internal;
	if (flags == MONO_FIELD_ATTR_FAMILY) return Visibility::Protected;
	if (flags == MONO_FIELD_ATTR_PUBLIC) return Visibility::Public;

	assert(false);

	return Visibility::Private;
}

auto Field::GetFieldType() const -> FieldType
{
	return _fieldType;
}

auto Field::IsStatic() const -> bool
{
	const uint32_t flags = mono_field_get_flags(_field);

	return (flags & MONO_FIELD_ATTR_STATIC) != 0;
}

auto Field::ConvertToMonoFieldType(MonoType* monoType) -> FieldType
{
	const int type = mono_type_get_type(monoType);
	switch (type)
	{
	case MONO_TYPE_R4: return FieldType::Float;
	case MONO_TYPE_I4: return FieldType::Int;
	case MONO_TYPE_U4: return FieldType::Uint;
	case MONO_TYPE_STRING: return FieldType::String;
	case MONO_TYPE_VALUETYPE:
	{
		char* name = mono_type_get_name(monoType);
		if (strcmp(name, "Saffron.Vector2") == 0) return FieldType::Vec2;
		if (strcmp(name, "Saffron.Vector3") == 0) return FieldType::Vec3;
		return FieldType::Vec4;
	}
	default: return FieldType::None;
	}
}
}
