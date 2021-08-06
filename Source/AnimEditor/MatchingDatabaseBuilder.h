#pragma once

#include <string>
#include <vector>

namespace inceptionengine::animeditor
{
	class MatchingDatabaseBuilder
	{
	public:
		void ConstructFromAnim(std::vector<std::string> const& animFiles, 
			std::string const& dbSavePath, std::vector<std::string> const& featureBones);

	private:

	};
}