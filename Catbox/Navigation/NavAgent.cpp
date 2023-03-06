#include "stdafx.h"
#include "NavAgent.h"
#include "Pathfinding.h"
#include "CommonUtilities\InputHandler.h"
#include "Debugging\DebugDrawer.h"
#include "StringPulling.h"
#include "DetourNavMeshQuery.h"

void NavAgent::Init()
{
	myDtFilter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL ^ SAMPLE_POLYFLAGS_DISABLED);
	myDtFilter.setExcludeFlags(0);
	// Change costs.
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_GROUND, 1.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_WATER, 10.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_ROAD, 1.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_DOOR, 1.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_GRASS, 2.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_JUMP, 1.5f);
}

void NavAgent::Awake()
{
	myDtFilter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL ^ SAMPLE_POLYFLAGS_DISABLED);
	myDtFilter.setExcludeFlags(0);
	// Change costs.
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_GROUND, 1.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_WATER, 10.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_ROAD, 1.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_DOOR, 1.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_GRASS, 2.0f);
	myDtFilter.setAreaCost(SAMPLE_POLYAREA_JUMP, 1.5f);
}

void NavAgent::Update()
{
	if (!myTargetReached && !myPathFailed)
	{
		if (!myUsingCharacterController) 
		{
			myTransform->Translate(myDir * mySpeed * deltaTime);
		}
		myTransform->LookAtHorizontal(myPath[myPathIndex]);
		Vector3f aVector = myPath[myPathIndex];
		Vector3f anotherVector = myTransform->worldPos();
		anotherVector.y = aVector.y;

		if ((aVector - anotherVector).LengthSqr() < 0.1f)
		{
			if (++myPathIndex >= myPath.size())
			{
				myTargetReached = true;
			}
			else
			{
				myDir = (myPath[myPathIndex] - myTransform->worldPos()).GetNormalized();
			}
		}
	}
}

void NavAgent::RunInEditor()
{
	if (myGameObject->IsSelected(0))
	{
		DebugDrawer::DrawSphere(myTargetPosition, 0.5f, Color::Cyan());
	}
}

bool NavAgent::FindPath(const Vector3f& aTarget, bool forcePathFinding)
{
	if (aTarget == myTargetPosition && !forcePathFinding) { return false; };
	myPathIndex = 0;
	myPath.clear();
	myPathFailed = false;
	myTargetReached = false;
	//myTargetPosition = aTarget;
	//myStartPoint = myTransform->worldPos();
	//myLeftFunnelPoints.clear();
	//myRightFunnelPoints.clear();

	//if (!myNavMeshToUse)
	//{
	//	printerror("Tried to find path with " + myGameObject->GetName() + " but no navmesh was assigned");
	//	return false;
	//}

	//myPath = Navigation::FindPath(myTransform->worldPos(), aTarget, *myNavMeshToUse, path);

	//if (myPath.empty())
	//{
	//	myPathFailed = true;
	//	return false;
	//}

	//myTargetReached = false;
	//myPathIndex = 0;
	//myDir = (myPath[myPathIndex] - myTransform->worldPos()).GetNormalized();

	float temp[3];

	auto navmesh = myNavMeshToUse->GetDtNavMeshQuery();
	dtPolyRef startPoly = 1;

	float posArray[3];
	Vector3f pos = myTransform->worldPos();
	posArray[0] = pos.x;
	posArray[1] = pos.y;
	posArray[2] = pos.z;

	float sizeArray[3];
	Vector3f size = myTransform->worldScale();
	sizeArray[0] = size.x;
	sizeArray[1] = size.y;
	sizeArray[2] = size.z;

	float random = 0.1f;
	float (*fpi)() = []() {return Catbox::GetRandom<float>(0, 1.0f);};
	dtTileRef aRef;
	
	//float realPos[3];
	//realPos[0] = 0;
	//realPos[1] = 0;
	//realPos[2] = 0;
	//auto status = navmesh->findRandomPoint(&myDtFilter, fpi, &aRef, realPos);
	//if (dtStatusFailed(status))
	//{
	//	return false;
	//}
	auto status = navmesh->findNearestPoly(posArray, sizeArray, &myDtFilter, &startPoly, temp);
	if (dtStatusFailed(status)) 
	{
		return false;
	}

	dtPolyRef endPoly = 1;

	float targetPosArray[3];
	Vector3f targetPos = aTarget;
	targetPosArray[0] = targetPos.x;
	targetPosArray[1] = targetPos.y;
	targetPosArray[2] = targetPos.z;

	status = navmesh->findNearestPoly(targetPosArray, sizeArray, &myDtFilter, &endPoly, temp);
	if (dtStatusFailed(status))
	{
		myPath.resize(1);
		myPath[0] = aTarget;
		return false;
	}
	dtPolyRef path[sizeof(dtPolyRef) * 255];

	int pathSize = 0;

	status = navmesh->findPath(startPoly, endPoly, posArray, targetPosArray, &myDtFilter, path, &pathSize, myMaxPathSize);
	if (dtStatusFailed(status))
	{
		myPathFailed = true;
		return false;
	}

	if (pathSize > 0) 
	{
		float finalTargetPosArray[3];
		finalTargetPosArray[0] = targetPos.x;
		finalTargetPosArray[1] = targetPos.y;
		finalTargetPosArray[2] = targetPos.z;
		if (path[pathSize - 1] != endPoly)
		{
			navmesh->closestPointOnPoly(path[pathSize - 1], targetPosArray, finalTargetPosArray, 0);
		}

		float pathPos[sizeof(float) * 3 * 255];
		int straightPathSize = 0;

		status = navmesh->findStraightPath(posArray, finalTargetPosArray, path, pathSize, pathPos, nullptr, nullptr, &straightPathSize, myMaxPathSize);
		if (dtStatusFailed(status))
		{
			myPathFailed = true;
			return false;
		}

		for (int i = 0; i < straightPathSize; i++)
		{
			Vector3f aPos;
			aPos.x = pathPos[i * 3];
			aPos.y = pathPos[i * 3 + 1];
			aPos.z = pathPos[i * 3 + 2];
			myPath.push_back(aPos);
		}

		if (myPath.size() < 1) 
		{
			myPathFailed = true;
		}
	}
	return true;
}

void NavAgent::ChangeSpeed(float aMovmentSpeed)
{
	mySpeed = aMovmentSpeed;
}

void NavAgent::ResumePathfinding()
{
	myTargetReached = myPathIndex >= myPath.size();
}

void NavAgent::RenderInProperties(std::vector<Component*>& aComponentList)
{
	auto agents = ComponentVectorCast<NavAgent>(aComponentList);

	bool edit;
	auto newNavMesh = AssetRegistry::GetInstance()->AssetDropdownMenu<NavMesh>(myNavMeshToUse.get(), "NavMesh", edit);
	if (edit)
	{
		for (auto& agent : agents)
		{
			agent->myNavMeshToUse = newNavMesh;
		}
	}

	if (Catbox::DragFloat("Speed", &mySpeed, 0.1f, 0, 100))
	{
		for (auto& agent : agents)
		{
			agent->mySpeed = mySpeed;
		}
	}

	if (ImGui::Button("Stop navigation")) myTargetReached = true;

	float target[3] = { myTargetPosition.x, myTargetPosition.y, myTargetPosition.z };
	if (ImGui::DragFloat3("Target", target, 0.05f))
	{
		myTargetPosition.x = target[0];
		myTargetPosition.y = target[1];
		myTargetPosition.z = target[2];
	}

	if (ImGui::Button("FindPath")) 
	{
		FindPath(myTargetPosition, true);
	}
}

std::shared_ptr<NavMesh> NavAgent::GetNavMesh() const
{
	return myNavMeshToUse;
}

void NavAgent::Save(rapidjson::Value& /*aComponentData*/)
{
	int version = 0;
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	wrapper.SaveValue<DataType::Int>("Version", version);

	const char* navName = myNavMeshToUse ? myNavMeshToUse->GetName().c_str() : "";
	wrapper.SaveString("NavMesh", navName);
	wrapper.SaveValue<DataType::Float>("Speed", mySpeed);
}

void NavAgent::Load(rapidjson::Value& aComponentData)
{
	if (aComponentData.HasMember("Version"))
	{
		//int version = aComponentData["Version"].GetInt();
		mySpeed = aComponentData["Speed"].GetFloat();
		myNavMeshToUse = AssetRegistry::GetInstance()->GetAsset<NavMesh>(aComponentData["NavMesh"].GetString());
	}
}

void NavAgent::PausePathfinding()
{
	myTargetReached = true;
}

void NavAgent::StopPathFinding()
{
	myTargetReached = true;
	myPath.clear();
}
