#include "stdafx.h"
#include "InputHandler.h"
#include <windowsx.h>

#pragma comment(lib,"XInput.lib")
#pragma comment(lib,"Xinput9_1_0.lib")

#define XINPUT_GAMEPAD_THUMB_STICK_MAX_MAGNITUDE 32767
#define XINPUT_GAMEPAD_TRIGGER_MAX_VALUE 255

std::bitset<256> Input::myTentativeState;
std::bitset<256> Input::myCurrentState;
std::bitset<256> Input::myPreviousState;
std::unordered_map<std::string, KeyCode> Input::myControllerScheme;
std::unordered_map <KeyCode, std::string> Input::myKeyCodeToName =
{
	{KeyCode::MOUSELEFT, "Mouse Left" },
	{KeyCode::MOUSERIGHT, "Mouse Right"},
	{KeyCode::CANCEL, "Cancel"},
	{KeyCode::MOUSEMIDDLE, "Mouse Middle"},
	{KeyCode::MOUSEXBUTTON1, "Mouse x button 1"},
	{KeyCode::MOUSEXBUTTON2, "Mouse x button 2"},
	{KeyCode::BACK, "Back"},
	{KeyCode::TAB, "Tab"},
	{KeyCode::CLEAR, "Clear"},
	{KeyCode::RETURN, "Return"},
	{KeyCode::SHIFT, "Shift"},
	{KeyCode::CTRL, "Ctrl"},
	{KeyCode::ALT, "Alt"},
	{KeyCode::PAUSE, "Pause"},
	{KeyCode::CAPITAL, "Capslock"},
	{KeyCode::KANA , "Kana"},
	{KeyCode::HANGEUL , "Hangeul"},
	{KeyCode::HANGUL , "Hangul"},
	{KeyCode::JUNJA , "Junja"},
	{KeyCode::FINAL , "Final"},
	{KeyCode::HANJA , "Hanja"},
	{KeyCode::KANJI , "Kanji"},
	{KeyCode::ESCAPE , "Escape"},
	{KeyCode::CONVERT , "Convert"},
	{KeyCode::NONCONVERT , "Non Convert"},
	{KeyCode::ACCEPT , "Accept"},
	{KeyCode::MODECHANGE , "Mode Change"},
	{KeyCode::SPACE , "Space"},
	{KeyCode::PRIOR , "Prior"},
	{KeyCode::NEXT , "Next"},
	{KeyCode::END , "End"},
	{KeyCode::HOME , "Home"},
	{KeyCode::LEFT , "Left"},
	{KeyCode::UP , "Up"},
	{KeyCode::RIGHT , "Right"},
	{KeyCode::DOWN , "Down"},
	{KeyCode::SELECT , "Select"},
	{KeyCode::PRINT , "Print"},
	{KeyCode::EXECUTE , "Execute"},
	{KeyCode::SNAPSHOT , "Snapshot"},
	{KeyCode::INSERT , "Insert"},
	{KeyCode::DELETE_BUTTON , "Delete"},
	{KeyCode::HELP , "Help"},

	{KeyCode::N0 , "0"},
	{KeyCode::N1 , "1"},
	{KeyCode::N2 , "2"},
	{KeyCode::N3 , "3"},
	{KeyCode::N4 , "4"},
	{KeyCode::N5 , "5"},
	{KeyCode::N6 , "6"},
	{KeyCode::N7 , "7"},
	{KeyCode::N8 , "8"},
	{KeyCode::N9 , "9"},

	{KeyCode::A , "A"},
	{KeyCode::B , "B"},
	{KeyCode::C , "C"},
	{KeyCode::D , "D"},
	{KeyCode::E , "E"},
	{KeyCode::F , "F"},
	{KeyCode::G , "G"},
	{KeyCode::H , "H"},
	{KeyCode::I , "I"},
	{KeyCode::J , "J"},
	{KeyCode::K , "K"},
	{KeyCode::L , "L"},
	{KeyCode::M , "M"},
	{KeyCode::N , "N"},
	{KeyCode::O , "O"},
	{KeyCode::P , "P"},
	{KeyCode::Q , "Q"},
	{KeyCode::R , "R"},
	{KeyCode::S , "S"},
	{KeyCode::T , "T"},
	{KeyCode::U , "U"},
	{KeyCode::V , "V"},
	{KeyCode::W , "W"},
	{KeyCode::X , "X"},
	{KeyCode::Y , "Y"},
	{KeyCode::Z , "Z"},
	{KeyCode::LWIN , "Left Windows Key"},
	{KeyCode::RWIN , "Right Windows Key"},
	{KeyCode::RWIN , "Right Windows Key"},
	{KeyCode::APPS , "Application Key"},
	{KeyCode::SLEEP , "Sleep Key"},

	{KeyCode::NUMPAD0 , "Numpad 0"},
	{KeyCode::NUMPAD1 , "Numpad 1"},
	{KeyCode::NUMPAD2 , "Numpad 2"},
	{KeyCode::NUMPAD3 , "Numpad 3"},
	{KeyCode::NUMPAD4 , "Numpad 4"},
	{KeyCode::NUMPAD5 , "Numpad 5"},
	{KeyCode::NUMPAD6 , "Numpad 6"},
	{KeyCode::NUMPAD7 , "Numpad 7"},
	{KeyCode::NUMPAD8 , "Numpad 8"},
	{KeyCode::NUMPAD9 , "Numpad 9"},

	{KeyCode::MULTIPLY , "Multiply"},
	{KeyCode::ADD , "Add" },
	{KeyCode::SEPARATOR , "Seperator"},
	{KeyCode::SUBTRACT , "Subtract"},
	{KeyCode::DIVIDE , "Divide"},


	{KeyCode::F1 , "F1"},
	{KeyCode::F2 , "F2"},
	{KeyCode::F3 , "F3"},
	{KeyCode::F4 , "F4"},
	{KeyCode::F5 , "F5"},
	{KeyCode::F6 , "F6"},
	{KeyCode::F7 , "F7"},
	{KeyCode::F8 , "F8"},
	{KeyCode::F9 , "F9"},
	{KeyCode::F10 , "F10"},
	{KeyCode::F12 , "F12"},
	{KeyCode::F13 , "F13"},
	{KeyCode::F14 , "F14"},
	{KeyCode::F15 , "F15"},
	{KeyCode::F16 , "F16"},
	{KeyCode::F17 , "F17"},
	{KeyCode::F18 , "F18"},
	{KeyCode::F19 , "F19"},
	{KeyCode::F20 , "F20"},
	{KeyCode::F21 , "F21"},
	{KeyCode::F22 , "F22"},
	{KeyCode::F23 , "F23"},
	{KeyCode::F24 , "F24"},
	{KeyCode::NUMLOCK , "Num lock"},
	{KeyCode::SCROLL , "Scroll lock"},
	{KeyCode::OEM_NEC_EQUAL , "Numpad Equal"},
	{KeyCode::LCONTROL , "Left ctrl"},
	{KeyCode::RCONTROL , "Right ctrl"},
	/*LSHIFT = 0xA0, these dont seem to work
	RSHIFT = 0xA1,*/
	{KeyCode::LMENU , "Left Menu"},
	{KeyCode::RALT , "Right Alt"},
};

Vector2i Input::myCurrentMousePos;
Vector2i Input::myPreviousMousePos;
float Input::myScroll = 0;
float Input::myPreviousScroll = 0;

void Input::Init(HWND aWindowHandle)
{
	myOwnerHWND = aWindowHandle;

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = myOwnerHWND;
	RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
}

void Input::SetMousePosition(POINT& aPoint)
{
	SetCursorPos(aPoint.x, aPoint.y);
}

void Input::ShowMouse()
{
	ShowCursor(true);
}

void Input::HideMouse()
{
	ShowCursor(false);
}

bool Input::GetKeyPress(const KeyCode aKeyCode)
{
	int keycode = static_cast<int>(aKeyCode);
	return myCurrentState[keycode] && !myPreviousState[keycode];
}

bool Input::GetKeyHeld(const KeyCode aKeyCode)
{
	int keycode = static_cast<int>(aKeyCode);
	return (myPreviousState[keycode] && myCurrentState[keycode]);
}

bool Input::GetKeyReleased(const KeyCode aKeyCode)
{
	int keycode = static_cast<int>(aKeyCode);

	if (myPreviousState.test(keycode) == true && myCurrentState.test(keycode) == false)
	{
		return true;
	}
	return false;
	//return !myCurrentState[aKeyCode] && myPreviousState[aKeyCode];
}

void Input::RemoveHotKey(const std::string aHotKey)
{
	if (myControllerScheme.count(aHotKey.c_str()))
	{
		myControllerScheme.erase(aHotKey);
	}
}

bool Input::GetKeyPress(const std::string& aHotKey)
{
	if (myControllerScheme.count(aHotKey.c_str()))
	{
		int keycode = static_cast<int>(myControllerScheme.find(aHotKey.c_str())->second);
		return myCurrentState[keycode] && !myPreviousState[keycode];
	}
	return false;
}

bool Input::GetKeyHeld(const std::string& aHotKey)
{
	if (myControllerScheme.count(aHotKey.c_str()))
	{
		int keycode = static_cast<int>(myControllerScheme.find(aHotKey.c_str())->second);
		return (myPreviousState[keycode] && myCurrentState[keycode]);
	}
	return false;
}

bool Input::GetKeyReleased(const std::string& aHotKey)
{
	if (myControllerScheme.count(aHotKey.c_str()))
	{
		int keycode = static_cast<int>(myControllerScheme.find(aHotKey.c_str())->second);
		if (myPreviousState.test(keycode) == true && myCurrentState.test(keycode) == false)
		{
			return true;
		}
	}
	return false;
	//return !myCurrentState[aKeyCode] && myPreviousState[aKeyCode];
}

void Input::AddKeyBind(std::string aName, KeyCode aKeyCode)
{
	if (myControllerScheme.count(aName) <= 0)
	{
		myControllerScheme.insert({ aName, aKeyCode });
	}
	else
	{
		myControllerScheme.erase(aName);
		myControllerScheme.insert({ aName, aKeyCode });
	}
}

KeyCode Input::GetCurrentKeyPressed()
{
	for (int i = 0; i != static_cast<int>(KeyCode::COUNT); i++)
	{
		if (myCurrentState[i])
		{
			return static_cast<KeyCode>(i);
		}
	}
	return KeyCode::COUNT;
}

Vector2i Input::GetMousePosition()
{
	return myCurrentMousePos;
}

Vector2i Input::GetMouseDelta()
{
	return { myCurrentMousePos.x - myPreviousMousePos.x, myCurrentMousePos.y - myPreviousMousePos.y };
}
Vector2i Input::GetRawMouseDelta()
{
	return { (int)myMouseDelta.x,(int)myMouseDelta.y };
}
float Input::GetMouseScroll() {
	return myPreviousScroll;
}

bool Input::UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		myTentativeState[wParam] = true;
		return true;
	case WM_KEYUP:
		myTentativeState[wParam] = false;
		return true;
	case WM_LBUTTONDOWN:
		myTentativeState.set(0x01);
		return true;
	case WM_LBUTTONUP:
		myTentativeState.reset(0x01);
		return true;
	case WM_RBUTTONDOWN:
		myTentativeState.set(0x02);
		return true;
	case WM_RBUTTONUP:
		myTentativeState.reset(0x02);
		return true;
	case WM_MBUTTONDOWN:
		myTentativeState.set(0x04);
		return true;
	case WM_MBUTTONUP:
		myTentativeState.reset(0x04);
		return true;
	case WM_XBUTTONDOWN:
		wParam < 0x00020000 ? myTentativeState.set(0x06) : myTentativeState.set(0x05);
		return true;
	case WM_XBUTTONUP:
		wParam < 0x00020000 ? myTentativeState.reset(0x06) : myTentativeState.reset(0x05);
		return true;
	case WM_MOUSEWHEEL:
		myScroll = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
		return true;
	case WM_MOUSEMOVE:
	{
		const int xPos = 0.5f;// GET_X_LPARAM(lParam);
		const int yPos = 0.5f;//GET_Y_LPARAM(lParam);

		myTentativeMousePosition.x = xPos;
		myTentativeMousePosition.y = yPos;

		return true;
	}

	// Handles mouse delta, used in 3D navigation etc.
	case WM_INPUT:
	{
		UINT dwSize = sizeof(RAWINPUT);
		static BYTE lpb[sizeof(RAWINPUT)];

		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

		RAWINPUT* raw = (RAWINPUT*)lpb;

		if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			myTentativeMouseDelta.x = static_cast<float>(raw->data.mouse.lLastX);
			myTentativeMouseDelta.y = static_cast<float>(raw->data.mouse.lLastY);
		}
		return true;
	}
	default:
		break;
	}
	return false;
}

void Input::Update()
{
	myPreviousScroll = myScroll;
	myScroll = 0;
	//update keys
	myPreviousState = myCurrentState;
	myCurrentState = myTentativeState;

	//update mouse
	myMouseDelta = myTentativeMouseDelta;
	myTentativeMouseDelta = { 0, 0 };
	myPreviousMousePos = myCurrentMousePos;
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(GetActiveWindow(), &point);
	myCurrentMousePos = Vector2i(point.x, point.y);

	myXboxPlayerOne.Refresh();
}
