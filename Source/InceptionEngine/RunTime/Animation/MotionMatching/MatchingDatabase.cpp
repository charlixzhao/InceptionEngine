#include "IE_PCH.h"
#include "MatchingDatabase.h"
#include "RunTime/Animation/Animation.h"
#include "RunTime/SkeletalMesh/Skeleton.h"

#include "Timer.hpp"

#define MULTI_THREAD 1

namespace inceptionengine
{

	std::pair<int, int> MatchingDatabase::Query(MatchingFeature const& f1) const
	{

#if MULTI_THREAD
		//auto nThread = std::thread::hardware_concurrency();
		////test
		/*
		Timer testThread;
		testThread.Reset();
		std::vector<std::thread> testThreads;
		testThreads.reserve(4);
		for (int i = 0; i < 4; i++)
		{
			testThreads.emplace_back([]() {; });
		}
		for (int i = 0; i < 4; i++)
		{
			testThreads[i].join();
		}
		auto testThreadTime = testThread.Count<Timer::Nanosecond>();
		std::cout << "test Time is " << testThreadTime << std::endl;
		*/
		//// 

		Timer timer;
		timer.Reset();
		//auto nThreads = std::thread::hardware_concurrency();
		auto nThreads = 2;
		int const nAnim = animPaths.size();
		int const workload = nAnim / nThreads + (nAnim % nThreads != 0);
		std::vector<std::thread> threads;
		threads.reserve(nThreads);
		std::vector<float> minCosts;
		std::vector<int> argmins;
		minCosts.resize(nAnim);
		argmins.resize(nAnim);

		for (int threadIdx = 0; threadIdx < nThreads; threadIdx++)
		{
			threads.emplace_back([this, nAnim, workload, &minCosts, &argmins, &f1](int threadIdx)
			{
				int animStartPos = threadIdx * workload;
				for (int anim = animStartPos; anim < workload + animStartPos && anim < nAnim; anim++)
				{
					float minCost = std::numeric_limits<float>().max();
					int argmin = -1;
					auto const& animFeatures = features[anim];
					for (int i = 0; i < animFeatures.size(); i++)
					{
						float cost = FeatureDistance(animFeatures[i], f1, anim);
						if (cost < minCost)
						{
							minCost = cost;
							argmin = i;
						}
					}
					minCosts[anim] = minCost;
					argmins[anim] = argmin;
				}
			}, threadIdx);
		}
		

		for (auto& thread : threads) { thread.join(); }

		int animIndex = std::min_element(minCosts.begin(), minCosts.end()) - minCosts.begin();
		auto time = timer.Count<Timer::Nanosecond>();
		std::cout << "Time is " << time << std::endl;
		//12264300
		assert(minCosts[animIndex] != std::numeric_limits<float>::max());
		printf("argmin is in %d, %d with cost %f\n", animIndex, argmins[animIndex], minCosts[animIndex]);

		return { animIndex, argmins[animIndex] };
#else
		Timer timer;
		timer.Reset();
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
		auto time = timer.Count<Timer::Nanosecond>();
		std::cout << "Time is " << time << std::endl;

		assert(minCost != std::numeric_limits<float>::max());
		printf("argmin is in %d, %d with cost %f\n", animIndex, argmin, minCost);

		//7108500
		//12079000
		return { animIndex, argmin };

#endif
	}


	/*
	float MatchingDatabase::FeatureDistance(MatchingFeature const& f1, MatchingFeature const& f2, int animIndex) const
	{
		if (RadsBetween(f1.currentFacing, f2.currentFacing) > (PI / 4.0f) ||
			RadsBetween(f1.futureFacing[MatchingFeature::NPoints - 1], f2.futureFacing[MatchingFeature::NPoints - 1]) > (PI / 4.0f))
		{
			return std::numeric_limits<float>::max();
		}

		//int boneNumber = featureBones.size();
		//float weight = static_cast<float>(boneNumber) / static_cast<float>(MatchingFeature::NPoints);


		float sum = 0.0f;

		sum += Distance((f1.currentFacing / currentFacingSD),
			(f2.currentFacing/ currentFacingSD));

		for (int i = 0; i < MatchingFeature::NPoints; i++)
		{
			sum += Distance((f1.futureTrajectory[i] / trajectorySD[i]),
				(f2.futureTrajectory[i] / trajectorySD[i]));

			sum += Distance((f1.futureFacing[i] / facingDirectionSD[i]),
				(f2.futureFacing[i] / facingDirectionSD[i]));
		}

		for (int i = 0; i < featureBones.size(); i++)
		{
			if (featureBonePosSDs[i] != Vec3f(0.0f, 0.0f, 0.0f))
			{
				sum += 0.5 * Distance((f1.featureBonePos[i] / featureBonePosSDs[i]),
					(f2.featureBonePos[i] / featureBonePosSDs[i]));
			}

			sum +=  Distance((f1.featureBoneVels[i]/ featureBoneVelSDs[i]),
				(f2.featureBoneVels[i] / featureBoneVelSDs[i]));
		}
		return sum;
	}*/

	
	float MatchingDatabase::FeatureDistance(MatchingFeature const& f1, MatchingFeature const& f2, int animIndex) const
	{
		int boneNumber = featureBones.size();
		float weight = static_cast<float>(boneNumber) / static_cast<float>(MatchingFeature::NPoints);
		float sum = 0.0f;

		if (RadsBetween(f1.currentFacing, f2.currentFacing) > (PI / 4.0f) ||
			RadsBetween(f1.futureFacing[MatchingFeature::NPoints - 1], f2.futureFacing[MatchingFeature::NPoints - 1]) > (PI / 4.0f))
		{
			return std::numeric_limits<float>::max();
		}

		sum += static_cast<float>(MatchingFeature::NPoints) * RadsBetween(f1.currentFacing, f2.currentFacing);
		
		for (int i = 0; i < MatchingFeature::NPoints; i++)
		{
			sum += 3.0f * Distance(f1.futureTrajectory[i], f2.futureTrajectory[i]);
			sum += RadsBetween(f1.futureFacing[i], f2.futureFacing[i]);
		}

		for (int i = 0; i < featureBones.size(); i++)
		{
			sum += Distance(f1.featureBonePos[i], f2.featureBonePos[i]) / 3.0f;
			sum += Distance(f1.featureBoneVels[i], f2.featureBoneVels[i]) / 3.0f;
		}


		return sum;
	}


}

