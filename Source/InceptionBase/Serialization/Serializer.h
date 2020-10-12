#pragma once

#include "EngineBaseGlobals/EngineBaseApiPrefix.h"
#include "Math/Math.h"

#include "External/cereal/cereal.hpp"
#include "External/cereal/archives/binary.hpp"

#include "External/cereal/types/array.hpp"
#include "External/cereal/types/functional.hpp"
#include "External/cereal/types/list.hpp"
#include "External/cereal/types/map.hpp"
#include "External/cereal/types/memory.hpp"
#include "External/cereal/types/optional.hpp"
#include "External/cereal/types/polymorphic.hpp"
#include "External/cereal/types/queue.hpp"
#include "External/cereal/types/set.hpp"
#include "External/cereal/types/stack.hpp"
#include "External/cereal/types/string.hpp"
#include "External/cereal/types/tuple.hpp"
#include "External/cereal/types/unordered_map.hpp"
#include "External/cereal/types/unordered_set.hpp"
#include "External/cereal/types/variant.hpp"
#include "External/cereal/types/vector.hpp"


#include <string>
#include <fstream>
#include <memory>

namespace cereal
{
	template<typename Archive>
	void serialize(Archive& archive, inceptionengine::Vec4f& v)
	{
		archive(v.x, v.y, v.z, v.w);
	}

	template<typename Archive>
	void serialize(Archive& archive, inceptionengine::Vec3f& v)
	{
		archive(v.x, v.y, v.z);
	}

	template<typename Archive>
	void serialize(Archive& archive, inceptionengine::Vec4ui& v)
	{
		archive(v.x, v.y, v.z, v.w);
	}

	template<typename Archive>
	void serialize(Archive& archive, inceptionengine::Matrix4x4f& mat)
	{
		archive(mat.value);
	}


}
namespace inceptionengine
{
	class Serializer
	{
	public:
		template<typename T>
		static void Serailize(T const& t, std::string assetFilePath)
		{
			{
				std::ofstream ost(assetFilePath, std::ios::binary);
				cereal::BinaryOutputArchive archive(ost);
				archive(t);
			}
		}

		template<typename T>
		static std::shared_ptr<T> Deserailize(std::string assetFilePath)
		{
			std::shared_ptr<T> res = std::make_shared<T>();
			{
				std::ifstream ist(assetFilePath, std::ios::binary);
				cereal::BinaryInputArchive archive(ist);
				archive(*res);
			}
			return res;
		}
	};
	

}