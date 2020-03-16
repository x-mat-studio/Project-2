#include "Entity.h"
#include "App.h"

Entity::Entity()
{}


Entity::Entity(SDL_Point position, ENTITY_TYPE type) :

	position(position),
	type(type),	
	started(false),
	toDelete(false),
	vectorPosition(NULL)
{}


Entity::Entity(SDL_Point position, ENTITY_TYPE type, SDL_Rect collRect, COLLIDER_TYPE collType, Module* callback) :

	position(position),
	type(type),	
	started(false),
	toDelete(false)
{
	collider = new Collider(collRect, collType, callback, this);
	

}

Entity::~Entity()
{
	texture = nullptr;
	collider->to_delete = true;
	collider = nullptr;
}


bool Entity::Start(SDL_Texture* texture)
{
	this->texture = texture;

	app->coll->AddColliderEntity(collider);

	started = true;

	return true;
}


bool Entity::PreUpdate(float dt)
{
	return true;
}


bool Entity::Update(float dt)
{
	return true;
}


bool Entity::PostUpdate(float dt)
{
	return true;
}


void Entity::OnCollision(Collider* collider)
{
}


Collider* Entity::GetCollider() const
{
	return collider;
}


void Entity::Draw(float dt)
{
}


SDL_Point Entity::GetPosition()
{
	return position;
}


ENTITY_TYPE Entity::GetType()
{
	return type;
}

void Entity::SetToDelete(bool toDel)
{
	if (toDel != toDelete)
	{
		toDelete = !toDelete;
	}
}