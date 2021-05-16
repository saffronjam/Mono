#pragma once

#include "Config.h"

#include "Object.h"
#include "Visibility.h"

namespace Mono
{
class Object;
class Type;

class Method
{
public:
	Method() = default;
	explicit Method(MonoMethod* method);
	explicit Method(const Type& type, const std::string& name_with_args);
	explicit Method(const Type& type, const std::string& name, int argc);

	auto GetReturnType() const -> Type;
	auto GetParamTypes() const -> std::vector<Type>;
	auto GetName() const -> const std::string&;
	auto GetFullname() const -> const std::string&;
	auto GetFullDeclname() const -> const std::string&;
	auto GetVisibility() const -> Visibility;
	auto GetVirtualImpl(const Object& object) const -> Method;

	auto IsStatic() const -> bool;
	auto IsVirtual() const -> bool;

protected:
	void GenerateMeta();

	NonOwningPtr<MonoMethod> _method = nullptr;
	NonOwningPtr<MonoMethodSignature> _signature = nullptr;
	std::string _name;
	std::string _fullname;
	std::string _fullDeclname;
};
}
