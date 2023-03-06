#include "stdafx.h"
#include "SGNode_MathOps.h"


#pragma region Add
void SGNode_MathAdd::Init()
{
	SetTitle("Add");
	SetCategory("Math");

	CreateDataPin<float>("A", PinDirection::Input);
	CreateDataPin<float>("B", PinDirection::Input);

	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_MathAdd::DoOperation()
{
	float inA = 0;
	float inB = 0;

	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		const float result = inA + inB;
		SetPinData("Result", result);
		return Exit();
	}

	return 0;
}
#pragma endregion

#pragma region Subtract
void SGNode_MathSubtract::Init()
{
	SetTitle("Subtract");
	SetCategory("Math");

	CreateDataPin<float>("A", PinDirection::Input);
	CreateDataPin<float>("B", PinDirection::Input);

	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_MathSubtract::DoOperation()
{
	float inA = 0;
	float inB = 0;

	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		const float result = inA - inB;
		SetPinData("Result", result);
		return Exit();
	}

	return 0;
}
#pragma endregion

#pragma region Multiply
void SGNode_MathMultiply::Init()
{
	SetTitle("Multiply");
	SetCategory("Math");

	CreateDataPin<float>("A", PinDirection::Input);
	CreateDataPin<float>("B", PinDirection::Input);

	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_MathMultiply::DoOperation()
{
	float inA = 0;
	float inB = 0;

	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		const float result = inA * inB;
		SetPinData("Result", result);
		return Exit();
	}

	return 0;
}
#pragma endregion

#pragma region Divide
void SGNode_MathDivide::Init()
{
	SetTitle("Divide");
	SetCategory("Math");

	CreateDataPin<float>("A", PinDirection::Input);
	CreateDataPin<float>("B", PinDirection::Input);

	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_MathDivide::DoOperation()
{
	float inA = 0;
	float inB = 0;

	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		const float result = inA / inB;
		SetPinData("Result", result);
		return Exit();
	}

	return 0;
}
#pragma endregion

#pragma region Remainder
void SGNode_MathRemainder::Init()
{
	SetTitle("Remainder");
	SetCategory("Math");
	SetDescription("Performs a division and returns the remainder. Ex: 10 % 3 = 1");

	CreateDataPin<int>("A", PinDirection::Input);
	CreateDataPin<int>("B", PinDirection::Input);

	CreateDataPin<int>("Result", PinDirection::Output);
}

size_t SGNode_MathRemainder::DoOperation()
{
	int inA = 0;
	int inB = 0;

	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		const int result = inA % inB;
		SetPinData("Result", result);
		return Exit();
	}

	return 0;
}
#pragma endregion

#pragma region Cos
void SGNode_MathCos::Init()
{
	SetTitle("Cos");
	SetCategory("Math");
	SetDescription("Does a std::cos on value");

	CreateDataPin<float>("A", PinDirection::Input);

	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_MathCos::DoOperation()
{
	int inA = 0;

	if (GetPinData("A", inA))
	{
		const float result = std::cos(inA);
		SetPinData("Result", result);
		return Exit();
	}

	return 0;
}
#pragma endregion

#pragma region Sin
void SGNode_MathSin::Init()
{
	SetTitle("Sin");
	SetCategory("Math");
	SetDescription("Does a std::sin on value");

	CreateDataPin<float>("A", PinDirection::Input);

	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_MathSin::DoOperation()
{
	float inA = 0;

	if (GetPinData("A", inA))
	{
		const float result = std::sin(inA);
		SetPinData("Result", result);
		return Exit();
	}

	return 0;
}
#pragma endregion

#pragma region Abs
void SGNode_MathAbs::Init()
{
	SetTitle("Abs");
	SetCategory("Math");
	SetDescription("Does a std::Abs on value");

	CreateDataPin<float>("A", PinDirection::Input);

	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_MathAbs::DoOperation()
{
	float inA = 0;

	if (GetPinData("A", inA))
	{
		const float result = std::abs(inA);
		SetPinData("Result", result);
		return Exit();
	}

	return 0;
}
#pragma endregion

#pragma region Atan2
void SGNode_MathAtan2::Init()
{
	SetTitle("Atan2");
	SetCategory("Math");
	SetDescription("Does a std::atan2 on A and B");

	CreateDataPin<float>("A", PinDirection::Input);
	CreateDataPin<float>("B", PinDirection::Input);

	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_MathAtan2::DoOperation()
{
	float inA = 0;
	float inB = 0;

	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		const float result = std::atan2(inA, inB);
		SetPinData("Result", result);
		return Exit();
	}

	return 0;
}
#pragma endregion

#pragma region Max
void SGNode_MathMax::Init()
{
	SetTitle("Max");
	SetCategory("Math");
	SetDescription("Outputs the highest value between A and B");

	CreateDataPin<float>("A", PinDirection::Input);
	CreateDataPin<float>("B", PinDirection::Input);

	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_MathMax::DoOperation()
{
	float inA = 0;
	float inB = 0;

	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		float result = inB;

		if (inA > inB) 
		{
			result = inA;
		}
		SetPinData("Result", result);
		return Exit();
	}

	return 0;
}
#pragma endregion
#pragma region Min
void SGNode_MathMin::Init()
{
	SetTitle("Min");
	SetCategory("Math");
	SetDescription("Outputs the lowest value between A and B");

	CreateDataPin<float>("Result", PinDirection::Output);
	CreateDataPin<float>("A", PinDirection::Input);
	CreateDataPin<float>("B", PinDirection::Input);

}

size_t SGNode_MathMin::DoOperation()
{
	float inA = 0;
	float inB = 0;

	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		float result = inB;

		if (inA < inB)
		{
			result = inA;
		}
		SetPinData("Result", result);
		return Exit();
	}

	return 0;
}
#pragma endregion
#pragma region Distance
void SGNode_MathDistance2D::Init()
{
	SetTitle("Distance 2D");
	SetCategory("Math");
	SetDescription("Outputs the distance between two vectors split up on four values");

	CreateDataPin<float>("X1", PinDirection::Input);
	CreateDataPin<float>("Y1", PinDirection::Input);
	CreateDataPin<float>("X2", PinDirection::Input);
	CreateDataPin<float>("Y2", PinDirection::Input);

	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_MathDistance2D::DoOperation()
{
	float inX1 = 0;
	float inY1 = 0;
	float inX2 = 0;
	float inY2 = 0;

	if (GetPinData("X1", inX1) && GetPinData("Y1", inY1) && GetPinData("X2", inX2) && GetPinData("Y2", inY2))
	{
		const float result = (Vector2f(inX1, inY1) - Vector2f(inX2, inY2)).Length();
		SetPinData("Result", result);
		return Exit();
	}

	return 0;
}
#pragma endregion
