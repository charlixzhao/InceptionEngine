#include "IE_PCH.h"
#include "MatchingDatabase.h"
#include "RunTime/Animation/Animation.h"
#include "RunTime/SkeletalMesh/Skeleton.h"

namespace inceptionengine
{
	std::pair<int, int> MatchingDatabase::Query(MatchingFeature const& f1)
	{
		float minCost = std::numeric_limits<float>().max();
		int animIndex = -1;
		int argmin = -1;

		for (int anim = 0; anim < animPaths.size(); anim++)
		{

			for (int i = 0; auto const& f : features[anim])
			{

				float cost = FeatureDistance(f, f1, anim);
				if (cost < minCost)
				{
					minCost = cost;
					argmin = i;
					animIndex = anim;
				}
				i++;
			}
		}
		

		printf("argmin is in %d, %d with cost %f\n", animIndex, argmin, minCost);

		return { animIndex, argmin };
	}


	/*
	float MatchingDatabase::FeatureDistance(MatchingFeature const& f1, MatchingFeature const& f2, int animIndex)
	{
		int boneNumber = featureBones.size();
		float weight = static_cast<float>(boneNumber) / static_cast<float>(MatchingFeature::NPoints);
		float sum = 0.0f;
		
		sum += Distance(f1.currentFacing / currentFacingSD, f2.currentFacing / currentFacingSD);

		for (int i = 0; i < MatchingFeature::NPoints; i++)
		{
			sum += 3.0f * weight * Distance(f1.futureTrajectory[i] / trajectorySD[i], f2.futureTrajectory[i] / trajectorySD[i]);
			sum += 3.0f * weight * Distance(f1.futureFacing[i] / facingDirectionSD[i], f2.futureFacing[i] / facingDirectionSD[i]);
		}

		for (int i = 0; i < featureBones.size(); i++)
		{
			if (featureBonePosSDs[i] != Vec3f(0.0f, 0.0f, 0.0f))
			{
				sum += Distance(f1.featureBonePos[i] / featureBonePosSDs[i],
					f2.featureBonePos[i] / featureBonePosSDs[i]);
			}


			sum += Distance(f1.featureBoneVels[i] / featureBoneVelSDs[i], 
				f2.featureBoneVels[i] / featureBoneVelSDs[i]);
		}
		return sum;
	}*/

	float MatchingDatabase::FeatureDistance(MatchingFeature const& f1, MatchingFeature const& f2, int animIndex)
	{
		int boneNumber = featureBones.size();
		float weight = static_cast<float>(boneNumber) / static_cast<float>(MatchingFeature::NPoints);
		float sum = 0.0f;

		if (RadsBetween(f1.currentFacing, f2.currentFacing) > (PI / 4.0f))
		{
			return std::numeric_limits<float>::max();
		}

		sum += static_cast<float>(MatchingFeature::NPoints) * RadsBetween(f1.currentFacing, f2.currentFacing);
		
		for (int i = 0; i < MatchingFeature::NPoints; i++)
		{
			sum += Distance(f1.futureTrajectory[i], f2.futureTrajectory[i]);
			sum += 60.0f * RadsBetween(f1.futureFacing[i], f2.futureFacing[i]);
		}

		for (int i = 0; i < featureBones.size(); i++)
		{
			sum += Distance(f1.featureBonePos[i], f2.featureBonePos[i]) / 5.0f;
			sum += Distance(f1.featureBoneVels[i], f2.featureBoneVels[i]) / 5.0f;
		}

		MatchingFeature& tt = features[0][802];

		return sum;
	}


}

