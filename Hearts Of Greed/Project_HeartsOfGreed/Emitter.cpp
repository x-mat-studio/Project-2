#include "Emitter.h"
#include "Textures.h"
#include "Render.h"
#include "Particle.h"
#include "App.h"
#include "math.h"

Emitter::Emitter(fMPoint& position, fMPoint& particleSpeed, iMPoint& particleVariationSpeed,
	fMPoint& particleAcceleration, iMPoint& particleVariationAcceleration, float particleAngularSpeed,
	int particleVariableAngularSpeed, float particlesRate, float particlesLifeTime, SDL_Rect& areaOfSpawn, SDL_Texture* texture, Animation particleAnimation, bool fade) :

	position(0, 0),
	offSet(position),
	particleSpeed(particleSpeed),
	particleVariationSpeed(particleVariationSpeed),
	particleAcceleration(particleAcceleration),
	particleVariationAcceleration(particleVariationAcceleration),
	particleAngularSpeed(particleAngularSpeed),
	particleVariationAngularSpeed(particleVariableAngularSpeed),

	particlesRate(particlesRate),
	particlesLifeTime(particlesLifeTime),

	particlesEmited(0),
	particlesPerFrame(0),

	areaOfSpawn(areaOfSpawn),
	particleTexture(texture),
	particleAnimation(particleAnimation),

	randomizePosX(true),
	randomizePosY(true),

	randomizeSpeedX(true),
	randomizeSpeedY(true),

	randomizeAccelerationX(true),
	randomizeAccelerationY(true),

	randomizeAngularSpeed(true),

	active(false),
	stopped(false),

	fadeParticles(fade),

	timeSinceStopped(0)

{
	//Start();
}


Emitter::Emitter(float positionX, float positionY, float particleSpeedX, float particleSpeedY, int particleVariationSpeedX, int particleVariationSpeedY,
	float particleAccelerationX, float particleAccelerationY, int particleVariationAccelerationX, int particleVariationAccelerationY, float particleAngularSpeed,
	int particleVariableAngularSpeed, float particlesRate, float particlesLifeTime, SDL_Rect& areaOfSpawn, SDL_Texture* texture, Animation particleAnimation, bool fade) :

	position(0, 0),
	offSet{ positionX, positionY },
	particleSpeed{ particleSpeedX, particleSpeedY },
	particleVariationSpeed{ particleVariationSpeedX, particleVariationSpeedY },
	particleAcceleration{ particleAccelerationX, particleAccelerationY },
	particleVariationAcceleration{ particleVariationAccelerationX, particleVariationAccelerationY },
	particleAngularSpeed(particleAngularSpeed),
	particleVariationAngularSpeed(particleVariableAngularSpeed),

	particlesRate(particlesRate),
	particlesLifeTime(particlesLifeTime),

	particlesEmited(0),
	particlesPerFrame(0),

	areaOfSpawn(areaOfSpawn),
	particleTexture(texture),
	particleAnimation(particleAnimation),

	randomizePosX(true),
	randomizePosY(true),

	randomizeSpeedX(true),
	randomizeSpeedY(true),

	randomizeAccelerationX(true),
	randomizeAccelerationY(true),

	randomizeAngularSpeed(true),

	active(false),
	stopped(false),

	fadeParticles(fade),

	timeSinceStopped(0)

{
	//Start();
}


void Emitter::Start()
{
	int maxParticles = particlesRate * particlesLifeTime + 1;

	//We assume that the game will allways go at 60 FPS
	particlesPerFrame = particlesRate * 16 / 1000;

	particleVector.reserve(maxParticles);

	for (int i = 0; i < maxParticles; i++)
	{
		CreateParticle();
	}

	//Set all the bools to check what variables will be randomized in the Generate() functions
	if (areaOfSpawn.w == 0 && areaOfSpawn.h == 0)
	{
		randomizePosX = false;
		randomizePosY = false;
	}


	if (particleVariationSpeed.x == NULL)
	{
		randomizeSpeedX = false;
	}

	if (particleVariationSpeed.y == NULL)
	{
		randomizeSpeedY = false;
	}


	if (particleVariationAcceleration.x == NULL)
	{
		randomizeAccelerationX = false;
	}

	if (particleVariationAcceleration.y == NULL)
	{
		randomizeAccelerationY = false;
	}

	if (particleVariationAngularSpeed == 0)
	{
		randomizeAngularSpeed = false;
	}
}


void Emitter::CreateParticle()
{
	particleVector.push_back(Particle(particlesLifeTime, particleTexture, particleAnimation, fadeParticles));
}


Emitter::~Emitter()
{
	particleVector.clear();

	particleTexture = nullptr;
}


void Emitter::Update(float dt)
{

	if (stopped == false && active == true && app->gamePause == false)
	{
		ThrowParticles();
	}

	else if (stopped == true && active == true)
	{
		CheckTimeSinceStopped(dt);
	}

	if (active == true)
	{
		int numParticles = particleVector.size();


		for (int i = 0; i < numParticles; i++)
		{
			particleVector[i].Update(dt);
		}
	}
}


void Emitter::PostUpdate(float dt)
{
	if (active == true)
	{
		int numParticles = particleVector.size();

		for (int i = 0; i < numParticles; i++)
		{
			particleVector[i].PostUpdate(dt);
		}
	}

}


void Emitter::Desactivate()
{
	stopped = true;
}


void Emitter::Activate()
{
	active = true;
	stopped = false;
}


void Emitter::CheckTimeSinceStopped(float dt)
{
	timeSinceStopped += dt;

	if (timeSinceStopped <= particlesLifeTime)
	{
		timeSinceStopped = 0;
		active = false;
	}
}


//This funtions activates the necesary particles and gives them the initial values
void Emitter::ThrowParticles() {

	//You could use delta time instead of particlesPerFrame, but i dont recommend it
	particlesEmited += particlesPerFrame;

	if (particlesEmited >= 1)
	{
		int emited = 0;

		for (int i = 0; i < particleVector.size(); i++)
		{
			//TODO 2: Call Activate(), use Generate...() functions to get the parameters Activate() needs.
			//Activate returns false if the particle is already active, and true if we activate it.

			if (particleVector[i].Activate())
			{
				particleVector[i].Reset(GeneratePosX(), GeneratePosY(), GenerateSpeedX(), GenerateSpeedY(), GenerateAccelerationX(), GenerateAccelerationY(), GenerateAngularSpeed());
				emited++;
			}

			//If we activated the necesary particles this frame, break
			if ((int)particlesEmited == emited)
				break;

		}

		particlesEmited -= emited;
	}
}


float Emitter::GeneratePosX()
{
	if (randomizePosX == true)
	{
		float x = (rand() % areaOfSpawn.w) + position.x + offSet.x;
		return x;
	}

	else
		return position.x + offSet.x;
}


float Emitter::GeneratePosY()
{
	if (randomizePosY == true)
	{
		float y = (rand() % areaOfSpawn.h) + position.y + offSet.y;
		return y;
	}

	else
		return position.y + offSet.y;
}


float Emitter::GenerateSpeedX()
{
	if (randomizeSpeedX == true)
	{
		float speedX = (rand() % particleVariationSpeed.x) + particleSpeed.x;
		return speedX;
	}

	else
		return particleSpeed.x;
}


float Emitter::GenerateSpeedY()
{
	if (randomizeSpeedY == true)
	{
		float speedY = (rand() % particleVariationSpeed.y) + particleSpeed.y;
		return speedY;
	}

	else
		return particleSpeed.y;

}


float Emitter::GenerateAccelerationX()
{
	if (randomizeAccelerationX == true)
	{
		float accX = (rand() % particleVariationAcceleration.x) + particleAcceleration.x;
		return accX;
	}

	else
		return particleAcceleration.x;
}


float Emitter::GenerateAccelerationY()
{
	if (randomizeAccelerationY == true)
	{
		float accY = (rand() % particleVariationAcceleration.y) + particleAcceleration.y;
		return accY;
	}

	else
		return particleAcceleration.y;
}


float Emitter::GenerateAngularSpeed()
{
	if (randomizeAngularSpeed == true)
	{
		float angularSpeed = (rand() % particleVariationAngularSpeed) + particleAngularSpeed;
		return angularSpeed;
	}

	else
		return particleAngularSpeed;
}


void Emitter::GetPosition(int& x, int& y) const
{
	x = position.x;
	y = position.y;
}


void Emitter::SetPosition(int x, int y)
{
	position.x = x;
	position.y = y;
}


void Emitter::SetTextureNStart(SDL_Texture* tex)
{
	particleTexture = tex;
	Start();

	active = true;
}