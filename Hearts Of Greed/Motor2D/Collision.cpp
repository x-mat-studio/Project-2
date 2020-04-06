#include "App.h"
#include "Render.h"
#include "Input.h"
#include "Collision.h"
#include "Brofiler/Brofiler/Brofiler.h"

ModuleCollision::ModuleCollision()
{
	name.create("colliders");

	matrix[COLLIDER_HERO][COLLIDER_WALL] = true;
	matrix[COLLIDER_HERO][COLLIDER_HERO] = true;
	matrix[COLLIDER_HERO][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_HERO][COLLIDER_BASE_ALERT] = false;
	matrix[COLLIDER_HERO][COLLIDER_RECLUIT_IA] = false;
	matrix[COLLIDER_HERO][COLLIDER_VISIBILITY] = false;

	matrix[COLLIDER_WALL][COLLIDER_WALL] = false;
	matrix[COLLIDER_WALL][COLLIDER_HERO] = true;
	matrix[COLLIDER_WALL][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_WALL][COLLIDER_BASE_ALERT] = false;
	matrix[COLLIDER_WALL][COLLIDER_RECLUIT_IA] = false;
	matrix[COLLIDER_WALL][COLLIDER_VISIBILITY] = false;

	matrix[COLLIDER_ENEMY][COLLIDER_WALL] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_HERO] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_BASE_ALERT] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_RECLUIT_IA] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_VISIBILITY] = false;

	matrix[COLLIDER_BASE_ALERT][COLLIDER_WALL] = false;
	matrix[COLLIDER_BASE_ALERT][COLLIDER_HERO] = true;
	matrix[COLLIDER_BASE_ALERT][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_BASE_ALERT][COLLIDER_BASE_ALERT] = false;
	matrix[COLLIDER_BASE_ALERT][COLLIDER_RECLUIT_IA] = false;
	matrix[COLLIDER_BASE_ALERT][COLLIDER_VISIBILITY] = false;

	matrix[COLLIDER_RECLUIT_IA][COLLIDER_WALL] = false;
	matrix[COLLIDER_RECLUIT_IA][COLLIDER_HERO] = false;
	matrix[COLLIDER_RECLUIT_IA][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_RECLUIT_IA][COLLIDER_BASE_ALERT] = false;
	matrix[COLLIDER_RECLUIT_IA][COLLIDER_RECLUIT_IA] = false;
	matrix[COLLIDER_RECLUIT_IA][COLLIDER_VISIBILITY] = false;

	matrix[COLLIDER_VISIBILITY][COLLIDER_WALL] = false;
	matrix[COLLIDER_VISIBILITY][COLLIDER_HERO] = true;
	matrix[COLLIDER_VISIBILITY][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_VISIBILITY][COLLIDER_BASE_ALERT] = false;
	matrix[COLLIDER_VISIBILITY][COLLIDER_RECLUIT_IA] = false;
	matrix[COLLIDER_VISIBILITY][COLLIDER_VISIBILITY] = false;

}


// Destructor
ModuleCollision::~ModuleCollision()
{}


bool ModuleCollision::PreUpdate(float dt)
{
	BROFILER_CATEGORY("Collision Pre-Update", Profiler::Color::Gold);

	// Remove all colliders scheduled for deletion
	for (uint i = 0; i < colliders.size(); ++i)
	{
		if (colliders[i]->to_delete == true)
		{
			delete colliders[i];
			colliders[i] = nullptr;
			colliders.erase(colliders.begin() + i);
		}
	}

	return true;
}


// Called before render is available
bool ModuleCollision::Update(float dt)
{
	// Calculate collisions
	Collider* c1;
	Collider* c2;

	for (uint i = 0; i < colliders.size(); ++i)
	{
		c1 = colliders[i];

		// avoid checking collisions already checked
		for (uint k = i + 1; k < colliders.size(); ++k)
		{

			c2 = colliders[k];

			if (c1->CheckCollision(c2->rect) == true)
			{
				if (matrix[c1->type][c2->type] && c1->callback)
					c1->callback->OnCollision(c1, c2);

				if (matrix[c2->type][c1->type] && c2->callback)
					c2->callback->OnCollision(c2, c1);
			}
		}
	}
	return true;
}


bool ModuleCollision::PostUpdate(float dt)
{
	DebugDraw();
	return true;
}


void ModuleCollision::DebugDraw()
{
	if (app->debugMode == false)
		return;

	Uint8 alpha = 80;
	for (uint i = 0; i < colliders.size(); ++i)
	{
		switch (colliders[i]->type)
		{
		case COLLIDER_NONE: // white
			app->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
			break;

		case COLLIDER_WALL: // blue
			app->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
			break;

		case COLLIDER_HERO: // green
			app->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
			break;

		case COLLIDER_DEATH: // white
			app->render->DrawQuad(colliders[i]->rect, 0, 109, 109, alpha);
			break;

		case COLLIDER_VISIBILITY: // no clue
			app->render->DrawQuad(colliders[i]->rect, 30, 75, 240, alpha);
			break;

		case COLLIDER_ENEMY:
			app->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
			break;

		}
	}
}


// Called before quitting
bool ModuleCollision::CleanUp()
{
	LOG("Freeing all colliders");

	int numColl = colliders.size();

	for (int i = 0; i < numColl; i++)
	{
		RELEASE(colliders[i]);
		colliders[i] = nullptr;
	}

	colliders.clear();

	return true;
}


Collider* ModuleCollision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, Module* callback)
{
	Collider* ret = nullptr;

	ret = new Collider(rect, type, callback);
	colliders.push_back(ret);

	return ret;
}


void ModuleCollision::AddColliderEntity(Collider* collider)
{
	colliders.push_back(collider);
}


bool Collider::CheckCollision(const SDL_Rect& r) const
{
	return !((this->rect.x + this->rect.w < r.x || r.x + r.w < this->rect.x) || (this->rect.y + this->rect.h < r.y || r.y + r.h < this->rect.y));
}