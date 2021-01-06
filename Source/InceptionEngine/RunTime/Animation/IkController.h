
#pragma once

namespace inceptionengine
{
	class AnimationController;


	struct AimIkChain
	{
		std::vector<std::string> boneNames;
		std::vector<int> boneID;
		std::vector<float> weights;
	};

	class IkController
	{
	public:
		IkController(std::reference_wrapper<AnimationController> controller);
		~IkController();

		void SetAimIkChain(std::vector<std::string> const& boneNames, std::vector<int> const& boneIDs, std::vector<float> const& weights);

		//retrun the solved local pose of aim ik chain
		std::vector<Matrix4x4f> ChainAimTo(Matrix4x4f modelTransform, Vec3f const& targetPosition, Vec3f const& eyeOffsetInHeadCoord) const;

		std::vector<int> const& GetAimIkChainBoneIDs() const;

		std::vector<Matrix4x4f> GetAimIkChainCurrentLclTransform() const;

		void ActivateAimIk();

		void DeactivateAimIk();

		bool IsAimIkActive() const;

	private:
		//return the bone transformation after solving aim ik
		Matrix4x4f BoneAimTo(Matrix4x4f modelTransform, std::vector<Matrix4x4f>& currentPose, int boneID, Vec3f const& targetPosition, Vec3f const& offset, float weight) const;

	private:
		std::reference_wrapper<AnimationController> mAnimationController;

		AimIkChain mAimIkChain;

		bool mAimIkActive = false;
	};
}