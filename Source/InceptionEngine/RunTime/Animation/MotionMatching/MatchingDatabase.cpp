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



	float MatchingDatabase::FeatureDistance(MatchingFeature const& f1, MatchingFeature const& f2, int animIndex)
	{
		int boneNumber = featureBones.size();
		float weight = static_cast<float>(boneNumber) / static_cast<float>(MatchingFeature::NPoints);
		float sum = 0.0f;
		
		for (int i = 0; i < MatchingFeature::NPoints; i++)
		{
			sum += weight * Distance(f1.trajectory[i] / trajectorySD[i], f2.trajectory[i] / trajectorySD[i]);
			sum += weight * Distance(f1.facingDirection[i] / facingDirectionSD[i], f2.facingDirection[i] / facingDirectionSD[i]);
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
	}


}

