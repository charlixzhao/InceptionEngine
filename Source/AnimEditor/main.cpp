
#include "PathHelper.h"
#include "MatchingDatabaseBuilder.h"

#include <string>
#include <iostream>
#include <filesystem>

using namespace inceptionengine;
using namespace inceptionengine::animeditor;

#define IMPORT 1
#define SPLIT 0

#if IMPORT



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
		PathHelper::GetAbsolutePath("StandAloneResource/humanoid/walk1_0"),
		PathHelper::GetAbsolutePath("StandAloneResource/humanoid/walk1_1"),
		PathHelper::GetAbsolutePath("StandAloneResource/humanoid/walk1_2"),
		PathHelper::GetAbsolutePath("StandAloneResource/humanoid/walk1_3"),
		PathHelper::GetAbsolutePath("StandAloneResource/humanoid/walk1_4"),
		PathHelper::GetAbsolutePath("StandAloneResource/humanoid/walk2_0"),
		PathHelper::GetAbsolutePath("StandAloneResource/humanoid/walk2_1"),
		PathHelper::GetAbsolutePath("StandAloneResource/humanoid/walk2_3"),
		PathHelper::GetAbsolutePath("StandAloneResource/humanoid/walk3_0"),
		PathHelper::GetAbsolutePath("StandAloneResource/humanoid/walk3_1")
	};
	std::string saveFile = PathHelper::GetAbsolutePath("StandAloneResource/humanoid/walk.ie_mmdb");
	
	
	/*
std::vector<std::string> featureBones =
	{
		"Model:LeftFoot",
		"Model:RightFoot",
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

#endif // IMPORT


#if  SPLIT
#include "Serialization/Serializer.h"
#include "RunTime/Animation/Animation.h"
class AnimSpiltter
{
public:
	struct Range
	{
		int from;
		int to;
	};
	void Split(std::string const& animFile, std::vector<Range> const& ranges)
	{
		auto rawAnim = Serializer::Deserailize<Animation>(PathHelper::GetAbsolutePath(animFile) + ".ie_anim");
		for (int i = 0; i < ranges.size(); i++)
		{
			Animation anim;
			anim.mDuration = static_cast<float>(ranges[i].to - ranges[i].from) / 30.0f;
			anim.mName = anim.mName + "_" + std::to_string(i);
			anim.mPathToSkeleton = rawAnim->mPathToSkeleton;
			for (int frame = ranges[i].from; frame <= ranges[i].to; frame++)
			{
				anim.mBoneTransforms.push_back(rawAnim->mBoneTransforms[frame]);
				anim.mBoneGlobalTranslVelocities.push_back(rawAnim->mBoneGlobalTranslVelocities[frame]);
				anim.mBoneLclTranslVelocities.push_back(rawAnim->mBoneLclTranslVelocities[frame]);
				anim.mBoneGlobalAngularVelocities.push_back(rawAnim->mBoneGlobalAngularVelocities[frame]);
				anim.mBoneLclAngularVelocities.push_back(rawAnim->mBoneLclAngularVelocities[frame]);
			}

			Serializer::Serailize<Animation>(anim, PathHelper::GetAbsolutePath(animFile) + "_" + std::to_string(i) + ".ie_anim");

		}
	}
private:

};

int main()
{
	std::string enginePath = PathHelper::GetEnginePath(std::filesystem::current_path().string());
	PathHelper::SetEngineDirectory(enginePath);

	{
		AnimSpiltter::Range r1 = { 102, 1300 };
		AnimSpiltter::Range r2 = { 2490, 3600 };
		AnimSpiltter::Range r3 = { 3600, 4400 };
		AnimSpiltter::Range r4 = { 6000, 6300 };
		AnimSpiltter::Range r5 = { 102, 1300 };
		AnimSpiltter s;
		s.Split("StandAloneResource/humanoid/walk1", { r1, r2, r3,r4, r5 });
	}


	{
		AnimSpiltter::Range r1 = { 97, 700 };
		AnimSpiltter::Range r2 = { 2248, 2930 };
		AnimSpiltter::Range r3 = { 4643, 5168 };
		AnimSpiltter::Range r4 = { 6277, 7116 };
		AnimSpiltter s;
		s.Split("StandAloneResource/humanoid/walk2", { r1, r2, r3,r4, });
	}

	{
		AnimSpiltter::Range r1 = { 94, 1990 };
		AnimSpiltter::Range r2 = { 6679, 7336 };
		AnimSpiltter s;
		s.Split("StandAloneResource/humanoid/walk3", { r1, r2 });
	}

}

#endif //  SPLIT

