#include "stdafx.h"
#include "XboxControllerInput.h"
#include <Xinput.h>
#include <Windows.h>

#pragma comment(lib,"XInput.lib")
#pragma comment(lib,"Xinput9_1_0.lib")

#define XINPUT_GAMEPAD_THUMB_STICK_MAX_MAGNITUDE 32767
#define XINPUT_GAMEPAD_TRIGGER_MAX_VALUE 255

CommonUtilities::XboxControllerInput::XboxControllerInput(XboxPlayer::ID aPlayerID)
{
	SetPlayerID(aPlayerID);
}

//std::bitset<CommonUtilities::Xbox::ButtonsCount> XInputToCUButtons(WORD aXInputButtons)
//{
//	std::bitset<CommonUtilities::Xbox::ButtonsCount> state;
//	if(aXInputButtons & XINPUT_GAMEPAD_DPAD_UP)
//	{
//		state.set(CommonUtilities::Xbox::Up);
//	}
//
//	if(aXInputButtons & XINPUT_GAMEPAD_DPAD_DOWN)
//	{
//		state.set(CommonUtilities::Xbox::Down);
//	}
//
//	if(aXInputButtons & XINPUT_GAMEPAD_DPAD_LEFT)
//	{
//		state.set(CommonUtilities::Xbox::Left);
//	}
//
//	if(aXInputButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
//	{
//		state.set(CommonUtilities::Xbox::Right);
//	}
//
//	if(aXInputButtons & XINPUT_GAMEPAD_START)
//	{
//		state.set(CommonUtilities::Xbox::Start);
//	}
//
//	if(aXInputButtons & XINPUT_GAMEPAD_BACK)
//	{
//		state.set(CommonUtilities::Xbox::Back);
//	}
//
//	if(aXInputButtons & XINPUT_GAMEPAD_LEFT_THUMB)
//	{
//		state.set(CommonUtilities::Xbox::LeftStick);
//	}
//
//	if(aXInputButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
//	{
//		state.set(CommonUtilities::Xbox::RightStick);
//	}
//
//	if(aXInputButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
//	{
//		state.set(CommonUtilities::Xbox::LeftBumper);
//	}
//
//	if(aXInputButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
//	{
//		state.set(CommonUtilities::Xbox::RightBumper);
//	}
//
//	if(aXInputButtons & XINPUT_GAMEPAD_A)
//	{
//		state.set(CommonUtilities::Xbox::A);
//	}
//
//	if(aXInputButtons & XINPUT_GAMEPAD_B)
//	{
//		state.set(CommonUtilities::Xbox::B);
//	}
//
//	if(aXInputButtons & XINPUT_GAMEPAD_X)
//	{
//		state.set(CommonUtilities::Xbox::X);
//	}
//
//	if(aXInputButtons & XINPUT_GAMEPAD_Y)
//	{
//		state.set(CommonUtilities::Xbox::Y);
//	}
//
//	return state;
//}

void CommonUtilities::XboxControllerInput::UpdateControllerState(const _XINPUT_STATE& aState)
{
	myCurrentXboxControllerState.ButtonsState = XInputToCUButtons(aState.Gamepad.wButtons);

	myCurrentXboxControllerState.IsConnected = true;

	//Left Stick
	myCurrentXboxControllerState.LeftStick = GetStickValueFromXInput(aState.Gamepad.sThumbLX, aState.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

	if (myCurrentXboxControllerState.LeftStick.x > 0.01f)
	{
		myCurrentXboxControllerState.ButtonsState.set(Xbox::LeftStickRight);
	}

	if (myCurrentXboxControllerState.LeftStick.x < -0.01f)
	{
		myCurrentXboxControllerState.ButtonsState.set(Xbox::LeftStickLeft);
	}

	if (myCurrentXboxControllerState.LeftStick.y > 0.01f)
	{
		myCurrentXboxControllerState.ButtonsState.set(Xbox::LeftStickUp);
	}

	if (myCurrentXboxControllerState.LeftStick.y < -0.01f)
	{
		myCurrentXboxControllerState.ButtonsState.set(Xbox::LeftStickDown);
	}

	//Right stick
	myCurrentXboxControllerState.RightStick = GetStickValueFromXInput(aState.Gamepad.sThumbRX, aState.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

	if (myCurrentXboxControllerState.RightStick.x > 0.01f)
	{
		myCurrentXboxControllerState.ButtonsState.set(Xbox::RightStickRight);
	}

	if (myCurrentXboxControllerState.RightStick.x < -0.01f)
	{
		myCurrentXboxControllerState.ButtonsState.set(Xbox::RightStickLeft);
	}

	if (myCurrentXboxControllerState.RightStick.y > 0.01f)
	{
		myCurrentXboxControllerState.ButtonsState.set(Xbox::RightStickUp);
	}

	if (myCurrentXboxControllerState.RightStick.y < -0.01f)
	{
		myCurrentXboxControllerState.ButtonsState.set(Xbox::RightStickDown);
	}
	//Left Trigger
	myCurrentXboxControllerState.LeftTrigger = GetTriggerValueFromXInput(aState.Gamepad.bLeftTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

	if (aState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	{
		myCurrentXboxControllerState.ButtonsState.set(Xbox::LeftTrigger);
	}

	//Right Trigger
	myCurrentXboxControllerState.RightTrigger = GetTriggerValueFromXInput(aState.Gamepad.bRightTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

	if (aState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	{
		myCurrentXboxControllerState.ButtonsState.set(Xbox::RightTrigger);
	}
}

void CommonUtilities::XboxControllerInput::ResetState()
{
	myCurrentXboxControllerState = {};
}

Vector2f CommonUtilities::XboxControllerInput::GetStickValueFromXInput(int16_t aStickX, int16_t aStickY, uint16_t aDeadZone) const
{
	float sx = static_cast<float>(aStickX);
	float sy = static_cast<float>(aStickY);
	float magnitude = sqrt((sx * sx) + (sy * sy));

	float normalizedStickX = sx / magnitude;
	float normalizedStickY = sy / magnitude;

	if (magnitude > aDeadZone)
	{
		if (magnitude > XINPUT_GAMEPAD_THUMB_STICK_MAX_MAGNITUDE)
		{
			magnitude = XINPUT_GAMEPAD_THUMB_STICK_MAX_MAGNITUDE;
		}

		magnitude -= aDeadZone;

		float normalizedMagnitude = magnitude / (XINPUT_GAMEPAD_THUMB_STICK_MAX_MAGNITUDE - aDeadZone);

		return Vector2f { normalizedStickX, normalizedStickY } * normalizedMagnitude;
	}
	else
	{
		return Vector2f { 0.0f, 0.0f };
	}
}

float CommonUtilities::XboxControllerInput::GetTriggerValueFromXInput(uint8_t aTriggerValue, uint8_t aTriggerThreshold) const
{
	if(aTriggerValue < aTriggerThreshold)
	{
		return 0.0f;
	}

	return static_cast<float>(aTriggerValue - aTriggerThreshold) / (XINPUT_GAMEPAD_TRIGGER_MAX_VALUE - aTriggerThreshold);
}

void CommonUtilities::XboxControllerInput::Refresh()
{
	myPreviousXboxControllerState = myCurrentXboxControllerState;

	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	DWORD result = XInputGetState(myPlayerID, &state);

	if (result == ERROR_SUCCESS)
	{
		if(state.dwPacketNumber != myPreviousXboxControllerState.PacketNumber) //State has changed, update
		{
			UpdateControllerState(state);
		}
	}
	else
	{
		ResetState();
	}
}

bool CommonUtilities::XboxControllerInput::IsPressed(Xbox::Button aXboxButton) const
{
	return myCurrentXboxControllerState.ButtonsState[aXboxButton];
}

bool CommonUtilities::XboxControllerInput::WasPressedThisFrame(Xbox::Button aXboxButton) const
{
	return myCurrentXboxControllerState.ButtonsState[aXboxButton] && !myPreviousXboxControllerState.ButtonsState[aXboxButton];
}

bool CommonUtilities::XboxControllerInput::WasReleasedThisFrame(Xbox::Button aXboxButton) const
{
	return !myCurrentXboxControllerState.ButtonsState[aXboxButton] && myPreviousXboxControllerState.ButtonsState[aXboxButton];
}

Vector2f CommonUtilities::XboxControllerInput::GetLeftStickAxis() const
{
	return myCurrentXboxControllerState.LeftStick;
}

Vector2f CommonUtilities::XboxControllerInput::GetRightStickAxis() const
{
	return myCurrentXboxControllerState.RightStick;
}

float CommonUtilities::XboxControllerInput::GetLeftTriggerAxis() const
{
	return myCurrentXboxControllerState.LeftTrigger;
}

float CommonUtilities::XboxControllerInput::GetRightTriggerAxis() const
{
	return myCurrentXboxControllerState.RightTrigger;
}

bool CommonUtilities::XboxControllerInput::IsConnected() const
{
	return myCurrentXboxControllerState.IsConnected;
}

void CommonUtilities::XboxControllerInput::SetPlayerID(XboxPlayer::ID aPlayerID)
{
	assert(aPlayerID < XUSER_MAX_COUNT, "XInput does not support more than {0} players.", XUSER_MAX_COUNT);
	if(aPlayerID < XUSER_MAX_COUNT)
	{
		myPlayerID = aPlayerID;
	}
}

CommonUtilities::XboxPlayer::ID CommonUtilities::XboxControllerInput::GetPlayerID(XboxPlayer::ID aPlayerID)
{
	return myPlayerID;
}

std::bitset<CommonUtilities::Xbox::ButtonsCount> CommonUtilities::XboxControllerInput::XInputToCUButtons(WORD aXInputButtons)
{
	std::bitset<CommonUtilities::Xbox::ButtonsCount> state;
	if(aXInputButtons & XINPUT_GAMEPAD_DPAD_UP)
	{
		state.set(CommonUtilities::Xbox::Up);
	}

	if(aXInputButtons & XINPUT_GAMEPAD_DPAD_DOWN)
	{
		state.set(CommonUtilities::Xbox::Down);
	}

	if(aXInputButtons & XINPUT_GAMEPAD_DPAD_LEFT)
	{
		state.set(CommonUtilities::Xbox::Left);
	}

	if(aXInputButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
	{
		state.set(CommonUtilities::Xbox::Right);
	}

	if(aXInputButtons & XINPUT_GAMEPAD_START)
	{
		state.set(CommonUtilities::Xbox::Start);
	}

	if(aXInputButtons & XINPUT_GAMEPAD_BACK)
	{
		state.set(CommonUtilities::Xbox::Back);
	}

	if(aXInputButtons & XINPUT_GAMEPAD_LEFT_THUMB)
	{
		state.set(CommonUtilities::Xbox::LeftStick);
	}

	if(aXInputButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
	{
		state.set(CommonUtilities::Xbox::RightStick);
	}

	if(aXInputButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
	{
		state.set(CommonUtilities::Xbox::LeftBumper);
	}

	if(aXInputButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
	{
		state.set(CommonUtilities::Xbox::RightBumper);
	}

	if(aXInputButtons & XINPUT_GAMEPAD_A)
	{
		state.set(CommonUtilities::Xbox::A);
	}

	if(aXInputButtons & XINPUT_GAMEPAD_B)
	{
		state.set(CommonUtilities::Xbox::B);
	}

	if(aXInputButtons & XINPUT_GAMEPAD_X)
	{
		state.set(CommonUtilities::Xbox::X);
	}

	if(aXInputButtons & XINPUT_GAMEPAD_Y)
	{
		state.set(CommonUtilities::Xbox::Y);
	}

	return state;
}
