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

class Property
{
public:
	Property() = default;
	explicit Property(const Type& type, const std::string& name);

	auto GetName() const -> const std::string&;
	auto GetFullname() const -> const std::string&;
	auto GetFullDeclname() const -> const std::string&;

	auto GetType() const -> const Type&;

	auto GetGetMethod() const -> Method;
	auto GetSetMethod() const -> Method;

	auto GetVisibility() const -> Visibility;
	auto IsStatic() const -> bool;

	auto GetInternalPtr() const -> MonoProperty*;

private:
	void GenerateMeta();

private:
	Type type_;

	NonOwningPtr<MonoProperty> property_ = nullptr;

	std::string name_;
	std::string fullname_;
	std::string full_declname_;
};
}
