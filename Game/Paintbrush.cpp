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

	auto mousePos = Input::GetMousePosition();
	auto screenPos = Engine::GetInstance()->ViewportToScreenPos(mousePos.x, mousePos.y);
	myTransform->SetWorldPos(Engine::GetInstance()->GetActiveCamera()->MouseToWorldPos(screenPos, 0.3f));

	const float density = 0.3f;
	const float radius = 30;

	if (Input::GetKeyPress(KeyCode::MOUSELEFT))
	{
		myRemainingPaint = 100;
	}


	if (Input::GetKeyHeld(KeyCode::MOUSELEFT) && myRemainingPaint > 0)
	{
		auto mouseDelta = Input::GetMouseDelta();
		int length = mouseDelta.Length();
		int dots = 0;
		myPaintTimer += length / 500.f;

		if (myPaintTimer > 1)
		{
			float diff = myPaintTimer - std::floor(myPaintTimer);
			myPaintTimer = diff;
		}
		
		const float sensitivity = 10;
		const float speedScale = 0.5f;
		float speedBonus = Catbox::Clamp(length / sensitivity, 0.f, 1.f);
		float speedMultiplier = speedBonus * speedScale;
		float remainingPaintMultiplier = 1;
		if (myRemainingPaint < 30)
		{
			remainingPaintMultiplier = myRemainingPaint / 33.f;
		}

		myRemainingPaint -= length * speedMultiplier * 0.1f;

		Color col = myGradient.Evaluate(myPaintTimer);
		col = Catbox::Lerp(Color::Black(), col, remainingPaintMultiplier);
		Canvas::GetInstance()->Paint(screenPos.x, screenPos.y, radius * speedMultiplier * remainingPaintMultiplier, col);

		//for (float i = 0; i < length;)
		//{
		//	//int radius *= Catbox::GetRandom(0.75f, 1.25f);
		//	float percent = i / static_cast<float>(length);
		//	Canvas::GetInstance()->Paint(screenPos.x - (percent * mouseDelta.x), screenPos.y - (percent * mouseDelta.y), radius, col);
		//	i += radius * density;
		//	++dots;
		//}
		//if (length > 30)
		//{
		//	print("Delta was: " + std::to_string(length) + ". Painted " + std::to_string(dots) + " dots.");
		//}
	}


	if (Input::GetKeyReleased(KeyCode::CTRL))
	{
		Canvas::GetInstance()->Clear();
	}
}
