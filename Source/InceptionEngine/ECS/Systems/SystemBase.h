
#pragma once

namespace inceptionengine
{
	class ComponentsPool;

	class SystemBase
	{
	public:
		explicit SystemBase(ComponentsPool& componentsPool);

		virtual ~SystemBase() = default;

	protected:
		ComponentsPool& mComponentsPool;
	};
}