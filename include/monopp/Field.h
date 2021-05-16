#pragma once

#include "Config.h"

#include "Type.h"
#include "Visibility.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/object.h>
END_MONO_INCLUDE

namespace Mono
{
class Object;

enum class FieldType
{
	None = 0,
	Float,
	Int,
	Uint,
	String,
	Vec2,
	Vec3,
	Vec4
};

class Field
{
public:
	Field() = default;
	explicit Field(const Type& type, const std::string& name);

	auto GetName() const -> const std::string&;
	auto GetFullname() const -> const std::string&;
	auto GetFullDeclname() const -> const std::string&;

	auto GetType() const -> const Type&;
	auto GetVisibility() const -> Visibility;
	auto GetFieldType() const -> FieldType;

	auto IsStatic() const -> bool;
	
	static auto ConvertToMonoFieldType(MonoType* monoType) -> FieldType;

protected:
	void GenerateMeta();
	auto IsValuetype() const -> bool;


protected:
	Type _type;
	FieldType _fieldType;

	NonOwningPtr<MonoClassField> _field = nullptr;
	NonOwningPtr<MonoVTable> _owningTypeVtable = nullptr;

	std::string _name;
	std::string _fullname;
	std::string _fullDeclname;
};
}
