#pragma once

#include "Config.h"

#include "Visibility.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/class.h>
#include <mono/metadata/image.h>
#include <mono/metadata/reflection.h>
END_MONO_INCLUDE

namespace Mono
{
class Assembly;
class Method;
class Field;
class Property;
class Object;

class Type
{
public:
	Type();
	explicit Type(MonoReflectionType* reflectionType);
	explicit Type(MonoImage* image, const std::string& name);
	explicit Type(MonoImage* image, const std::string& namespaceName, const std::string& name);
	explicit Type(MonoClass* cls);
	explicit Type(MonoType* type);

	auto operator==(const Type& rhs) const -> bool;

	auto Valid() const -> bool;

	auto NewInstance() const -> Object;

	auto GetMethod(const std::string& name_with_args) const -> Method;
	auto GetMethod(const std::string& name, int argc) const -> Method;
	auto GetField(const std::string& name) const -> Field;
	auto GetProperty(const std::string& name) const -> Property;

	auto GetFields() const -> std::vector<Field>;
	auto GetProperties() const -> std::vector<Property>;
	auto GetMethods() const -> std::vector<Method>;

	auto HasBaseType() const -> bool;
	auto GetBaseType() const -> Type;
	auto GetNestedTypes() const -> std::vector<Type>;

	auto IsDerivedFrom(const Type& type) const -> bool;

	auto GetNamespace() const -> const std::string&;
	auto GetName() const -> const std::string&;
	auto GetFullname() const -> const std::string&;

	auto IsValuetype() const -> bool;

	auto GetRank() const -> int;
	auto GetSizeof() const -> std::uint32_t;
	auto GetAlignof() const -> std::uint32_t;

	auto GetInternalPtr() const -> MonoClass*;

	static auto Exists(MonoImage* image, const std::string& name) -> bool;
	static auto Exists(MonoImage* image, const std::string& namespaceName, const std::string& name) -> bool;

private:
	void GenerateMeta();

private:
	NonOwningPtr<MonoType> _type = nullptr;
	NonOwningPtr<MonoClass> _class = nullptr;

	std::string _namespace;
	std::string _name;
	std::string _fullname;

	std::uint32_t _sizeOf = 0;
	std::uint32_t _alignOf = 0;

	int _rank = 0;
	bool _valuetype = true;
};
}
