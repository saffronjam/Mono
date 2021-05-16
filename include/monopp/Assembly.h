#pragma once

#include "Config.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/image.h>
END_MONO_INCLUDE

#include <unordered_map>

namespace Mono
{
class Domain;
class Type;

class Assembly
{
public:
	Assembly() = default;
	explicit Assembly(const Domain& domain, const std::string& path);

	auto GetType(const std::string& name) const -> const Type&;
	auto GetType(const std::string& namespaceName, const std::string& name) const -> const Type&;

	auto TypeExists(const std::string& name) const -> bool;
	auto TypeExists(const std::string& namespaceName, const std::string& name) const -> bool;

	auto DumpReferences() const -> std::vector<std::string>;

	auto GetInternalAssemblyPtr() const -> MonoAssembly*;
	auto GetInternalImagePtr() const -> MonoImage*;

private:
	NonOwningPtr<MonoAssembly> _assembly = nullptr;
	NonOwningPtr<MonoImage> _image = nullptr;

	mutable std::unordered_map<std::string, Type> _types;
};
}
