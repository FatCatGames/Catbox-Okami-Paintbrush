#pragma once
#include "XboxInputCodes.h"
#include "Vector.h"
#include <bitset>

struct _XINPUT_STATE;
namespace CommonUtilities
{
	struct XboxPlayer
	{
		enum ID : uint8_t
		{
			One,
			Two,
			Three,
			Four,
			Count,
			Invalid
		};
	};

	//todo: Vibrations
	class XboxControllerInput
	{
		public:
			XboxControllerInput() = delete;
			explicit XboxControllerInput(XboxPlayer::ID aPlayerID);
			~XboxControllerInput() = default;

			void Refresh();

			bool IsPressed(Xbox::Button aXboxButton) const;
			bool WasPressedThisFrame(Xbox::Button aXboxButton) const;
			bool WasReleasedThisFrame(Xbox::Button aXboxButton) const;

			Vector2f GetLeftStickAxis() const;
			Vector2f GetRightStickAxis() const;

			float GetLeftTriggerAxis() const;
			float GetRightTriggerAxis() const;

			bool IsConnected() const;

			void SetPlayerID(XboxPlayer::ID aPlayerID);
			CommonUtilities::XboxPlayer::ID GetPlayerID(XboxPlayer::ID aPlayerID);

			std::bitset<CommonUtilities::Xbox::ButtonsCount> XInputToCUButtons(WORD aXInputButtons);

		private:
			struct XboxControllerState
			{
				std::bitset<Xbox::ButtonsCount> ButtonsState {};
				Vector2f LeftStick { 0.0f, 0.0f };
				Vector2f RightStick { 0.0f, 0.0f };
				uint32_t PacketNumber { 0 };
				float LeftTrigger { 0 };
				float RightTrigger { 0 };
				bool IsConnected { false };
			};

			void UpdateControllerState(const _XINPUT_STATE& aState);
			void ResetState();
			Vector2f GetStickValueFromXInput(int16_t aStickX, int16_t aStickY, uint16_t aDeadZone) const;
			float GetTriggerValueFromXInput(uint8_t aTriggerValue, uint8_t aTriggerThreshold) const;

			XboxControllerState myPreviousXboxControllerState {};
			XboxControllerState myCurrentXboxControllerState {};
			XboxPlayer::ID myPlayerID { XboxPlayer::Invalid };
	};
}
