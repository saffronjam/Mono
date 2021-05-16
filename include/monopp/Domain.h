#pragma once

#include "Config.h"
#include "Assembly.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/metadata.h>
#include <mono/metadata/threads.h>
END_MONO_INCLUDE

#include <unordered_map>
#include <thread>

namespace Mono
{
class String;

class Domain
{
public:
	explicit Domain(const std::string& name);
	~Domain();

	void AttachThread();
	void DetachThread();

	auto GetAssembly(const std::string& path, bool forceReload = false) const -> Assembly;

	auto CreateString(const std::string& str) const -> String;

	static auto SetCurrentDomain(const Domain& domain) -> void;
	static auto GetCurrentDomain() -> const Domain&;

	auto GetInternalPtr() const -> MonoDomain*;

private:
	mutable std::unordered_map<std::string, Assembly> _assemblies;
	NonOwningPtr<MonoDomain> _domain = nullptr;
	std::unordered_map<std::thread::id, MonoThread*> _threads;
};
}
