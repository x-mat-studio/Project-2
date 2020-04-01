#include "UI_Button.h"

UI_Button::UI_Button(fMPoint positionValue, UI* father, UI_TYPE uiType, SDL_Rect rect, P2SString uiName, bool draggable) : UI(positionValue, father, uiType, rect, uiName, draggable)
{}

UI_Button::~UI_Button()
{}

bool UI_Button::Start()
{
	return true;
}

bool UI_Button::PreUpdate(float dt)
{
	return true;
}

bool UI_Button::Update(float dt)
{
	return true;
}

void UI_Button::HandleInput()
{
	
}