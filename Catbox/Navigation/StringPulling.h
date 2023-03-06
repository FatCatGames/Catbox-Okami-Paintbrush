#pragma once
#include "Vector.h"
#include <vector>
#include "NavMesh.h"

inline float triarea2(const Vector3f& aVertexA, const Vector3f& aVertexB, const Vector3f& aVertexC)
{
	float ax = aVertexB.x - aVertexA.x;
	float ay = aVertexB.z - aVertexA.z;
	float bx = aVertexC.x - aVertexA.x;
	float by = aVertexC.z - aVertexA.z;
	float area = bx * ay - ax * by;
	return area;
}

inline bool vequal(const Vector3f& aVertexA, const Vector3f& aVertexB)
{
	static const float eq = 0.001f * 0.001f;
	float lengthSqr = (aVertexA - aVertexB).LengthSqr();
	return lengthSqr < eq;
}

inline void stringPullDebug(const std::vector<NavMesh::Connection*>& aPath, std::vector<Vector3f>& waypoints, int& i, bool drawRight, std::vector<Vector3f>& leftPoints, std::vector<Vector3f>& rightPoints, Vector3f& portalApex, Vector3f& portalLeft, Vector3f& portalRight)
{
	if (i >= aPath.size()) return;
	const Vector3f left = aPath[i]->left;
	const Vector3f right = aPath[i]->right;


	// Update right vertex.
	if (triarea2(portalApex, portalRight, right) <= 0.0f)
	{
		if (vequal(portalApex, portalRight) || triarea2(portalApex, portalLeft, right) > 0.0f || vequal(portalRight, right))
		{
			// Tighten the funnel.
			portalRight = aPath[i]->right;
			rightPoints.push_back(portalRight);
		}
		else
		{
			waypoints.push_back(portalLeft);
			rightPoints.push_back(portalRight);
			portalRight = portalApex = portalLeft;
		}
	}

	// Update left vertex.
	if (triarea2(portalApex, portalLeft, left) >= 0.0f)
	{
		if (vequal(portalApex, portalLeft) || triarea2(portalApex, portalRight, left) < 0.0f || vequal(portalLeft, left))
		{
			// Tighten the funnel.
			portalLeft = aPath[i]->left;
			leftPoints.push_back(portalLeft);
		}
		else
		{
			waypoints.push_back(portalRight);
			leftPoints.push_back(portalLeft);
			portalLeft = portalApex = portalRight;
		}
	}
	++i;

}



inline std::vector<Vector3f> stringPull(const std::vector<NavMesh::Connection*>& aPath, bool leftOrRight, Vector3f aPos)
{
	Vector3f portalLeft = aPath[0]->left;
	Vector3f portalRight = aPath[0]->right;
	
	Vector3f portalApex = aPath[0]->left;


	std::vector<Vector3f> points;
	
	//points.push_back(aPos);
	Vector3f lastLeft;
	Vector3f lastRight;

	Vector3f lastPos;

	//points.push_back(portalApex);

	int apexIndex = 0, leftIndex = 0, rightIndex = 0;

	for (int i = 1; i < aPath.size(); ++i)
	{
		Vector3f left = aPath[i]->left;
		Vector3f right = aPath[i]->right;

		float area = triarea2(portalApex, portalRight, right);
		if (area <= 0.0f)
		{
			area = triarea2(portalApex, portalLeft, right);
			if (vequal(portalApex, portalRight) || area >= 0.0f)
			{
				// Tighten the funnel.
				portalRight = aPath[i]->right;
				rightIndex = i;
 			}
			else
			{
				// Right over left, insert left to path and restart scan from portal left point.
				points.push_back(portalLeft);
				portalApex = portalLeft;
				apexIndex = leftIndex;

				portalLeft = portalApex;
				portalRight = portalApex;

				leftIndex = apexIndex;
				rightIndex = apexIndex;

				i = apexIndex;
				continue;
			}
		}
		area = triarea2(portalApex, portalLeft, left);
		if (area >= 0.0f)
		{
			area = triarea2(portalApex, portalRight, left);
			if (vequal(portalApex, portalLeft) || area <= 0.0f)
			{
				portalLeft = aPath[i]->left;
				leftIndex = i;
			}
			else
			{
				// Left over right, insert right to path and restart scan from portal right point.
				points.push_back(portalRight);

				portalApex = portalRight;
				apexIndex = rightIndex;

				portalLeft = portalApex;
				portalRight = portalApex;

				leftIndex = apexIndex;
				rightIndex = apexIndex;
				i = apexIndex;
				continue;
			}
		}
	}
	//if ((points.size() == 0))
	//{
	//	points.push_back(aPath[aPath.size() - 1]->left);
	//}

	return points;
}
