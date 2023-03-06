#include "stdafx.h"
#include "SkeletonData.h"
#include <stack>



//void SkeletonInstance::UpdateAnimationHierarchy(Animation::Frame& aFrame, Animation::Frame& aNextFrame, std::vector<bool>& aBoneMask, float aFramePercentage)
//{
//	std::stack<unsigned int> bonesToVisit;
//	bonesToVisit.push(0);
//	Catbox::Matrix4x4<float> parentTransform;
//	float t = aFramePercentage;
//	while (!bonesToVisit.empty())
//	{
//		unsigned int currentBoneID = bonesToVisit.top();
//		bonesToVisit.pop();
//
//
//		if (aBoneMask[currentBoneID])
//		{
//			Catbox::Matrix4x4<float> interpolatedTransform;
//
//			const std::string aBone = sharedData->bones[currentBoneID].name;
//			Catbox::Vector3<float>& firstFramePos = aFrame.localPositions[aBone];
//			Catbox::Vector3<float>& firstFrameScale = aFrame.localScales[aBone];
//			Catbox::Vector4<float>& firstFrameRotation = aFrame.localRotations[aBone];
//
//			Catbox::Vector3<float>& nextFramePos = aNextFrame.localPositions[aBone];
//			Catbox::Vector3<float>& nextFrameScale = aNextFrame.localScales[aBone];
//			Catbox::Vector4<float>& nextFrameRotation = aNextFrame.localRotations[aBone];
//
//			Catbox::Vector3<float> interpolatedPos = Catbox::Lerp(firstFramePos, nextFramePos, t);
//			Catbox::Vector3<float> interpolatedScale = Catbox::Lerp(firstFrameScale, nextFrameScale, t);
//			Catbox::Vector4<float> interpolatedRotation = QuatFunctions::Slerp(firstFrameRotation, nextFrameRotation, t);
//
//			Catbox::Matrix4x4<float> rotationMatrix = QuatFunctions::GetRotationMatrix4x4(interpolatedRotation);
//
//			interpolatedTransform(1, 1) = interpolatedScale.x;
//			interpolatedTransform(2, 2) = interpolatedScale.y;
//			interpolatedTransform(3, 3) = interpolatedScale.z;
//			interpolatedTransform = interpolatedTransform * rotationMatrix;
//			interpolatedTransform(4, 1) = interpolatedPos.x;
//			interpolatedTransform(4, 2) = interpolatedPos.y;
//			interpolatedTransform(4, 3) = interpolatedPos.z;
//			interpolatedTransform = Catbox::Matrix4x4<float>::ToColumnMajor(interpolatedTransform);
//
//			if (currentBoneID <= aFrame.localTransforms.size())
//			{
//				boneMatrices[currentBoneID] = parentTransform * interpolatedTransform;
//			}
//
//			boneMatricesNoBindpose[currentBoneID] = boneMatrices[currentBoneID];
//			boneMatrices[currentBoneID] *= sharedData->bones[currentBoneID].bindPoseInverse;
//		}
//		else
//		{
//			boneMatrices[currentBoneID] = parentTransform;
//			boneMatricesNoBindpose[currentBoneID] = boneMatrices[currentBoneID];
//			boneMatrices[currentBoneID] *= sharedData->bones[currentBoneID].bindPoseInverse;
//		}
//		for (unsigned int bone : sharedData->bones[currentBoneID].childBoneIndices)
//		{
//			bonesToVisit.push(bone);
//		}
//
//		int parentBoneIndex = sharedData->bones[currentBoneID].parentBoneIndex;
//		if (parentBoneIndex != -1)
//		{
//			parentTransform = boneMatricesNoBindpose[parentBoneIndex];
//		}
//	}
//}