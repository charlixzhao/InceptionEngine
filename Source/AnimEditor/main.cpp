
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

	/*
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
	std::cout << "Constrution finished\n";*/

	std::vector<std::string> animFiles = 
	{ 
		PathHelper::GetAbsolutePath("StandAloneResource/humanoid/walk1"),
		PathHelper::GetAbsolutePath("StandAloneResource/humanoid/walk2"),
		PathHelper::GetAbsolutePath("StandAloneResource/humanoid/walk3"),
		PathHelper::GetAbsolutePath("StandAloneResource/humanoid/walk4")
	};
	std::string saveFile = PathHelper::GetAbsolutePath("StandAloneResource/humanoid/walk.ie_mmdb");
	
	/*
	std::vector<std::string> featureBones =
	{
		"Model:LeftFoot",
		"Model:RightFoot",
		"Model:RightHand",
		"Model:LeftHand",
		"Model:Hips"
	};*/

	
	/*
	std::vector<std::string> featureBones =
	{
"Model:Hips",
"Model:Spine",
"Model:RightUpLeg",
"Model:LeftUpLeg",
"Model:Spine1",
"Model:RightLeg",
"Model:LeftLeg",
"Model:Spine2",
"Model:RightFoot",
"Model:LeftFoot",
"Model:Neck",
"Model:RightShoulder",
"Model:LeftShoulder",
"Model:RightToe",
"Model:LeftToe",
"Model:Head",
"Model:RightArm",
"Model:LeftArm",
"Model:RightToeEnd",
"Model:LeftToeEnd",
"Model:RightForeArm",
"Model:LeftForeArm",
"Model:RightHand",
"Model:LeftHand"
	};*/

	/*
	std::vector<std::string> featureBones =
	{
"Model:Hips",
"Model:Spine",
"Model:RightUpLeg",
"Model:LeftUpLeg",
"Model:RightLeg",
"Model:LeftLeg",
"Model:RightFoot",
"Model:LeftFoot"
	}; */

	
	std::vector<std::string> featureBones =
	{
"Model:Hips",
"Model:Spine",
"Model:RightUpLeg",
"Model:LeftUpLeg",
"Model:Spine1",
"Model:RightLeg",
"Model:LeftLeg",
"Model:Spine2",
"Model:RightFoot",
"Model:LeftFoot",
"Model:Neck",
"Model:RightShoulder",
"Model:LeftShoulder",
"Model:Head",
"Model:RightArm",
"Model:LeftArm",
"Model:RightForeArm",
"Model:LeftForeArm",
"Model:RightHand",
"Model:LeftHand"
	};



	std::cout << "Job start\n";
	MatchingDatabaseBuilder builder;
	builder.ConstructFromAnim(animFiles, saveFile, featureBones);
	std::cout << "Job finish\n";

}