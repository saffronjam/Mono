#pragma once

#include "Config.h"

#include "Assembly.h"

namespace Mono
{
auto Initialize(const std::string &mono_path, const std::string &mono_assembly_dir, const std::string &mono_config_dir, const std::string &domain,
                bool enable_debugging) -> bool;

void Shutdown();
}
