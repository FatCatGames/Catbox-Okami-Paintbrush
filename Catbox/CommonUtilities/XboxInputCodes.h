#pragma once
#include <cstdint>

namespace CommonUtilities
{
	using InputCode = uint16_t;
	static constexpr InputCode InvalidInputCode = 0;

	struct Xbox
	{
		enum Button : InputCode
		{
			None = InvalidInputCode,
			Up,
			Down,
			Left,
			Right,
			Start,
			Back,
			LeftStick,
			RightStick,
			LeftStickUp,
			LeftStickDown,
			LeftStickRight,
			LeftStickLeft,
			RightStickUp,
			RightStickDown,
			RightStickRight,
			RightStickLeft,
			LeftBumper,
			RightBumper,
			LeftTrigger,
			RightTrigger,
			A,
			B,
			X,
			Y,
			ButtonsCount
		};
	};
}