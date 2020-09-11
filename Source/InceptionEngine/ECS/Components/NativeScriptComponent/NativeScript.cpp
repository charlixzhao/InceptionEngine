#include "NativeScript.h"

namespace inceptionengine
{
	NativeScript::NativeScript(Entity const& entity)
		:mEntity(entity)
	{
	}

	void NativeScript::Begin()
	{
		OnBegin();
	}

	void NativeScript::Tick()
	{
		OnTick();
	}
	void NativeScript::Destroy()
	{
		OnDestroy();
	}

	void NativeScript::OnBegin()
	{
	}

	void NativeScript::OnTick()
	{
	}

	void NativeScript::OnDestroy()
	{
	}


}

