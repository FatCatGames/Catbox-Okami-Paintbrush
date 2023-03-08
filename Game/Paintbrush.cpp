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
	if (Input::GetKeyPress(KeyCode::SPACE))
	{
		myIsGay = !myIsGay;
	}

	auto mousePos = Input::GetMousePosition();
	auto screenPos = Engine::GetInstance()->ViewportToScreenPos(mousePos.x, mousePos.y);
	myTransform->SetWorldPos(Engine::GetInstance()->GetActiveCamera()->MouseToWorldPos(screenPos, 0.3f));


	if (Input::GetKeyPress(KeyCode::MOUSELEFT))
	{
		myRemainingPaint = 100;
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

				col = myIsGay ? myGradient.Evaluate(myPaintTimer) : Color::Black();
			}

			float percent = i / static_cast<float>(length);
			float size = myRadius * speedMultiplier * remainingPaintMultiplier;
			//size = Catbox::Clamp(size, myMinSize, myMaxSize);
			auto pos = Engine::GetInstance()->ViewportToScreenPos(previousMousePos.x + mouseDelta.x * percent, previousMousePos.y + mouseDelta.y * percent);
			Canvas::GetInstance()->Paint(pos.x, pos.y, size, col);
			i += Catbox::Clamp(size * myDensity, myMinSize, myRadius);
		}
	}


	if (Input::GetKeyReleased(KeyCode::CTRL))
	{
		Canvas::GetInstance()->Clear();
	}
}
