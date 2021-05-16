#pragma once

#include "Config.h"

#include "Object.h"

namespace Mono
{
class GcHandle : public Object
{
public:
	explicit GcHandle(const Object& object);
	using Object::Object;

	void Lock();
	void Unlock();

	void Invalidate();

private:
	std::uint32_t _handle = 0;
};

class ScopedGcHandle
{
public:
	explicit ScopedGcHandle(GcHandle& handle);
	explicit ScopedGcHandle(ScopedGcHandle&& other) noexcept;
	~ScopedGcHandle();

	auto GetHandle() const -> const GcHandle&;
	void Invalidate();

private:
	GcHandle& _handle;
	bool _invalid = false;
	int test = 0;
};
}
