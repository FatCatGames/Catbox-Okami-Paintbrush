#pragma once

class SphereCollider;
class BoxCollider;
class CircleCollider2D;
class BoxCollider2D;
class Ray;
class Plane;
class Collider;

namespace Catbox
{
	struct Sphere;
}

class Intersection
{
public:

	struct OverlapData
	{
		float overlapTop;
		float overlapBottom;
		float overlapRight;
		float overlapLeft;
	};

	static bool RayIntersect(Collider& aCol, const Ray& aRay, const float& aMaxLength, Vector3f& aOutIntersectionPoint);
	static bool SphereIntersect(Collider& aCol, Catbox::Sphere& aSphere, Vector3f& anOutIntersectionPoint);
	static bool IsCollidedWith(Collider& aCol1, Collider& aCol2, Vector3f& anOutIntersectionPoint);
	static Vector2f GetOverlap(Collider& aCol1, Collider& aCol2);

private:
	//collisions
	static bool IntersectionSphereSphere(const Catbox::Sphere& aFirstSphere, const Catbox::Sphere& aSecondSphere, Vector3f& aOutIntersectionPoint);
	static bool IntersectionAABBAABB(const BoxCollider& aFirstAABB, const BoxCollider& aSecondAABB);
	static bool IntersectionAABBSphere(const BoxCollider& aAABB, const Catbox::Sphere& aSphere, Vector3f& aOutIntersectionPoint);
	static bool IntersectionCircle2DBox2D(const CircleCollider2D& aCircle, const BoxCollider2D& aBox);
	static bool IntersectionCircle2DCircle2D(const CircleCollider2D& aFirstCircle, const CircleCollider2D& aSecondCircle);
	static bool IntersectionBox2DBox2D(const BoxCollider2D& aFirstBox, const BoxCollider2D& aSecondBox);

	//rays
	static bool IntersectionPlaneRay(const Plane& aPlane, const Ray& aRay, const float& aMaxLength, Vector3f& aOutIntersectionPoint);
	static bool IntersectionAABBRay(const BoxCollider& aAABB, const Ray& aRay, const float& aMaxLength, Vector3f& aOutIntersectionPoint);
	static bool IntersectionBox2DRay(const BoxCollider2D& aBox, const Ray& aRay, const float& aMaxLength, Vector3f& aOutIntersectionPoint);
	static bool IntersectionCircle2DRay(const CircleCollider2D& aCircle, const Ray& aRay, const float& aMaxLength, Vector3f& aOutIntersectionPoint);
	static bool IntersectionSphereRay(const Catbox::Sphere& aSphere, const Ray& aRay, const float& aMaxLength);

	//overlap
	static Vector2f GetOverlapBox2DBox2D(const BoxCollider2D& aFirstBox, const BoxCollider2D& aSecondBox);
	static Vector2f GetOverlapCircle2DBox2D(const CircleCollider2D& aCircle, const BoxCollider2D& aBox);
	static Vector2f GetOverlapBox2DCircle2D(const BoxCollider2D& aBox, const CircleCollider2D& aCircle);
	static Vector2f GetOverlapCircle2DCircle2D(const CircleCollider2D& aFirstCircle, const CircleCollider2D& aSecondCircle);
};