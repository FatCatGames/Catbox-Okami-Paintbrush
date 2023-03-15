#include "Game.pch.h"
#include "Paintbrush.h"
#include "Canvas.h"

void Paintbrush::Awake()
{
	myGradient.AddColor(Color::Red(), 0);
	myGradient.AddColor(Color::Orange(), 0.125f);
	myGradient.AddColor(Color::Yellow(), 0.2f);
	myGradient.AddColor(Color::Green(), 0.3f);
	myGradient.AddColor(Color::Blue(), 0.5f);
	myGradient.AddColor(Color::Purple(), 0.8f);
	myGradient.AddColor(Color::Red(), 1);
}

void Paintbrush::Update()
{
	if (Input::GetKeyPress(KeyCode::N1))
	{
		myColorMode = ColorMode::Black;
	}
	else if (Input::GetKeyPress(KeyCode::N2))
	{
		myColorMode = ColorMode::Red;
	}
	else if (Input::GetKeyPress(KeyCode::N3))
	{
		myColorMode = ColorMode::Rainbow;
	}

	auto mousePos = Input::GetMousePosition();
	auto screenPos = Engine::GetInstance()->ViewportToScreenPos(mousePos.x, mousePos.y);


	myTransform->SetWorldPos(Engine::GetInstance()->GetActiveCamera()->MouseToWorldPos(mousePos, 0));

	if (Input::GetKeyPress(KeyCode::MOUSELEFT))
	{
		myRemainingPaint = 1000;
	}

	bool isPainting = Input::GetKeyHeld(KeyCode::MOUSELEFT);
	bool isErasing = Input::GetKeyHeld(KeyCode::MOUSERIGHT);

	if ((isPainting || isErasing) && myRemainingPaint > 0)
	{
		auto mouseDelta = Input::GetMouseDelta();

		int length = mouseDelta.Length();
		if (length > myMaxMouseDelta)
		{
			length = myMaxMouseDelta;
		}

		float speedBonus = Catbox::Clamp(length / mySensitivity, 0.f, 1.5f);
		float speedMultiplier = speedBonus * mySpeedScale;
		float remainingPaintMultiplier = 1;
		auto previousMousePos = Input::GetPreviousMousePosition();
		auto previousScreenPos = Engine::GetInstance()->ViewportToScreenPos(previousMousePos.x, previousMousePos.y);
		Vector2i convertedPreviousPos;
		convertedPreviousPos.x = (previousScreenPos.x / static_cast<float>(DX11::GetResolution().x)) * 1920;
		convertedPreviousPos.y = (previousScreenPos.y / static_cast<float>(DX11::GetResolution().y)) * 1080;

		Vector2i convertedCurrentPos;
		convertedCurrentPos.x = (screenPos.x / static_cast<float>(DX11::GetResolution().x)) * 1920;
		convertedCurrentPos.y = (screenPos.y / static_cast<float>(DX11::GetResolution().y)) * 1080;


		Vector2i convertedMouseDelta = convertedCurrentPos - convertedPreviousPos;
		//print("X: " + std::to_string(convertedCurrentPos.x) + ", Y: " + std::to_string(convertedCurrentPos.y));

		for (float i = 0; i <= length;)
		{
			Color col;
			 
			if (!isErasing)
			{
				myPaintTimer += myGradientSpeed;

				if (myPaintTimer > 1)
				{
					float diff = myPaintTimer - std::floor(myPaintTimer);
					myPaintTimer = diff;
				}

				myRemainingPaint -= myRadius * speedMultiplier * remainingPaintMultiplier * myPaintDecreaseSpeed;
				if (myRemainingPaint < 15)
				{
					remainingPaintMultiplier = myRemainingPaint / 15.f;
				}

				if (myColorMode == ColorMode::Black) col = Color::Black();
				else if (myColorMode == ColorMode::Red) col = Color::Red();
				else if (myColorMode == ColorMode::Rainbow) col = myGradient.Evaluate(myPaintTimer);
			}

			float percent = 0;
			if (length != 0)
			{
				percent = i / static_cast<float>(length);
			}

			int newX = std::round(convertedPreviousPos.x + convertedMouseDelta.x * percent);
			int newY = std::round(convertedPreviousPos.y + convertedMouseDelta.y * percent);

			float multiplier = speedMultiplier * remainingPaintMultiplier;
			//size = Catbox::Clamp(myRadius * multiplier, myMinSize, myMaxSize);
			Canvas::GetInstance()->Paint(newX, newY, myRadius, multiplier, col);
			i += myRadius;
		}
	}

	if (Input::GetKeyReleased(KeyCode::S))
	{
		Canvas::GetInstance()->Save();
	}

	if (Input::GetKeyReleased(KeyCode::C))
	{
		Canvas::GetInstance()->Clear();
	}


	//if (Input::GetKeyReleased(KeyCode::CTRL))
	//{
	//	Canvas::GetInstance()->Save();
	//	Canvas::GetInstance()->Clear();
	//}

	if (Input::GetKeyReleased(KeyCode::G))
	{
		Canvas::GetInstance()->Generate();
	}
}
