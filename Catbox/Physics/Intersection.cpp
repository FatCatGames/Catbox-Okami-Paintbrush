#include "stdafx.h"
#include "Intersection.h"
#include "Plane.h"
#include "Ray.h"
#include "../Components/Physics/Collisions/Collider.h"
#include "../Components/Physics/Collisions/SphereCollider.h"
#include "../Components/Physics/Collisions/BoxCollider.h"
#include "../Components/Physics/Collisions/BoxCollider2D.h"
#include "../Components/Physics/Collisions/CircleCollider2D.h"
#include "../CommonUtilities/UtilityFunctions.hpp"
#include "../CommonUtilities/Sphere.hpp"

#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define max(a,b)            (((a) > (b)) ? (a) : (b))

bool Intersection::RayIntersect(Collider& aCol, const Ray& aRay, const float& aMaxLength, Vector3f& aOutIntersectionPoint)
{
	switch (aCol.GetColliderType())
	{
	case Collider::ColliderType::Box2D:
	{
		const BoxCollider2D& box = static_cast<BoxCollider2D&>(aCol);
		return IntersectionBox2DRay(box, aRay, aMaxLength, aOutIntersectionPoint);
	}
	case Collider::ColliderType::Circle2D:
	{
		const CircleCollider2D& circle = static_cast<CircleCollider2D&>(aCol);
		return IntersectionCircle2DRay(circle, aRay, aMaxLength, aOutIntersectionPoint);
	}
	case Collider::ColliderType::Sphere:
	{
		const SphereCollider& sphereCol = static_cast<SphereCollider&>(aCol);
		Catbox::Sphere sphere(sphereCol.GetPosition(), sphereCol.GetRadius());
		return IntersectionSphereRay(sphere, aRay, aMaxLength);
	}
	case Collider::ColliderType::Box:
	{
		const BoxCollider& box = static_cast<BoxCollider&>(aCol);
		return IntersectionAABBRay(box, aRay, aMaxLength, aOutIntersectionPoint);
	}
	}
	return false;
}

bool Intersection::SphereIntersect(Collider& aCol, Catbox::Sphere& aSphere, Vector3f& anOutIntersectionPoint)
{
	switch (aCol.GetColliderType())
	{
	case Collider::ColliderType::Sphere:
	{
		const SphereCollider& sphereCol = static_cast<SphereCollider&>(aCol);
		Catbox::Sphere sphere(sphereCol.GetPosition(), sphereCol.GetRadius());
		return IntersectionSphereSphere(sphere, aSphere, anOutIntersectionPoint);
	}
	case Collider::ColliderType::Box:
	{
		const BoxCollider& box = static_cast<BoxCollider&>(aCol);
		return IntersectionAABBSphere(box, aSphere, anOutIntersectionPoint);
	}
	}
	return false;
}

bool Intersection::IsCollidedWith(Collider& aCol1, Collider& aCol2, Vector3f& anOutIntersectionPoint)
{
	//Out intersection point not implemented with all collision types!

	switch (aCol1.GetColliderType())
	{
	case Collider::ColliderType::Box:
	{
		const BoxCollider& box = static_cast<BoxCollider&>(aCol1);
		switch (aCol2.GetColliderType())
		{
		case Collider::ColliderType::Box: return IntersectionAABBAABB(box, static_cast<BoxCollider&>(aCol2));
		case Collider::ColliderType::Sphere:
		{
			auto sphereCol = static_cast<SphereCollider&>(aCol2);
			Catbox::Sphere sphere(sphereCol.GetPosition(), sphereCol.GetRadius());
			return IntersectionAABBSphere(box, sphere, anOutIntersectionPoint);

		}
		default: return false;
		}
	}
	case Collider::ColliderType::Sphere:
	{
		const SphereCollider& sphereCol = static_cast<SphereCollider&>(aCol1);
		Catbox::Sphere sphere(sphereCol.GetPosition(), sphereCol.GetRadius());
		switch (aCol2.GetColliderType())
		{
		case Collider::ColliderType::Sphere:
		{
			auto sphereCol2 = static_cast<SphereCollider&>(aCol2);
			Catbox::Sphere sphere2(sphereCol2.GetPosition(), sphereCol2.GetRadius());
			return IntersectionSphereSphere(sphere, sphere2, anOutIntersectionPoint);

		}
		case Collider::ColliderType::Box: return IntersectionAABBSphere(static_cast<BoxCollider&>(aCol2), sphere, anOutIntersectionPoint);
		default: return false;
		}
	}
	case Collider::ColliderType::Box2D:
	{
		const BoxCollider2D& box = static_cast<BoxCollider2D&>(aCol1);
		switch (aCol2.GetColliderType())
		{
		case Collider::ColliderType::Box2D: return IntersectionBox2DBox2D(box, static_cast<BoxCollider2D&>(aCol2));
		case Collider::ColliderType::Circle2D: return IntersectionCircle2DBox2D(static_cast<CircleCollider2D&>(aCol2), box);
		default: return false;
		}
	}
	case Collider::ColliderType::Circle2D:
	{
		const CircleCollider2D& circle = static_cast<CircleCollider2D&>(aCol1);
		switch (aCol2.GetColliderType())
		{
		case Collider::ColliderType::Box2D: return IntersectionCircle2DBox2D(circle, static_cast<BoxCollider2D&>(aCol2));
		case Collider::ColliderType::Circle2D: return IntersectionCircle2DCircle2D(circle, static_cast<CircleCollider2D&>(aCol2));
		default: return false;
		}
	}
	default:
		return false;
	}
}

Vector2f Intersection::GetOverlap(Collider& aCol1, Collider& aCol2)
{
	switch (aCol1.GetColliderType())
	{
	case Collider::ColliderType::Box2D:
	{
		const BoxCollider2D& box = static_cast<BoxCollider2D&>(aCol1);
		switch (aCol2.GetColliderType())
		{
		case Collider::ColliderType::Box2D: return GetOverlapBox2DBox2D(box, static_cast<BoxCollider2D&>(aCol2));
		case Collider::ColliderType::Circle2D:
		{
			return GetOverlapBox2DCircle2D(box, static_cast<CircleCollider2D&>(aCol2));
		}
		default: return { 0,0 };
		}
	}
	case Collider::ColliderType::Circle2D:
	{
		const CircleCollider2D& circle = static_cast<CircleCollider2D&>(aCol1);
		switch (aCol2.GetColliderType())
		{
		case Collider::ColliderType::Box2D: return GetOverlapCircle2DBox2D(circle, static_cast<BoxCollider2D&>(aCol2));
		case Collider::ColliderType::Circle2D: return GetOverlapCircle2DCircle2D(circle, static_cast<CircleCollider2D&>(aCol2));
		}
	}
	default:
		return { 0,0 };
	}
}


bool Intersection::IntersectionPlaneRay(const Plane& aPlane, const Ray& aRay, const float& aMaxLength, Vector3f& aOutIntersectionPoint)
{
	float dot = aPlane.GetNormal().Dot(aRay.GetDirection());
	if (dot > -0.0001f && dot < 0.0001f)
	{
		return false;
	}


	if (aPlane.IsInside(aRay.GetOrigin()))
	{
		if (dot > 0)
		{
			float t = (aPlane.GetPoint().Dot(aPlane.GetNormal()) - (aRay.GetOrigin().Dot(aPlane.GetNormal()))) / (aRay.GetDirection().Dot(aPlane.GetNormal()));
			if (t > aMaxLength) return false;
			aOutIntersectionPoint = aRay.GetOrigin() + aRay.GetDirection() * t;
			return true;
		}
		else
		{
			return false;
		}

	}
	else
	{
		if (dot < 0)
		{
			float t = (aPlane.GetPoint().Dot(aPlane.GetNormal()) - (aRay.GetOrigin().Dot(aPlane.GetNormal()))) / (aRay.GetDirection().Dot(aPlane.GetNormal()));
			if (t > aMaxLength) return false;
			aOutIntersectionPoint = aRay.GetOrigin() + aRay.GetDirection() * t;
			return true;
		}
		else
		{
			return false;
		}
	}
}


bool Intersection::IntersectionAABBRay(const BoxCollider& aAABB, const Ray& aRay, const float& aMaxLength, Vector3f& aOutIntersectionPoint)
{
	Plane plane;

	Vector3f mn = aAABB.GetMinCorner();
	Vector3f mx = aAABB.GetMaxCorner();

	//front
	plane.InitWith3Points(mn, { mx.x, mn.y, mn.z }, { mx.x, mx.y, mn.z });
	if (IntersectionPlaneRay(plane, aRay, aMaxLength, aOutIntersectionPoint))
	{
		if (aAABB.IsInside(aOutIntersectionPoint))
		{
			return true;
		}
	}

	//Right
	plane.InitWith3Points({ mx.x, mn.y, mn.z }, { mx.x, mn.y, mx.z }, mx);
	if (IntersectionPlaneRay(plane, aRay, aMaxLength, aOutIntersectionPoint))
	{
		if (aAABB.IsInside(aOutIntersectionPoint))
		{
			return true;
		}
	}

	//top
	plane.InitWith3Points(mn, { mn.x, mn.y, mx.z }, { mx.x, mn.y, mx.z });
	if (IntersectionPlaneRay(plane, aRay, aMaxLength, aOutIntersectionPoint))
	{
		if (aAABB.IsInside(aOutIntersectionPoint))
		{
			return true;
		}
	}

	//left
	plane.InitWith3Points(mn, { mn.x, mn.y, mx.z }, { mn.x, mx.y, mx.z });
	if (IntersectionPlaneRay(plane, aRay, aMaxLength, aOutIntersectionPoint))
	{
		if (aAABB.IsInside(aOutIntersectionPoint))
		{
			return true;
		}
	}

	//bottom
	plane.InitWith3Points(mx, { mn.x, mx.y, mx.z }, { mx.x, mx.y, mn.z });
	if (IntersectionPlaneRay(plane, aRay, aMaxLength, aOutIntersectionPoint))
	{
		if (aAABB.IsInside(aOutIntersectionPoint))
		{
			return true;
		}
	}

	//back
	plane.InitWith3Points(mx, { mn.x, mx.y, mx.z }, { mn.x, mn.y,mx.z });
	if (IntersectionPlaneRay(plane, aRay, aMaxLength, aOutIntersectionPoint))
	{
		if (aAABB.IsInside(aOutIntersectionPoint))
		{
			return true;
		}
	}

	return false;
}

bool Intersection::IntersectionBox2DRay(const BoxCollider2D& aBox, const Ray& aRay, const float& aMaxLength, Vector3f& aOutIntersectionPoint)
{
	const Vector3f& rayOrg = aRay.GetOrigin();
	Vector3f invDir = Vector3f::one() / aRay.GetDirection();
	Vector2f boxMin = aBox.GetMinCorner();
	Vector2f boxMax = aBox.GetMaxCorner();

	float tx1 = (boxMin.x - rayOrg.x) * invDir.x;
	float tx2 = (boxMax.x - rayOrg.x) * invDir.x;

	float tmin = min(tx1, tx2);
	float tmax = max(tx1, tx2);

	float ty1 = (boxMin.y - rayOrg.y) * invDir.y;
	float ty2 = (boxMax.y - rayOrg.y) * invDir.y;

	tmin = max(tmin, min(ty1, ty2));
	if (tmin >= aMaxLength) return false;

	tmax = min(tmax, max(ty1, ty2));
	if (tmax < 0) return false;

	aOutIntersectionPoint.x = aRay.GetDirection().x * tmin + rayOrg.x;
	aOutIntersectionPoint.y = aRay.GetDirection().y * tmin + rayOrg.y;

	return tmax >= tmin;
}

bool Intersection::IntersectionCircle2DRay(const CircleCollider2D& aCircle, const Ray& aRay, const float& aMaxLength, Vector3f& aOutIntersectionPoint)
{
	Vector2f diff = (aCircle.GetPosition() - Vector2f{ aRay.GetOrigin().x, aRay.GetOrigin().y });
	float t = diff.Dot({ aRay.GetDirection().x, aRay.GetDirection().y });

	Vector3f closestPoint = aRay.GetOrigin() + aRay.GetDirection() * t;
	aOutIntersectionPoint = closestPoint;
	if ((closestPoint - aRay.GetOrigin()).LengthSqr() > aMaxLength * aMaxLength) return false;
	if (aRay.GetDirection().Dot(Vector3f(diff.x, diff.y, 0)) < 0) return false;
	float distSqr = (Vector2f{ closestPoint.x, closestPoint.y } - aCircle.GetPosition()).LengthSqr();

	return (distSqr <= aCircle.GetRadius() * aCircle.GetRadius());
}

bool Intersection::IntersectionSphereRay(const Catbox::Sphere& aSphere, const Ray& aRay, const float& /*aMaxLength*/)
{
	Vector3f diff = (aSphere.position - aRay.GetOrigin());
	float t = diff.Dot(aRay.GetDirection());

	Vector3f closestPoint = aRay.GetOrigin() + aRay.GetDirection() * t;

	return (aSphere.IsInside(closestPoint));
}

Vector2f Intersection::GetOverlapBox2DBox2D(const BoxCollider2D& aFirstBox, const BoxCollider2D& aSecondBox)
{
	Vector2f overlap;

	const Vector2f& aFirstMin = aFirstBox.GetMinCorner();
	const Vector2f& aFirstMax = aFirstBox.GetMaxCorner();

	const Vector2f& aSecondMin = aSecondBox.GetMinCorner();
	const Vector2f& aSecondMax = aSecondBox.GetMaxCorner();

	float overlapLeft = aSecondMax.x - aFirstMin.x;
	float overlapRight = aFirstMax.x - aSecondMin.x;
	overlap.x = overlapRight - overlapLeft > 0 ? -overlapLeft : overlapRight;

	float overlapTop = aSecondMax.y - aFirstMin.y;
	float overlapBottom = aFirstMax.y - aSecondMin.y;
	overlap.y = overlapBottom - overlapTop > 0 ? -overlapTop : overlapBottom;

	return overlap;
}

Vector2f Intersection::GetOverlapCircle2DBox2D(const CircleCollider2D& aCircle, const BoxCollider2D& aBox)
{
	const Vector2f& boxMin = aBox.GetMinCorner();
	const Vector2f& boxMax = aBox.GetMaxCorner();
	const Vector2f& circlePos = aCircle.GetPosition();
	const float circleRadius = aCircle.GetRadius();

	//float closestCornerX = std::abs(boxMin.x - circlePos.x) < std::abs(boxMax.x - circlePos.x) ? boxMin.x : boxMax.x;
	//float closestCornerY = std::abs(boxMin.y - circlePos.y) < std::abs(boxMax.y - circlePos.y) ? boxMin.y : boxMax.y;
	//float pointX = Catbox::Clamp(circlePos.x, boxMin.x, boxMax.x);
	//float pointY = Catbox::Clamp(circlePos.y, boxMin.y, boxMax.y);
	//Vector2f radiusDir = (aBox.GetPosition() - circlePos).GetNormalized() * circleRadius;
	////float overlapX = radiusDir - (radiusDir - Vector2f(closestCornerX, closestCornerY));

	//float overlapX = radiusDir.x - (pointX - circlePos.x);
	//float overlapY = radiusDir.y - (pointY - circlePos.y);

	Vector2f overlap;

	if (circlePos.y < aBox.GetPosition().y) //circle is closer to the bottom of box
	{
		Vector2f radiusDir = (boxMax - circlePos).GetNormalized() * circleRadius;
		overlap.y = (circlePos.y + circleRadius) + (circlePos.y - boxMin.y);
	}
	else //circle is closer to top of box
	{
		Vector2f radiusDir = (boxMin - circlePos).GetNormalized() * circleRadius;
		overlap.y = (circlePos.y - circleRadius) - (circlePos.y - boxMax.y);
	}

	if (circlePos.x < aBox.GetPosition().x) //circle is closer to the left of box
	{
		Vector2f radiusDir = (boxMax - circlePos).GetNormalized() * circleRadius;
		overlap.x = (circlePos.x + circleRadius) + (circlePos.x - boxMin.x);

	}
	else //circle is closer to right of box
	{
		Vector2f radiusDir = (boxMin - circlePos).GetNormalized() * circleRadius;
		overlap.x = (circlePos.x - circleRadius) - (circlePos.x - boxMax.x);
	}

	return circlePos - overlap;

	/*OverlapData overlap;

	const Vector2f boxPos = aBox.GetPosition();
	const Vector2f circlePos = aCircle.GetPosition();
	const Vector2f halfBoxSize = aBox.GetSize() / 2.f;
	const float& circleRadius = aCircle.GetRadius();

	overlap.overlapLeft = (circlePos.x + circleRadius) - (circlePos.x - halfBoxSize.x);
	overlap.overlapRight = (boxPos.x + halfBoxSize.x) - (circlePos.x - circleRadius);
	overlap.overlapTop = (circlePos.y + circleRadius) - (circlePos.y - halfBoxSize.y);
	overlap.overlapBottom = (boxPos.y + halfBoxSize.y) - (circlePos.y - circleRadius);

	return overlap;*/
}

Vector2f Intersection::GetOverlapBox2DCircle2D(const BoxCollider2D& aBox, const CircleCollider2D& aCircle)
{
	const Vector2f& boxMin = aBox.GetMinCorner();
	const Vector2f& boxMax = aBox.GetMaxCorner();
	const Vector2f& circlePos = aCircle.GetPosition();
	const float circleRadius = aCircle.GetRadius();

	Vector2f overlap;
	if (circlePos.y < aBox.GetPosition().y) //circle is closer to the bottom of box
	{
		Vector2f radiusDir = (boxMax - circlePos).GetNormalized() * circleRadius;
		overlap.y = (circlePos.y + circleRadius) + (circlePos.y - boxMin.y);
	}
	else //circle is closer to top of box
	{
		Vector2f radiusDir = (boxMin - circlePos).GetNormalized() * circleRadius;
		overlap.y = (circlePos.y - circleRadius) + (circlePos.y - boxMax.y);
	}

	if (circlePos.x < aBox.GetPosition().x) //circle is closer to the left of box
	{
		Vector2f radiusDir = (boxMax - circlePos).GetNormalized() * circleRadius;
		overlap.x = (circlePos.x + circleRadius) + (circlePos.x - boxMin.x);

	}
	else //circle is closer to right of box
	{
		Vector2f radiusDir = (boxMin - circlePos).GetNormalized() * circleRadius;
		overlap.x = (circlePos.x - circleRadius) + (circlePos.x - boxMax.x);
	}

	return circlePos - overlap;
}

Vector2f Intersection::GetOverlapCircle2DCircle2D(const CircleCollider2D& aFirstCircle, const CircleCollider2D& aSecondCircle)
{
	Vector2f diff = aSecondCircle.GetPosition() - aFirstCircle.GetPosition();
	Vector2f radiusDir1 = diff * aFirstCircle.GetRadius();
	Vector2f radiusDir2 = Vector2f{ -diff.x, -diff.y } *aSecondCircle.GetRadius();
	Vector2f overlap = diff - (diff - radiusDir1) - (diff - radiusDir2);

	/*OverlapData overlap;
	const Vector2f pos1 = aFirstCircle.GetPosition();
	const Vector2f pos2 = aSecondCircle.GetPosition();
	const float& rad1 = aFirstCircle.GetRadius();
	const float& rad2 = aSecondCircle.GetRadius();

	overlap.overlapLeft = (pos2.x + rad2) - (pos1.x - rad1);
	overlap.overlapRight = (pos1.x + rad1) - (pos2.x - rad2);
	overlap.overlapTop = (pos2.y + rad2) - (pos1.y - rad1);
	overlap.overlapBottom = (pos1.y + rad1) - (pos2.y - rad2);*/

	return overlap;
}


bool Intersection::IntersectionSphereSphere(const Catbox::Sphere& aFirstSphere, const Catbox::Sphere& aSecondSphere, Vector3f& aOutIntersectionPoint)
{
	Vector3f diff = aSecondSphere.position - aFirstSphere.position;
	aOutIntersectionPoint = aFirstSphere.position + diff.GetNormalized() * aFirstSphere.radius;
	float distSqr = diff.LengthSqr();
	return (distSqr < ((aFirstSphere.radius + aSecondSphere.radius * (aFirstSphere.radius + aSecondSphere.radius))));
}


bool Intersection::IntersectionAABBAABB(const BoxCollider& aFirstAABB, const BoxCollider& aSecondAABB)
{
	const Vector3f& aFirstMin = aFirstAABB.GetMinCorner();
	const Vector3f& aFirstMax = aFirstAABB.GetMaxCorner();

	const Vector3f& aSecondMin = aSecondAABB.GetMinCorner();
	const Vector3f& aSecondMax = aSecondAABB.GetMaxCorner();

	if (aFirstMin.x > aSecondMax.x) return false;
	if (aFirstMax.x < aSecondMin.x) return false;

	if (aFirstMin.y > aSecondMax.y) return false;
	if (aFirstMax.y < aSecondMin.y) return false;

	if (aFirstMin.z > aSecondMax.z) return false;
	if (aFirstMax.z < aSecondMin.z) return false;

	return true;
}


bool Intersection::IntersectionAABBSphere(const BoxCollider& aAABB, const Catbox::Sphere& aSphere, Vector3f& aOutIntersectionPoint)
{
	float sqDist = 0;

	// for each axis count any excess distance outside box extents
	Vector3f minCorner = aAABB.GetMinCorner();
	Vector3f maxCorner = aAABB.GetMaxCorner();
	float v = aSphere.position.x;
	if (v < minCorner.x)
	{
		sqDist += (minCorner.x - v) * (minCorner.x - v);
		aOutIntersectionPoint.x = minCorner.x;
	}
	else if (v > maxCorner.x)
	{
		sqDist += (v - maxCorner.x) * (v - maxCorner.x);
		aOutIntersectionPoint.x = maxCorner.x;
	}

	v = aSphere.position.y;
	if (v < minCorner.y)
	{
		sqDist += (minCorner.y - v) * (minCorner.y - v);
		aOutIntersectionPoint.y = minCorner.y;
	}
	else if (v > maxCorner.y)
	{
		sqDist += (v - maxCorner.y) * (v - maxCorner.y);
		aOutIntersectionPoint.y = maxCorner.y;
	}

	v = aSphere.position.z;
	if (v < minCorner.z)
	{
		sqDist += (minCorner.z - v) * (minCorner.z - v);
		aOutIntersectionPoint.z = minCorner.z;
	}
	else if (v > maxCorner.z)
	{
		sqDist += (v - maxCorner.z) * (v - maxCorner.z);
		aOutIntersectionPoint.z = maxCorner.z;
	}

	return sqDist <= aSphere.radius * aSphere.radius;
}

bool Intersection::IntersectionCircle2DBox2D(const CircleCollider2D& aCircle, const BoxCollider2D& aBox)
{
	const Vector2f& boxPos = aBox.GetPosition();
	const Vector2f& boxSize = aBox.GetSize();
	const Vector2f& circlePos = aCircle.GetPosition();
	const float& circleRadius = aCircle.GetRadius();

	float testX = Catbox::Clamp(circlePos.x, boxPos.x - boxSize.x / 2.f, boxPos.x + boxSize.x / 2.f);
	float testY = Catbox::Clamp(circlePos.y, boxPos.y - boxSize.y / 2.f, boxPos.y + boxSize.y / 2.f);

	float sqrDist = (circlePos - Vector2f(testX, testY)).LengthSqr();
	return (sqrDist <= circleRadius * circleRadius);
}


bool Intersection::IntersectionCircle2DCircle2D(const CircleCollider2D& aFirstCircle, const CircleCollider2D& aSecondCircle)
{
	float sqrDist = (aSecondCircle.GetPosition() - aFirstCircle.GetPosition()).LengthSqr();
	return (sqrDist <= ((aSecondCircle.GetRadius() + aFirstCircle.GetRadius()) * (aSecondCircle.GetRadius() + aFirstCircle.GetRadius())));
}

bool Intersection::IntersectionBox2DBox2D(const BoxCollider2D& aFirstBox, const BoxCollider2D& aSecondBox)
{
	Vector2f firstMin = aFirstBox.GetMinCorner();
	Vector2f firstMax = aFirstBox.GetMaxCorner();
	Vector2f otherMin = aSecondBox.GetMinCorner();
	Vector2f otherMax = aSecondBox.GetMaxCorner();

	if (otherMax.x < firstMin.x) return false;
	if (otherMin.x > firstMax.x) return false;
	if (otherMax.y < firstMin.y) return false;
	if (otherMin.y > firstMax.y) return false;

	return true;
}
