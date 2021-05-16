#include "monopp/Domain.h"
#include "monopp/Assembly.h"

#include "monopp/String.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/appdomain.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/threads.h>
END_MONO_INCLUDE

namespace Mono
{
static const Domain* current_domain = nullptr;

MonoDomain* Domain::GetInternalPtr() const
{
	return _domain;
}

void Domain::SetCurrentDomain(const Domain& domain)
{
	current_domain = &domain;
}

auto Domain::GetCurrentDomain() -> const Domain&
{
	return *current_domain;
}

auto Domain::CreateString(const std::string& str) const -> String
{
	return String(*this, str);
}

Domain::Domain(const std::string& name)
{
	_domain = mono_domain_create_appdomain(const_cast<char*>(name.c_str()), nullptr);

	const auto res = mono_domain_set(_domain, 0);
	if (res)
	{
		mono_thread_attach(_domain);
	}
}

Domain::~Domain()
{
	if (_domain)
	{
		auto* const root_domain = mono_get_root_domain();
		const auto res = mono_domain_set(root_domain, 0);
		if (res)
		{
			mono_domain_unload(_domain);
		}
	}
	mono_gc_collect(mono_gc_max_generation());
}

void Domain::AttachThread()
{
	_threads.emplace(std::this_thread::get_id(), mono_thread_attach(_domain));
}

void Domain::DetachThread()
{
	mono_thread_detach(_threads.at(std::this_thread::get_id()));
	_threads.erase(std::this_thread::get_id());
}

Assembly Domain::GetAssembly(const std::string& path, bool forceReload) const
{
	const auto it = _assemblies.find(path);

	// If already exist
	if (it != _assemblies.end())
	{
		if (forceReload)
		{
			// Optional overwrite
			it->second = Assembly{*this, path};
			return it->second;
		}
		const auto& assembly = it->second;
		return assembly;
	}
	const auto res = _assemblies.emplace(path, Assembly{*this, path});

	const auto& assembly = res.first->second;

	return assembly;
}
}
