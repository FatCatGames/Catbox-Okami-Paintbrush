#pragma once
#include <unordered_map>
#include "Animation.h"

namespace QuatFunctions
{
	//Zoe code, ask Zoe for Info
	inline Catbox::Vector4<float> FromRotationMatrix4x4(const Catbox::Matrix4x4<float>& aMatrix)
	{
		float m00 = aMatrix(1, 1);
		float m01 = aMatrix(2, 1);
		float m02 = aMatrix(3, 1);

		float m10 = aMatrix(1, 2);
		float m11 = aMatrix(2, 2);
		float m12 = aMatrix(3, 2);

		float m20 = aMatrix(1, 3);
		float m21 = aMatrix(2, 3);
		float m22 = aMatrix(3, 3);

		Catbox::Vector4<float> quatFromMatrix;

		float t = static_cast<float>(0);
		if (m22 < static_cast<float>(0))
		{
			if (m00 > m11)
			{
				t = static_cast<float>(1) + m00 - m11 - m22;

				quatFromMatrix.x = t;
				quatFromMatrix.y = m01 + m10;
				quatFromMatrix.z = m20 + m02;
				quatFromMatrix.w = m12 - m21;
			}
			else
			{
				t = static_cast<float>(1) - m00 + m11 - m22;
				quatFromMatrix.x = m01 + m10;
				quatFromMatrix.y = t;
				quatFromMatrix.z = m12 + m21;
				quatFromMatrix.w = m20 - m02;
			}
		}
		else
		{
			if (m00 < -m11)
			{
				t = static_cast<float>(1) - m00 - m11 + m22;
				quatFromMatrix.x = m20 + m02;
				quatFromMatrix.y = m12 + m21;
				quatFromMatrix.z = t;
				quatFromMatrix.w = m01 - m10;
			}
			else
			{
				t = static_cast<float>(1) + m00 + m11 + m22;
				quatFromMatrix.x = m12 - m21;
				quatFromMatrix.y = m20 - m02;
				quatFromMatrix.z = m01 - m10;
				quatFromMatrix.w = t;
			}
		}

		quatFromMatrix *= static_cast<float>(0.5) / std::sqrt(t);
		quatFromMatrix.w *= static_cast<float>(-1);
		quatFromMatrix.Normalize();


		return Catbox::Vector4<float>(quatFromMatrix.x, quatFromMatrix.y, quatFromMatrix.z, quatFromMatrix.w);
	}

	inline Catbox::Vector4<float> Slerp(const Catbox::Vector4<float>& aQuaternionA, const Catbox::Vector4<float>& aQuaternionB, const float& aT)
	{
		Catbox::Vector4<float> quatB = aQuaternionB;

		float dotAB = aQuaternionA.Dot(quatB);

		if (dotAB < static_cast<float>(0))
		{
			quatB *= -1.0f;
			dotAB *= static_cast<float>(-1);
		}

		float dotABSqr = Catbox::Min(dotAB * dotAB, static_cast<float>(1));

		float sinTheta = std::sqrt(static_cast<float>(1) - dotABSqr);

		if (sinTheta < static_cast<float>(0.0001))
		{
			return aQuaternionA;
		}

		return ((sinTheta * (static_cast<float>(1) - aT)) / sinTheta) * aQuaternionA + ((sinTheta * aT) / sinTheta) * quatB;
	}

	inline Catbox::Matrix4x4<float> GetRotationMatrix4x4(Catbox::Vector4<float> aQuaternion)
	{
		Catbox::Matrix4x4<float> rotationMatrix;
		float x = aQuaternion.x;
		float y = aQuaternion.y;
		float z = aQuaternion.z;
		float w = aQuaternion.w;

		rotationMatrix(1, 1) = static_cast<float>(1) - static_cast<float>(2) * (y * y + z * z);
		rotationMatrix(1, 2) = static_cast<float>(2) * (x * y + w * z);
		rotationMatrix(1, 3) = static_cast<float>(2) * (x * z - w * y);

		rotationMatrix(2, 1) = static_cast<float>(2) * (x * y - w * z);
		rotationMatrix(2, 2) = static_cast<float>(1) - static_cast<float>(2) * (x * x + z * z);
		rotationMatrix(2, 3) = static_cast<float>(2) * (y * z + w * x);

		rotationMatrix(3, 1) = static_cast<float>(2) * (x * z + w * y);
		rotationMatrix(3, 2) = static_cast<float>(2) * (y * z - w * x);
		rotationMatrix(3, 3) = static_cast<float>(1) - static_cast<float>(2) * (x * x + y * y);

		return rotationMatrix;
	}
}

struct Skeleton : public Asset
{
	struct Bone
	{
		std::string name;
		Catbox::Matrix4x4<float> bindPoseInverse;
		int id;
		int parentBoneIndex;
		std::vector<unsigned int> childBoneIndices;

		Bone(std::string aName, int anId, int aParentIndex, const Catbox::Matrix4x4<float>& aBindPoseInverse, std::vector<unsigned int> someChildren)
		{
			name = aName;
			id = anId;
			parentBoneIndex = aParentIndex;
			bindPoseInverse = aBindPoseInverse;
			childBoneIndices = someChildren;
		}
	};
	void SaveAsset(const char* aPath) {}
	std::vector<Bone> bones;
	std::unordered_map<std::string, size_t> boneNameToIndex;
	std::vector<std::string> boneNames;
	std::unordered_map<std::string, std::shared_ptr<Animation>> animations;
	std::vector<std::shared_ptr<Animation>> allAnimations;
	__forceinline const Bone* GetRoot() const { return !bones.empty() ? &bones[0] : nullptr; }
};

struct SkeletonInstance
{
	std::shared_ptr<Skeleton> sharedData;
	std::array<Catbox::Matrix4x4<float>, 128> boneMatrices;
	std::vector<Transform> boneTransforms;

	//void UpdateAnimationHierarchy(Animation::Frame& aFrame, Animation::Frame& aNextFrame, std::vector<bool>& aBoneMask, float aFramePercentage);


	void SetSharedData(std::shared_ptr<Skeleton> aSkeleton)
	{
		boneTransforms.clear();
		for (size_t i = 0; i < boneMatrices.size(); i++)
		{
			boneMatrices[i].Reset();
		}
		boneTransforms.resize(aSkeleton->bones.size());

		sharedData = aSkeleton;
	}

	void Reset()
	{
		for (size_t i = 0; i < boneMatrices.size(); i++)
		{
			boneMatrices[i].Reset();
		}
		for (size_t i = 0; i < boneTransforms.size(); i++)
		{
			boneTransforms[i].Reset();
		}
	}


	void UpdateAnimationHierarchyNoBlend(int aBoneID, Animation::Frame& aFrame, const Catbox::Matrix4x4<float>& aParentTransform, std::array<bool, 128>& aBoneList)
	{
		//Is bone masked? Proceed with calculation, otherwise don't override the animation result from the previous layer.
		if (aBoneList[aBoneID])
		{
			Catbox::Matrix4x4<float> transform;

			const std::string aBone = sharedData->bones[aBoneID].name;
			Catbox::Vector3<float>& pos = aFrame.localPositions[aBone];
			Catbox::Vector3<float>& scale = aFrame.localScales[aBone];
			Catbox::Vector4<float>& rRotation = aFrame.localRotations[aBone];

			Catbox::Matrix4x4<float> rotationMatrix = QuatFunctions::GetRotationMatrix4x4(rRotation);

			transform(1, 1) = scale.x;
			transform(2, 2) = scale.y;
			transform(3, 3) = scale.z;
			transform = transform * rotationMatrix;
			transform(4, 1) = pos.x;
			transform(4, 2) = pos.y;
			transform(4, 3) = pos.z;
			transform = Catbox::Matrix4x4<float>::ToColumnMajor(transform);

			//Apply frame result to bone
			if (aBoneID <= aFrame.localTransforms.size())
			{
				boneMatrices[aBoneID] = aParentTransform * transform;
			}
		}

		//Update child bones
		for (const auto bone : sharedData->bones[aBoneID].childBoneIndices)
		{
			UpdateAnimationHierarchyNoBlend(bone, aFrame, boneMatrices[aBoneID], aBoneList);
		}
	}

	void UpdateAnimationHierarchy(int aBoneID, Animation::Frame& aFrame, Animation::Frame& aNextFrame, const Catbox::Matrix4x4<float>& aParentTransform, float t, std::array<bool, 128>& aBoneList)
	{
		//Is bone masked? Proceed with calculation, otherwise don't override the animation result from the previous layer.
		if (aBoneList[aBoneID])
		{
				//Interpolate between current frame and next frame for smoother animations
#pragma region Blending

				Catbox::Matrix4x4<float> interpolatedTransform;

				const std::string aBone = sharedData->bones[aBoneID].name;
				Catbox::Vector3<float>& firstFramePos = aFrame.localPositions[aBone];
				Catbox::Vector3<float>& firstFrameScale = aFrame.localScales[aBone];
				Catbox::Vector4<float>& firstFrameRotation = aFrame.localRotations[aBone];

				Catbox::Vector3<float>& nextFramePos = aNextFrame.localPositions[aBone];
				Catbox::Vector3<float>& nextFrameScale = aNextFrame.localScales[aBone];
				Catbox::Vector4<float>& nextFrameRotation = aNextFrame.localRotations[aBone];

				Catbox::Vector3<float> interpolatedPos = Catbox::Lerp(firstFramePos, nextFramePos, t);
				Catbox::Vector3<float> interpolatedScale = Catbox::Lerp(firstFrameScale, nextFrameScale, t);
				Catbox::Vector4<float> interpolatedRotation = QuatFunctions::Slerp(firstFrameRotation, nextFrameRotation, t);

				Catbox::Matrix4x4<float> rotationMatrix = QuatFunctions::GetRotationMatrix4x4(interpolatedRotation);

				interpolatedTransform(1, 1) = interpolatedScale.x;
				interpolatedTransform(2, 2) = interpolatedScale.y;
				interpolatedTransform(3, 3) = interpolatedScale.z;
				interpolatedTransform = interpolatedTransform * rotationMatrix;
				interpolatedTransform(4, 1) = interpolatedPos.x;
				interpolatedTransform(4, 2) = interpolatedPos.y;
				interpolatedTransform(4, 3) = interpolatedPos.z;
				interpolatedTransform = Catbox::Matrix4x4<float>::ToColumnMajor(interpolatedTransform);

#pragma endregion

				//Apply frame result to bone
				if (aBoneID <= aFrame.localTransforms.size())
				{
					boneMatrices[aBoneID] = aParentTransform * interpolatedTransform;
				}
			}

		//Update child bones
		for (const auto bone : sharedData->bones[aBoneID].childBoneIndices)
		{
			UpdateAnimationHierarchy(bone, aFrame, aNextFrame, boneMatrices[aBoneID], t, aBoneList);
		}

	}
};