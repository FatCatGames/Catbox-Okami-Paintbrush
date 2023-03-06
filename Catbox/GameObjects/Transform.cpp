#include "stdafx.h"
#include "Transform.h"
#include <UtilityFunctions.hpp>
#include "ComponentTools\SceneManager.h"
#include <Editor/Commands/EditorCommandHandler.h>
#include <Editor/Commands/TransformCommand.h>


bool Transform::ImguiRenderWorldTransform = false;
Vector3f startValue = { 0,0,0 };
Vector3f valueLastFrame = { 0,0,0 };

Transform::Transform()
{
	myScale = Vector3f::one();
	myRotation = Vector3f::zero();
	myRootParent = this;
}

Transform::~Transform()
{
	if (myParent)
	{
		if (myBoneParentId != -1)
		{
			myParent->myGameObject->myTransform.RemoveChild(this);
		}
		myParent->RemoveChild(this);
	}
}

const Vector3f Transform::localPos()
{
	return myPosition;
}

const Vector3f Transform::worldPos()
{
	return
	{
		myWorldTransform(4,1),
		myWorldTransform(4,2),
		myWorldTransform(4,3)
	};
}

const Vector3f Transform::worldRot()
{
	Vector3f worldRot = myRotation;
	Transform* nextParent = myParent;
	while (nextParent != nullptr)
	{
		worldRot += nextParent->myRotation;
		nextParent = nextParent->myParent;
	}

	return worldRot;
}

const Vector3f Transform::worldScale()
{
	Vector3f worldScale = myScale;
	Transform* nextParent = myParent;
	while (nextParent != nullptr)
	{
		worldScale *= nextParent->myScale;
		nextParent = nextParent->myParent;
	}

	return worldScale;
}

void Transform::SetWorldTransform(const Vector3f& aNewPos, const Vector3f& aNewRot, const Vector3f& aNewScale)
{
	if (!Engine::GetInstance()->GetSceneManager()->SceneIsLoading())
	{
		Catbox::Matrix4x4<float> rotationMatrix = Catbox::Matrix4x4<float>();

		rotationMatrix = rotationMatrix.CreateRotationAroundZ(Catbox::Deg2Rad(aNewRot.z)) * rotationMatrix;
		rotationMatrix = rotationMatrix.CreateRotationAroundY(Catbox::Deg2Rad(aNewRot.y)) * rotationMatrix;
		rotationMatrix = rotationMatrix.CreateRotationAroundX(Catbox::Deg2Rad(aNewRot.x)) * rotationMatrix;

		Vector4f scale = { aNewScale.x, aNewScale.y, aNewScale.z, 1 };
		Catbox::Matrix4x4<float> scaleMatrix;
		scaleMatrix = scaleMatrix * scale;

		myWorldTransform = scaleMatrix * rotationMatrix;

		myWorldTransform(4, 1) = aNewPos.x;
		myWorldTransform(4, 2) = aNewPos.y;
		myWorldTransform(4, 3) = aNewPos.z;

		//if (myIsBone) return;
		myLocalTransform = toLocalSpace();
		myPosition.x = myLocalTransform(4, 1);
		myPosition.y = myLocalTransform(4, 2);
		myPosition.z = myLocalTransform(4, 3);
		myRotation = myLocalTransform.GetRotationFromMatrix(myLocalTransform);
		myScale = myLocalTransform.GetScaleFromMatrix(myLocalTransform);

		UpdateTransform();
	}
}

void Transform::SetLocalTransform(const Vector3f& aNewPos, const Vector3f& aNewRot, const Vector3f& aNewScale)
{
	myRotation = aNewRot;
	myRotationMatrix = Catbox::Matrix4x4<float>();

	myRotationMatrix = myRotationMatrix.CreateRotationAroundZ(Catbox::Deg2Rad(aNewRot.z)) * myRotationMatrix;
	myRotationMatrix = myRotationMatrix.CreateRotationAroundY(Catbox::Deg2Rad(aNewRot.y)) * myRotationMatrix;
	myRotationMatrix = myRotationMatrix.CreateRotationAroundX(Catbox::Deg2Rad(aNewRot.x)) * myRotationMatrix;

	myPosition = aNewPos;
	myScale = aNewScale;

	RebuildMatrix();
	UpdateTransform();
}

void Transform::SetLocalPos(const Vector3f& aNewPos)
{
	myPosition = aNewPos;
	RebuildMatrix();
	UpdateTransform();
}

void Transform::SetLocalPos(const float& aX, const float& aY, const float& aZ)
{
	SetLocalPos({ aX, aY, aZ });
}

void Transform::SetWorldPos(const Vector3f& aNewPos)
{
	myWorldTransform(4, 1) = aNewPos.x;
	myWorldTransform(4, 2) = aNewPos.y;
	myWorldTransform(4, 3) = aNewPos.z;
	myLocalTransform = toLocalSpace();

	myPosition.x = myLocalTransform(4, 1);
	myPosition.y = myLocalTransform(4, 2);
	myPosition.z = myLocalTransform(4, 3);
	RebuildMatrix();
	UpdateTransform();
}

void Transform::SetWorldPos(const float& aX, const float& aY, const float& aZ)
{
	SetWorldPos({ aX, aY, aZ });
}

void Transform::Translate(const Vector3f& aVector)
{
	if (aVector == Vector3f::zero()) return;
	SetWorldPos(worldPos() + aVector);
	UpdateTransform();
}

void Transform::Translate(const float& aX, const float& aY, const float& aZ)
{
	Translate({ aX, aY, aZ });
}

void Transform::SetLocalRot(const Vector3f& aNewRot)
{
	myRotation = aNewRot;
	myRotationMatrix = Catbox::Matrix4x4<float>();

	myRotationMatrix = myRotationMatrix.CreateRotationAroundZ(Catbox::Deg2Rad(aNewRot.z)) * myRotationMatrix;
	myRotationMatrix = myRotationMatrix.CreateRotationAroundY(Catbox::Deg2Rad(aNewRot.y)) * myRotationMatrix;
	myRotationMatrix = myRotationMatrix.CreateRotationAroundX(Catbox::Deg2Rad(aNewRot.x)) * myRotationMatrix;

	RebuildMatrix();
	UpdateTransform();
}

void Transform::SetLocalRot(const float& aX, const float& aY, const float& aZ)
{
	SetLocalRot({ aX, aY, aZ });
}

void Transform::SetWorldRot(const Vector3f& aNewRot)
{
	myRotation = aNewRot;
	Transform* parent = myParent;
	while (parent != nullptr)
	{
		myRotation -= parent->myRotation;
		parent = parent->myParent;
	}

	SetLocalRot(myRotation);
}

void Transform::SetWorldRot(const float& aX, const float& aY, const float& aZ)
{
	SetWorldRot({ aX, aY, aZ });
}

void Transform::Rotate(const float& aX, const float& aY, const float& aZ)
{
	Rotate({ aX, aY, aZ });
}

void Transform::Rotate(const Vector3f& aRotation)
{
	SetWorldRot(myRotation + aRotation);
}

void Transform::SetLocalScale(const Vector3f& aNewScale)
{
	myScale = aNewScale;
	RebuildMatrix();
	UpdateTransform();
}

void Transform::SetLocalScale(const float& aX, const float& aY, const float& aZ)
{
	SetLocalScale({ aX, aY, aZ });
}

void Transform::SetWorldScale(const Vector3f& aNewScale)
{
	myScale = aNewScale;
	Transform* parent = myParent;
	while (parent != nullptr)
	{
		myScale /= parent->myScale;
		parent = parent->myParent;
	}

	RebuildMatrix();
	UpdateTransform();
}

void Transform::SetWorldScale(const float& aX, const float& aY, const float& aZ)
{
	SetWorldScale({ aX, aY, aZ });
}

void Transform::SetCenter(Vector3f aCenter)
{
	myCenter = aCenter;
}
void Transform::SetBoundsRadius(float aBounds)
{
	myBoundsRadius = aBounds;
}

void Transform::LookAt(const Vector3f& aTarget)
{
	Vector3f distance = aTarget - worldPos();

	float xzDistance = sqrtf(distance.x * distance.x + distance.z * distance.z);

	SetLocalRot(Catbox::Rad2Deg(-atan2f(distance.y, xzDistance)), Catbox::Rad2Deg(-atan2f(-distance.x, distance.z)), 0.0f);
}


void Transform::LookAtHorizontal(const Vector3f& aTarget)
{
	Vector3f dir = aTarget - worldPos();
	float angleY = (std::atan2(dir.x, dir.z) * 57.3f);
	SetLocalRot(0, angleY, 0);
}

void Transform::Reset()
{
	myLocalTransform.Reset();
	myPosition = Vector3f::zero();
	myRotation = Vector3f::zero();
	myScale = Vector3f::one();
	myWorldTransform = toWorldSpace();
}

Catbox::Matrix4x4<float> Transform::toWorldSpace()
{
	Catbox::Matrix4x4<float> matrixResult(myLocalTransform);

	if (myParent != nullptr)
	{
		matrixResult *= myParent->myWorldTransform;
	}
	return matrixResult;
}

Catbox::Matrix4x4<float> Transform::toLocalSpace()
{
	Catbox::Matrix4x4<float> matrixResult(myWorldTransform);

	if (myParent != nullptr)
	{
		matrixResult *= Catbox::Matrix4x4<float>::GetFastInverse(myParent->myWorldTransform);
	}
	return matrixResult;
}

void Transform::AddChild(Transform& aChild)
{
	myChildren.push_back(&aChild);
}

void Transform::RemoveChild(Transform* aChild)
{
	for (size_t i = 0; i < myChildren.size(); i++)
	{
		if (myChildren[i] == aChild)
		{
			myChildren.erase(myChildren.begin() + i);
			break;
		}
	}
}

void Transform::SetParent(Transform* aTransform)
{
	if (aTransform == this) return;
	myScale = worldScale();
	myRotation = worldRot();
	myLocalTransform = myWorldTransform;

	if (myParent != nullptr)
	{
		if (myBoneParentId != -1)
		{
			myParent->myGameObject->GetTransform()->RemoveChild(this);
			myBoneParentId = -1;
		}
		myParent->RemoveChild(this);
		if (!aTransform)
		{
			if (Scene* scene = Engine::GetInstance()->GetSceneManager()->GetCurrentScene())
			{
				scene->AddRootObject(myGameObject);
			}
		}
	}
	else
	{
		if (aTransform)
		{
			if (Scene* scene = Engine::GetInstance()->GetSceneManager()->GetCurrentScene())
			{
				scene->RemoveRootObject(myGameObject);
			}
		}
	}

	myParent = aTransform;
	Transform* parent = myParent;
	myRootParent = this;
	while (parent != nullptr)
	{
		myRotation += parent->myRotation;
		myScale /= parent->myScale;
		myRootParent = parent;
		parent = parent->myParent;
	}

	if (myParent != nullptr)
	{
		myLocalTransform = toLocalSpace();
		myParent->AddChild(*this);
		myPosition =
		{
			myLocalTransform(4, 1),
			myLocalTransform(4, 2),
			myLocalTransform(4, 3)
		};
	}

	UpdateTransform();
}

std::vector<Transform*>& Transform::GetChildren()
{
	return myChildren;
}

const Catbox::Matrix4x4<float>& Transform::GetWorldTransformMatrix()
{
	return myWorldTransform;
}

const Catbox::Matrix4x4<float>& Transform::GetLocalTransformMatrix()
{
	return myLocalTransform;
}

void Transform::RebuildMatrix()
{
	Vector4f scale = { myScale.x, myScale.y, myScale.z, 1 };
	Catbox::Matrix4x4<float> scaleMatrix;
	scaleMatrix = scaleMatrix * scale;
	myLocalTransform = scaleMatrix * myRotationMatrix;
	myLocalTransform(4, 1) = myPosition.x;
	myLocalTransform(4, 2) = myPosition.y;
	myLocalTransform(4, 3) = myPosition.z;

	if (myIsBone) return;
	myWorldTransform = toWorldSpace();
}

void Transform::SetWorldTransformMatrix(const Catbox::Matrix4x4<float>& aMatrix)
{
	myWorldTransform = aMatrix;
	myLocalTransform = toLocalSpace();

	myPosition.x = myLocalTransform(4, 1);
	myPosition.y = myLocalTransform(4, 2);
	myPosition.z = myLocalTransform(4, 3);

	myScale = Catbox::Matrix4x4<float>::GetScaleFromMatrix(aMatrix);
	myRotation = Catbox::Matrix4x4<float>::GetRotationFromMatrix(aMatrix);
	Transform* parent = myParent;
	while (parent != nullptr)
	{
		myScale -= parent->myScale;
		myRotation -= parent->myRotation;
		myPosition -= parent->myPosition;
		parent = parent->myParent;
	}

	myRotationMatrix = Catbox::Matrix4x4<float>();
	myRotationMatrix = myRotationMatrix.CreateRotationAroundZ(Catbox::Deg2Rad(myRotation.z)) * myRotationMatrix;
	myRotationMatrix = myRotationMatrix.CreateRotationAroundY(Catbox::Deg2Rad(myRotation.y)) * myRotationMatrix;
	myRotationMatrix = myRotationMatrix.CreateRotationAroundX(Catbox::Deg2Rad(myRotation.x)) * myRotationMatrix;

	UpdateTransform();
}

void Transform::SetLocalTransformMatrix(const Catbox::Matrix4x4<float>& aMatrix)
{
	myLocalTransform = aMatrix;
	myWorldTransform = toWorldSpace();
	myScale = Catbox::Matrix4x4<float>::GetScaleFromMatrix(myWorldTransform);
	myRotation = Catbox::Matrix4x4<float>::GetRotationFromMatrix(myWorldTransform);
}


void Transform::UpdateChildren()
{
	for (size_t i = 0; i < myChildren.size(); i++)
	{
		if (!myIsBone && myChildren[i]->myBoneParentId != -1) continue;
		myChildren[i]->UpdateWorldTransform();
		myChildren[i]->UpdateTransform();
	}
}

void Transform::UpdateWorldTransform()
{
	myWorldTransform = toWorldSpace();
}

void Transform::UpdateTransform()
{
	if (myGameObject && !myIsBone)
	{
		myGameObject->UpdateTransform();
	}

	UpdateChildren();
}


void Transform::RenderInProperties(std::vector<Transform*>& aTransformList)
{
	if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Spacing();
		bool snapping = ImGui::IsKeyDown(ImGuiKey_LeftCtrl);
		std::string label = (std::string("Show ") + (ImguiRenderWorldTransform ? "Local" : "World") + " Transform");
		bool world = ImguiRenderWorldTransform;
		if (ImGui::Button(label.c_str()))
		{
			ImguiRenderWorldTransform = !world;
		}

		bool positionUniform[3] = { true, true, true };
		Vector3f position = world ? aTransformList[0]->worldPos() : aTransformList[0]->localPos();
		Vector3f prevPos = position;
		bool rotationUniform[3] = { true, true, true };
		Vector3f rotation = world ? aTransformList[0]->worldRot() : aTransformList[0]->localRot();
		bool scaleUniform[3] = { true, true, true };
		Vector3f scale = world ? aTransformList[0]->worldScale() : aTransformList[0]->localScale();

		if (aTransformList[0]->myBoneParentId != -1)
		{
			ImGui::Text("Parented to bone");
			Vector3f parentPos = aTransformList[0]->myParent->worldPos();
			ImGui::Text(("Parent bone world pos: " + std::to_string(parentPos.x) + "," + std::to_string(parentPos.y) + "," + std::to_string(parentPos.z)).c_str());
		}
		//POSITION
		Mode mode = world ? Mode::World : Mode::Local;
		float speed = snapping ? 0.5f : 0.1f;
		label = (std::string(world ? "World" : "Local") + " Position##" + std::to_string(aTransformList[0]->GetGameObject()->myInstanceID));
		bool editPos = Catbox::DragFloat3(label.c_str(), &position, speed, -1000, 1000);
		if (ImGui::IsItemActivated())
		{
			startValue = position;
		}

		if (editPos)
		{
			bool edit[3];
			edit[0] = prevPos.x != position.x;
			edit[1] = prevPos.y != position.y;
			edit[2] = prevPos.z != position.z;
			valueLastFrame = position;
			for (auto& transform : aTransformList)
			{
				Vector3f objectPos = world ? transform->worldPos() : transform->localPos();
				if (world)
				{
					transform->SetWorldPos(position);
				}
				else
				{
					transform->SetLocalPos(position);
				}
			}

			valueLastFrame = position;
		}

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Vector3f translation = position - startValue;
			EditorCommandHandler::AddCommand(std::make_shared<TransformCommand>(aTransformList, Transform::Operation::Translate, mode, translation));
		}

		//ROTATION
		//float rot[3] = { rotation.x, rotation.y, rotation.z };
		speed = snapping ? 0.5f : 0.1f;
		label = (std::string(world ? "World" : "Local") + " Rotation" +std::to_string(aTransformList[0]->GetGameObject()->myInstanceID));
		bool editRotation = Catbox::DragFloat3(label.c_str(), &rotation, speed, -1000, 1000);
		if (ImGui::IsItemActivated())
		{
			startValue = rotation;
		}

		if (editRotation)
		{
			//bool edit[3];
			//edit[0] = rot[0] != rotation.x;
			//edit[1] = rot[1] != rotation.y;
			//edit[2] = rot[2] != rotation.z;
			valueLastFrame = rotation;
			for (auto& transform : aTransformList)
			{
				Vector3f objectRot = world ? transform->worldRot() : transform->localRot();
				if (world)
				{
					//transform->SetWorldRot(edit[0] ? rot[0] : objectRot.x, edit[1] ? rot[1] : objectRot.y, edit[2] ? rot[2] : objectRot.z);
					transform->SetWorldRot(rotation);
				}
				else
				{
					//transform->SetLocalRot(edit[0] ? rot[0] : objectRot.x, edit[1] ? rot[1] : objectRot.y, edit[2] ? rot[2] : objectRot.z);
					transform->SetLocalRot(rotation);
				}
			}

			//rotation = { rot[0], rot[1], rot[2] };
			valueLastFrame = rotation;
		}

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Vector3f translation = rotation - startValue;
			EditorCommandHandler::AddCommand(std::make_shared<TransformCommand>(aTransformList, Transform::Operation::Rotate, mode, translation));
		}

		//SCALE
		//float scaleArray[3] = { scale.x, scale.y, scale.z };
		speed = snapping ? 0.5f : 0.1f;
		label = (std::string(world ? "World" : "Local") + " Scale");
		bool editScale = Catbox::DragFloat3(label.c_str(), &scale, speed, -1000, 1000);
		if (ImGui::IsItemActivated())
		{
			startValue = scale;
		}

		if (editScale)
		{
			/*
			bool edit[3];
			edit[0] = scaleArray[0] != scale.x;
			edit[1] = scaleArray[1] != scale.y;
			edit[2] = scaleArray[2] != scale.z;*/


			valueLastFrame = scale;
			for (auto& transform : aTransformList)
			{
				Vector3f objectScale = world ? transform->worldScale() : transform->localScale();
				if (world)
				{
					//transform->SetWorldScale(edit[0] ? scaleArray[0] : objectScale.x, edit[1] ? scaleArray[1] : objectScale.y, edit[2] ? scaleArray[2] : objectScale.z);
					transform->SetWorldScale(scale);
				}
				else
				{
					//transform->SetLocalScale(edit[0] ? scaleArray[0] : objectScale.x, edit[1] ? scaleArray[1] : objectScale.y, edit[2] ? scaleArray[2] : objectScale.z);
					transform->SetLocalScale(scale);
				}
			}

			//scale = { scaleArray[0], scaleArray[1], scaleArray[2] };
			valueLastFrame = scale;
		}

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Vector3f translation = scale - startValue;
			EditorCommandHandler::AddCommand(std::make_shared<TransformCommand>(aTransformList, Transform::Operation::Scale, mode, translation));
		}

		ImGui::TreePop();
	}
}

void Transform::Save()
{
	int version = 2;
	auto& wrapper = *RapidJsonWrapper::GetInstance();
	auto& alloc = wrapper.GetDocument().GetAllocator();
	auto transformObj = wrapper.CreateObject();
	auto gameObject = wrapper.GetTarget();

	wrapper.SetTarget(transformObj);
	wrapper.SaveValue<DataType::Int>("Version", version);
	wrapper.SaveValue<DataType::Int>("BoneParentID", myBoneParentId);

	auto positionArray = wrapper.CreateArray();
	positionArray.SetArray();
	Vector3f pos = localPos();
	positionArray.PushBack(pos.x, alloc);
	positionArray.PushBack(pos.y, alloc);
	positionArray.PushBack(pos.z, alloc);
	wrapper.SetTarget(transformObj);
	wrapper.SaveObject("Pos", positionArray);

	auto rotationArray = wrapper.CreateArray();
	rotationArray.SetArray();
	Vector3f rot = localRot();
	rotationArray.PushBack(rot.x, alloc);
	rotationArray.PushBack(rot.y, alloc);
	rotationArray.PushBack(rot.z, alloc);
	wrapper.SetTarget(transformObj);
	wrapper.SaveObject("Rot", rotationArray);

	auto scaleArray = wrapper.CreateArray();
	scaleArray.SetArray();
	Vector3f scale = localScale();
	scaleArray.PushBack(scale.x, alloc);
	scaleArray.PushBack(scale.y, alloc);
	scaleArray.PushBack(scale.z, alloc);
	wrapper.SetTarget(transformObj);
	wrapper.SaveObject("Size", scaleArray);

	auto childrenArray = wrapper.CreateArray();
	childrenArray.SetArray();
	for (size_t i = 0; i < myChildren.size(); i++)
	{
		wrapper.SetTarget(childrenArray);
		myChildren[i]->GetGameObject()->SaveObjectData();
	}
	wrapper.SetTarget(transformObj);
	wrapper.SaveObject("Children", childrenArray);

	wrapper.SetTarget(*gameObject);
	wrapper.SaveObject("Transform", transformObj);
}

void Transform::Load(rapidjson::Value& aTransformData)
{
	for (int i = 0; i < myChildren.size(); i++)
	{
		myChildren[i]->myGameObject->Destroy();
	}
	myChildren.clear();

	auto position = aTransformData["Pos"].GetArray();
	myPosition.x = position[0].GetFloat();
	myPosition.y = position[1].GetFloat();
	myPosition.z = position[2].GetFloat();

	auto rotation = aTransformData["Rot"].GetArray();
	SetLocalRot(rotation[0].GetFloat(), rotation[1].GetFloat(), rotation[2].GetFloat());

	auto scale = aTransformData["Size"].GetArray();
	SetLocalScale(scale[0].GetFloat(), scale[1].GetFloat(), scale[2].GetFloat());

	RebuildMatrix();
}

void Transform::LoadChildren(rapidjson::Value& aTransformData)
{
	int version = aTransformData["Version"].GetInt();

	auto children = aTransformData["Children"].GetArray();
	for (auto& child : children)
	{
		GameObject* newObject;
		if (myGameObject->myInstanceID != -1) newObject = Instantiate(this);
		else
		{
			newObject = new GameObject();
			newObject->GetTransform()->SetParent(this);
		}
		newObject->LoadObjectData(child.GetObj());
	}

	if (version > 1)
	{
		myBoneParentId = aTransformData["BoneParentID"].GetInt();
		if (myBoneParentId != -1)
		{
			myParent->myGameObject->myChildrenToBones.push_back(this);
		}
	}
}
