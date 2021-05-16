#include "monopp/GcHandle.h"

namespace Mono
{
GcHandle::GcHandle(const Object& object) :
	Object(object)
{
}

void GcHandle::Lock()
{
	// If was moved
	if (_handle > 0) return;
	
	_handle = mono_gchandle_new(_object, 1);
}

void GcHandle::Unlock()
{
	// If was moved
	if (_handle <= 0) return;

	mono_gchandle_free(_handle);
	_handle = 0;
}

void GcHandle::Invalidate()
{
	_handle = 0;
}

ScopedGcHandle::ScopedGcHandle(GcHandle& handle):
	_handle(handle)
{
	_handle.Lock();
}

ScopedGcHandle::ScopedGcHandle(ScopedGcHandle&& other) noexcept :
	_handle(other._handle)
{
	other.Invalidate();
}

ScopedGcHandle::~ScopedGcHandle()
{
	if (!_invalid)
	{
		_handle.Unlock();
	}
}

auto ScopedGcHandle::GetHandle() const -> const GcHandle&
{
	return _handle;
}

void ScopedGcHandle::Invalidate()
{
	_invalid = true;
}
}
