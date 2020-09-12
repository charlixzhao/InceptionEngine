#include "NativeWorldScript.h"

namespace inceptionengine
{
	void NativeWorldScript::Begin()
	{
		OnBegin();
	}
	void NativeWorldScript::Tick()
	{
		OnTick();
	}

	void NativeWorldScript::OnBegin()
	{
	}

	void NativeWorldScript::OnTick()
	{
	}
}