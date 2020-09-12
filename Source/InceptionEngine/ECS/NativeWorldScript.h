#pragma once

namespace inceptionengine
{
	class IWorld;

	class NativeWorldScript
	{
	public:
		virtual ~NativeWorldScript() = default;

		void Begin();

		void Tick();

	protected:
		IWorld* mWorld = nullptr;

	private:
		virtual void OnBegin();

		virtual void OnTick();
	};
}