#include "monopp/Assembly.h"
#include "monopp/Domain.h"
#include "monopp/MonoException.h"

#include "monopp/String.h"
#include "monopp/Type.h"
#include <sstream>

BEGIN_MONO_INCLUDE
#include <mono/metadata/appdomain.h>
#include <mono/metadata/assembly.h>
END_MONO_INCLUDE

namespace Mono
{
Assembly::Assembly(const Domain& domain, const std::string& path)
{
	_assembly = mono_domain_assembly_open(domain.GetInternalPtr(), path.c_str());

	if (!_assembly) throw MonoException("NATIVE::Could not open assembly with path : " + path);

	_image = mono_assembly_get_image(_assembly);
}

auto Assembly::GetType(const std::string& name) const -> const Type&
{
	return GetType("", name);
}

auto Assembly::GetType(const std::string& namespaceName, const std::string& name) const -> const Type&
{
	const auto& it = _types.find(name);
	if (it == _types.end())
	{
		auto newType = Type(_image, namespaceName, name);
		const auto [pair, didEmplace] = _types.emplace(namespaceName + "." + name, newType);
		return pair->second;
	}
	return it->second;
}

auto Assembly::TypeExists(const std::string& name) const -> bool
{
	return Type::Exists(_image, name);
}

auto Assembly::TypeExists(const std::string& namespaceName, const std::string& name) const -> bool
{
	return Type::Exists(_image, namespaceName, name);
}

auto Assembly::DumpReferences() const -> std::vector<std::string>
{
	std::vector<std::string> refs;
	/* Get a pointer to the AssemblyRef metadata table */
	const auto table_info = mono_image_get_table_info(_image, MONO_TABLE_ASSEMBLYREF);

	/* Fetch the number of rows available in the table */
	const int rows = mono_table_info_get_rows(table_info);

	refs.reserve(static_cast<size_t>(rows));
	/* For each row, print some of its values */
	for (int i = 0; i < rows; i++)
	{
		/* Space where we extract one row from the metadata table */
		uint32_t cols[MONO_ASSEMBLYREF_SIZE];

		/* Extract the row into the array cols */
		mono_metadata_decode_row(table_info, i, cols, MONO_ASSEMBLYREF_SIZE);

		std::stringstream s;
		s << i + 1;
		s << " Version=";
		s << cols[MONO_ASSEMBLYREF_MAJOR_VERSION];
		s << ".";
		s << cols[MONO_ASSEMBLYREF_MINOR_VERSION];
		s << ".";
		s << cols[MONO_ASSEMBLYREF_BUILD_NUMBER];
		s << ".";
		s << cols[MONO_ASSEMBLYREF_REV_NUMBER];
		s << "\n\tName=";
		s << mono_metadata_string_heap(_image, cols[MONO_ASSEMBLYREF_NAME]);

		refs.emplace_back(s.str());
	}

	return refs;
}

auto Assembly::GetInternalAssemblyPtr() const -> MonoAssembly*
{
	return _assembly;
}

auto Assembly::GetInternalImagePtr() const -> MonoImage*
{
	return _image;
}
}
