#include "App.h"
#include "UI_Scrollbar.h"
#include "UIManager.h"
#include "Audio.h"
#include "Input.h"

UI_Scrollbar::UI_Scrollbar(float x, float y, UI* parent, SDL_Rect rect, SDL_Texture* texture, float maxValue) :

	UI({ x, y }, parent, UI_TYPE::SCROLLBAR, rect, true, true, texture), maxValue(maxValue), previousX(x),

	clickCheck(true)
{
	if (parent == nullptr)
		assert("Must have a father");

	if (maxValue == 128.0f)
	{
		localPosition.x = ValueToPosition(app->audio->musicVolume);
	}
	else
	{
		localPosition.x = ValueToPosition(app->audio->volumeAdjustment + 255);
	}
}

UI_Scrollbar::~UI_Scrollbar()
{}


void UI_Scrollbar::HandleInput()
{
	if (previousX != localPosition.x)
	{
		currentValue = PositionToValue();
		if (maxValue == 128.0f)
		{
			app->audio->SetVolume(currentValue);
		}
		else
		{
			app->audio->volumeAdjustment = currentValue - 255;
		}

	}

	previousX = localPosition.x;
}


void UI_Scrollbar::Move() {

	if (localPosition.x > father->rect.w - rect.w / 2)
		localPosition.x = father->rect.w - rect.w / 2;

	else if (localPosition.x < 0)
		localPosition.x = 0;

	position.x = localPosition.x + father->GetPosition().x;
	position.y = father->GetPosition().y - 15;

	if (focused == true && app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_STATE::KEY_DOWN)
	{
		if (clickCheck == true)
		{
			app->uiManager->PlayClickSound();
			clickCheck = false;
		}
	}

	else
	{
		clickCheck = true;
	}
}

float UI_Scrollbar::PositionToValue() const
{
	if (localPosition.x < 1 || father->rect.w < 1 || rect.w < 1) { return 0; }
	else return (localPosition.x + int(rect.w * 0.5)) * maxValue / father->rect.w;
}


float UI_Scrollbar::ValueToPosition(float value) const
{
	if (value < 1 || maxValue == 0) { return 0; }
	else return (value * father->rect.w / maxValue) - int(rect.w * 0.5);
}

