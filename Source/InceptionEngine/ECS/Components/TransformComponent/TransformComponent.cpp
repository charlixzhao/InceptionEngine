
#include "TransformComponent.h"

#include "Serialization/Serializer.h"


namespace inceptionengine
{

	TransformComponent::TransformComponent(TransformSystem& system, int x, int y)
		:mSystem(system), mX(x), mY(y)
	{
	}

	void TransformComponent::Hello()
	{
		Vec4f ss(2.0f,2.0f,2.0f,2.0f);

		Serializer::Serailize(ss, "C:/Users/Xiaoxiang Zhao/Desktop/testCereal/vec3.iasset");

		//Vec4f sss(1.0f, 1.0f, 1.0f, 1.0f);;
		//std::cout << v.x << v.y << v.z << std::endl;
		auto ptr = Serializer::Deserailize<Vec4f>("C:/Users/Xiaoxiang Zhao/Desktop/testCereal/vec3.iasset");


		std::cout << "Adder result is " << Adder(mX, mY) << std::endl;
	}
}

