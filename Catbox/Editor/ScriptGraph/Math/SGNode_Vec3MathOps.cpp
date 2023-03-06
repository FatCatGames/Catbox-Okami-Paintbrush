#include "stdafx.h"
#include "SGNode_Vec3MathOps.h"


#pragma region Add
void SGNode_Vec3Add::Init()
{
	SetTitle("Add Vector3");
	SetCategory("Vector Math");

	CreateDataPin<Vector3f>("A", PinDirection::Input);
	CreateDataPin<Vector3f>("B", PinDirection::Input);

	CreateDataPin<Vector3f>("Result", PinDirection::Output);
}

size_t SGNode_Vec3Add::DoOperation()
{
	Vector3f inA;
	Vector3f inB;

	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		const Vector3f result = inA + inB;
		SetPinData("Result", result);
		return Exit();
	}

	return 0;
}
#pragma endregion

#pragma region Subtract
void SGNode_Vec3Subtract::Init()
{
	SetTitle("Subtract Vector3");
	SetCategory("Vector Math");

	CreateDataPin<Vector3f>("A", PinDirection::Input);
	CreateDataPin<Vector3f>("B", PinDirection::Input);

	CreateDataPin<Vector3f>("Result", PinDirection::Output);
}

size_t SGNode_Vec3Subtract::DoOperation()
{
	Vector3f inA;
	Vector3f inB;

	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		const Vector3f result = inA - inB;
		SetPinData("Result", result);
		return Exit();
	}

	return 0;
}
#pragma endregion

#pragma region Multiply
void SGNode_Vec3Multiply::Init()
{
	SetTitle("Multiply Vector3");
	SetCategory("Vector Math");

	CreateDataPin<Vector3f>("A", PinDirection::Input);
	CreateDataPin<Vector3f>("B", PinDirection::Input);

	CreateDataPin<Vector3f>("Result", PinDirection::Output);
}

size_t SGNode_Vec3Multiply::DoOperation()
{
	Vector3f inA;
	Vector3f inB;

	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		const Vector3f result = inA * inB;
		SetPinData("Result", result);
		return Exit();
	}

	return 0;
}
#pragma endregion

#pragma region Scalar Multiply
void SGNode_Vec3ScalarMultiply::Init()
{
	SetTitle("Multiply Vector3 with Scalar");
	SetCategory("Vector Math");

	CreateDataPin<Vector3f>("Vector", PinDirection::Input);
	CreateDataPin<float>("Scalar", PinDirection::Input);

	CreateDataPin<Vector3f>("Result", PinDirection::Output);
}

size_t SGNode_Vec3ScalarMultiply::DoOperation()
{
	Vector3f inA;
	float inB;

	if (GetPinData("Vector", inA) && GetPinData("Scalar", inB))
	{
		const Vector3f result = inA * inB;
		SetPinData("Result", result);
		return Exit();
	}

	return 0;
}
#pragma endregion

#pragma region Divide
void SGNode_Vec3Divide::Init()
{
	SetTitle("Divide Vector3");
	SetCategory("Vector Math");

	CreateDataPin<Vector3f>("A", PinDirection::Input);
	CreateDataPin<Vector3f>("B", PinDirection::Input);

	CreateDataPin<Vector3f>("Result", PinDirection::Output);
}

size_t SGNode_Vec3Divide::DoOperation()
{
	Vector3f inA;
	Vector3f inB;

	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		const Vector3f result = inA / inB;
		SetPinData("Result", result);
		return Exit();
	}

	return 0;
}
#pragma endregion

#pragma region LerpVec3
void SGNode_MathLerpVec3::Init()
{
	SetTitle("Lerp Vector3f");
	SetCategory("Vector Math");
	SetDescription("Transitions from one vector to another over time.");

	CreateDataPin<Vector3f>("Start", PinDirection::Input);
	CreateDataPin<Vector3f>("End", PinDirection::Input);
	CreateDataPin<float>("Alpha", PinDirection::Input);
	CreateDataPin<Vector3f>("Output", PinDirection::Output);

}

size_t SGNode_MathLerpVec3::DoOperation()
{
	float alpha;
	Vector3f start, end;
	GetPinData("Alpha", alpha);
	GetPinData("Start", start);
	GetPinData("End", end);

	Vector3f out = Catbox::Lerp(start, end, alpha);
	SetPinData("Output", out);

	return 0;
}
#pragma endregion
