#pragma once

#include <string>

namespace inceptionengine
{
	struct ShaderPath
	{
		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(vertexShaderPath, fragmentShaderPath);
		}

		std::string vertexShaderPath;
		std::string fragmentShaderPath;
	};
}