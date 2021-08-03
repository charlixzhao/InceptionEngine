
#include "PathHelper.h"
#include "MatchingDatabaseBuilder.h"

#include <string>
#include <iostream>
#include <filesystem>

using namespace inceptionengine;
using namespace inceptionengine::animeditor;

int main()
{
	std::string enginePath = PathHelper::GetEnginePath(std::filesystem::current_path().string());
	PathHelper::SetEngineDirectory(enginePath);

	std::string animPath;
	std::string dbSavePath;
	std::cout << "Enter the path of the anim file, without extension: ";
	std::getline(std::cin, animPath);
	std::string dir = std::filesystem::path(PathHelper::GetAbsolutePath(animPath)).parent_path().string();

	std::string dbName;
	std::cout << "Enter the name of the db file: ";
	std::getline(std::cin, dbName);
	
	MatchingDatabaseBuilder builder;
	builder.ConstructFromAnim(animPath, dir + "/" + dbName + ".ie_mmdb");
	std::cout << "Constrution finished\n";

}