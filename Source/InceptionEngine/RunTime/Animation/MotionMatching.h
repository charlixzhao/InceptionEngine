#pragma once

#include "Animation.h"
#include "RunTime/SkeletalMesh/Skeleton.h"
#include "AnimBlender.h"
#include "RunTime/Resource/ResourceManager.h"
#include <iostream>

namespace inceptionengine
{
	namespace motionmatching
	{

		inline std::vector<Matrix4x4f> GetBoneGlobalTransforms(std::vector<Matrix4x4f> const& localFinalPose, std::shared_ptr<Skeleton const> skeleton)
		{
			std::vector<Matrix4x4f> globalFinalPose;
			globalFinalPose.resize(localFinalPose.size());
			for (auto const& bone : skeleton->mBones)
			{
				Matrix4x4f globalTransform = localFinalPose[bone.ID];
				int parentID = bone.parentID;
				while (parentID != -1)
				{
					globalTransform = localFinalPose[parentID] * globalTransform;
					parentID = skeleton->mBones[parentID].parentID;
				}
				globalFinalPose[bone.ID] = globalTransform;
			}

			return globalFinalPose;
		}


		struct Feature
		{
			std::array<Vec3f, 3> trajectory = {};  //10 frame, 20 frame, 30 frame
			std::array<Vec3f, 3> facingDirection = {};
			Vec3f leftFootPosition = { 0.0f, 0.0f,0.0f };
			Vec3f rightFootPosition = { 0.0f, 0.0f,0.0f };
			Vec3f leftFootVelocity = { 0.0f, 0.0f,0.0f };
			Vec3f rightFootVelocity = { 0.0f, 0.0f,0.0f };
			Vec3f hipVelocity = { 0.0f, 0.0f,0.0f };

			static float FeatureDistance(Feature const& f1, Feature const& f2)
			{
				return Distance(f1.trajectory[0], f2.trajectory[0])
					+ Distance(f1.trajectory[1], f2.trajectory[1])
					+ Distance(f1.trajectory[2], f2.trajectory[2])
					+ Distance(f1.facingDirection[0], f2.facingDirection[0])
					+ Distance(f1.facingDirection[1], f2.facingDirection[1])
					+ Distance(f1.facingDirection[2], f2.facingDirection[2])
					+ Distance(f1.leftFootPosition, f2.leftFootPosition)
					+ Distance(f1.rightFootPosition, f2.rightFootPosition);

				/*NEED VEL: in computing cost*/
					//+ Distance(f1.leftFootVelocity, f2.leftFootVelocity)
					//+ Distance(f1.rightFootVelocity, f2.rightFootVelocity)
					//+ Distance(f1.hipVelocity, f2.hipVelocity);
			}
		};

		struct Pose
		{
			std::vector<Vec3f> lclTranslations;
			std::vector<Quaternion4f> lclRotations;
			std::vector<Vec3f> lclTranslationVel;
			std::vector<Vec4f> lclRotationVel;
			Vec3f lclRootTranslationVel; // local to character facing dirction
			Vec4f lclRootRotationVel; // local to character facing direction
		};

		struct MatchingDB
		{
			std::vector<Feature> features;

			int Query(Feature const& f1)
			{
				float minCost = std::numeric_limits<float>().max();
				int argmin = -1;
				for (int i = 0; auto const& f : features)
				{
					float cost = Feature::FeatureDistance(f, f1);
					if (cost < minCost)
					{
						minCost = cost;
						argmin = i;
					}
					i++;
				}

				printf("argmin is %d with cost %f\n", argmin, minCost);

				return argmin;
			}

			static MatchingDB ConstructFromAnim(Animation const& anim)
			{
				
				MatchingDB db;

				int constexpr FacingAxis = 1;
				std::vector<std::vector<Matrix4x4f>> globalTransforms;
				globalTransforms.reserve(anim.mBoneTransforms.size());
				for (auto const& frame : anim.mBoneTransforms)
				{
					globalTransforms.push_back(GetBoneGlobalTransforms(frame, anim.mSkeleton));
				}

				for (int i = 0; i < anim.mBoneTransforms.size() - 30; i++)
				{
					Feature f{};

					std::vector<Matrix4x4f> const& currentPose = globalTransforms[i]; //GetBoneGlobalTransforms(anim.mBoneTransforms[i], anim.mSkeleton);
					std::vector<Matrix4x4f> const& next10Pose = globalTransforms[i + 10];//GetBoneGlobalTransforms(anim.mBoneTransforms[i + 10], anim.mSkeleton);
					std::vector<Matrix4x4f> const& next20Pose = globalTransforms[i + 20];//GetBoneGlobalTransforms(anim.mBoneTransforms[i + 20], anim.mSkeleton);
					std::vector<Matrix4x4f> const& next30Pose = globalTransforms[i + 30];//GetBoneGlobalTransforms(anim.mBoneTransforms[i + 30], anim.mSkeleton);
					//std::vector<Matrix4x4f> const& prev10Pose = globalTransforms[i - 10];//GetBoneGlobalTransforms(anim.mBoneTransforms[i - 10], anim.mSkeleton);

					//Vec3f currentPosition = ProjectToXZ(currentPose[0][3]); 
					//f.trajectory[0] = ProjectToXZ(next10Pose[0][3]) - currentPosition;
					//f.trajectory[1] = ProjectToXZ(next20Pose[0][3]) - currentPosition;
					//f.trajectory[2] = ProjectToXZ(next30Pose[0][3]) - currentPosition;

					Vec3f currentPosition = currentPose[0][3];
					f.trajectory[0] = Vec3f(next10Pose[0][3]) - currentPosition;
					f.trajectory[1] = Vec3f(next20Pose[0][3]) - currentPosition;
					f.trajectory[2] = Vec3f(next30Pose[0][3]) - currentPosition;

					f.facingDirection[0] = next10Pose[0][FacingAxis];
					f.facingDirection[1] = next20Pose[0][FacingAxis];
					f.facingDirection[2] = next30Pose[0][FacingAxis];

					std::string leftFootName = "Model:LeftFoot";
					std::string rightFootName = "Model:RightFoot";
					std::string hipName = "Model:Hips";
					float timeInterval = 10.0f / 30.0f;
					f.leftFootPosition = Vec3f(currentPose[anim.mSkeleton->GetBoneID(leftFootName)][3]) - currentPosition;
					f.rightFootPosition = Vec3f(currentPose[anim.mSkeleton->GetBoneID(rightFootName)][3]) - currentPosition;
					
					/*NEED VEL: during compute in building matching db*/
					//f.leftFootVelocity = (next10Pose[anim.mSkeleton->GetBoneID(leftFootName)][3] - prev10Pose[anim.mSkeleton->GetBoneID(leftFootName)][3]) / (2 * timeInterval);
					//f.rightFootVelocity = (next10Pose[anim.mSkeleton->GetBoneID(rightFootName)][3] - prev10Pose[anim.mSkeleton->GetBoneID(rightFootName)][3]) / (2 * timeInterval);
					//f.hipVelocity = (next10Pose[anim.mSkeleton->GetBoneID(hipName)][3] - prev10Pose[anim.mSkeleton->GetBoneID(hipName)][3]) / (2 * timeInterval);
					
					db.features.push_back(f);
				}

				return db;
			}
		};
		
		struct AnimDB
		{
			std::vector<Pose> poses;
		};

		


		class MotionMatchingController
		{
		public:

			std::vector<Matrix4x4f> GetCurrentPose() const { return mCurrentPose; }

			void LoadMatchingDB(std::string const& animFile)
			{
				auto anim = gResourceMgr.GetResource<Animation>(animFile);
				mAnimDB = anim;
				std::cout << "0 frame hip transformation:\n";
				auto mat = anim->mBoneTransforms[137][0];
				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						std::cout << mat[j][i] << " ";
					}
					std::cout << std::endl;
				}



				mMatchingDB = std::make_shared<MatchingDB>(MatchingDB::ConstructFromAnim(*anim));


				int frame = 530;
				std::cout << frame << " frame trajory\n";
				std::cout << VecToString(mMatchingDB->features[frame-10].trajectory[0]) << std::endl;
				std::cout << VecToString(mMatchingDB->features[frame-10].trajectory[1]) << std::endl;
				std::cout << VecToString(mMatchingDB->features[frame-10].trajectory[2]) << std::endl;

				std::cout << frame << " frame fracing\n";
				std::cout << VecToString(mMatchingDB->features[frame-10].facingDirection[0]) << std::endl;
				std::cout << VecToString(mMatchingDB->features[frame-10].facingDirection[1]) << std::endl;
				std::cout << VecToString(mMatchingDB->features[frame-10].facingDirection[2]) << std::endl;
				
				int a = 0;

				a++;
		
				
			}

			//continue to play on the current anim
			void Update(float dt)
			{
				//check if transition is occuring
				if (mMotionBlender.IsBlending())
				{
					auto blendResult = mMotionBlender.Blend(dt);
					if (blendResult.has_value())
					{
						mCurrentPose = blendResult.value();
					}
					return;
				}
				else
				{
					mRunningTime += dt;
					if (mRunningTime >= mAnimDB->GetDuration()) mRunningTime = 0.0f;
					mCurrentPose = mAnimDB->Interpolate(mRunningTime);
				}
			}

			//new feature arrive, check if transition need to be insert
			void Update(float dt, Feature const& f) 
			{ 

				int index = mMatchingDB->Query(f);

				float const timePerFrame = (1.0f / 30.0f);
				float indexTime = index * timePerFrame;

				if (std::abs(mRunningTime - indexTime) <= 10.0f * timePerFrame) //check whether need transition
				{
					//no need to insert transition, continue to play
					Update(dt);
					return;
				}
				else
				{
					//insert a transition
					mMotionBlender.StartBlending(mCurrentPose, mAnimDB->mBoneTransforms[index], 3.0f * timePerFrame);
					mRunningTime = indexTime;
				}
				
			}

		private:
			std::shared_ptr<MatchingDB> mMatchingDB = nullptr;
			std::shared_ptr<Animation> mAnimDB = nullptr;
			std::vector<Matrix4x4f> mCurrentPose;
			int mCurrentIndex = -1;
			float mRunningTime = 530.0f / 30.0f;
			AnimBlender mMotionBlender;
			
		};



	}
}
