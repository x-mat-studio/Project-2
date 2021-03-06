#include "Particle.h"
#include "Textures.h"
#include "Render.h"
#include "Map.h"

Particle::Particle() :

	position{ NULL, NULL },
	speed{ NULL, NULL },
	acceleration{ NULL, NULL },
	angle(0),
	angularSpeed(0),

	life(0),
	originalLife(0),
	texture(nullptr),
	fade(false),
	active(false)

{}


Particle::Particle(fMPoint& position, fMPoint& speed, fMPoint& acceleration,
	float angle, float angularSpeed, float life, SDL_Texture* texture, Animation animation, bool fade) :

	position(position),
	speed(speed),
	acceleration(acceleration),
	angle(angle),
	angularSpeed(angularSpeed),

	life(life),
	originalLife(life),

	texture(texture),
	animation(animation),

	fade(fade),
	active(false)

{}


Particle::Particle(float life, SDL_Texture* texture, Animation animation, bool fade) :

	position{ NULL, NULL },
	speed{ NULL, NULL },
	acceleration{ NULL, NULL },
	angle(0),
	angularSpeed(0),

	life(life),
	originalLife(life),

	texture(texture),
	animation(animation),

	fade(fade),
	active(false)
{}



Particle::Particle(float positionX, float positionY, float speedX, float speedY, float accelerationX, float accelerationY,
	float angle, float angularSpeed, float life, SDL_Texture* texture, Animation animation, bool fade) :

	position{ positionX, positionY },
	speed{ speedX, speedY },
	acceleration{ accelerationX, accelerationY },
	angle(angle),
	angularSpeed(angularSpeed),

	life(life),
	originalLife(life),

	texture(texture),
	animation(animation),

	fade(fade),
	active(false)
{}


Particle::~Particle()
{
	texture = nullptr;
}


fMPoint Particle::GetPosition() const
{
	return position;
}


fMPoint Particle::GetSpeed() const
{
	return speed;
}


fMPoint Particle::GetAcceleration() const
{
	return acceleration;
}


float Particle::GetAngle() const
{
	return angle;
}


float Particle::GetAngularSpeed() const
{
	return angularSpeed;
}


float Particle::GetLife() const
{
	return life;
}


SDL_Texture* Particle::GetTexture() const
{
	return texture;
}


void Particle::SetPosition(fMPoint& pos)
{
	position = pos;
}


void Particle::SetSpeed(fMPoint& spd)
{
	speed = spd;
}


void Particle::SetAcceleration(fMPoint& acc)
{
	acceleration = acc;
}


void Particle::SetAngle(float ang)
{
	angle = ang;
}


void Particle::SetAngularSpeed(float aspd)
{
	angularSpeed = aspd;
}


void Particle::Update(float dt)
{
	if (active)
	{
		Move(dt);
		CheckLife(dt);
	}
}


void Particle::PostUpdate(float dt)
{
	if (active)
		Draw(dt);
}

//(SDL_Texture* texture, int x, int y, const SDL_Rect* section, bool fliped, bool cameraUse, Uint8 alpha, Uint8 r, Uint8 g, Uint8 b, float additionalScale,
//float pivotX, float pivotY, float speedX, float speedY, double angle, int rotpivot_x, int rotpivot_y

void Particle::Draw(float dt)
{
	float w = animation.GetRect().w;
	float h = animation.GetRect().h;

	if (app->map->EntityInsideCamera(position.x, position.y, w, h) == true)
	{
		if (fade == true)
		{
			Uint8 transparency = life / originalLife * 255;
			app->render->Blit(texture, position.x, position.y, &animation.GetCurrentFrameBox(dt), false, true, transparency, 255, 255, 255, 1.0f, 0, 0, 1, 1, angle);
		}

		else
			app->render->Blit(texture, position.x, position.y, &animation.GetCurrentFrameBox(dt), false, true, 255, 255, 255, 255, 1.0f, 0, 0, 1, 1, angle);
	}
}

void Particle::Move(float dt)
{
	speed += acceleration * dt * TIME_CONST;

	position += speed * dt * TIME_CONST;

	angle += angularSpeed * dt * TIME_CONST;
}


void Particle::CheckLife(float dt)
{
	life -= dt;

	if (life < 0)
	{
		Desactivate();
	}
}


void Particle::Desactivate()
{
	active = false;
}


bool Particle::Activate()
{
	return !active;
}


void Particle::Reset(float x, float y, float speedX, float speedY, float accX, float accY, float angSpeed)
{
	life = originalLife;

	position.x = x;
	position.y = y;

	speed.x = speedX;
	speed.y = speedY;

	acceleration.x = accX;
	acceleration.y = accY;

	angularSpeed = angSpeed;

	active = true;
}