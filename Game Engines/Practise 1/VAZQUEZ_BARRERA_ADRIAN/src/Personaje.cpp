#include "Personaje.h"

void CPersonaje::Init()
{
	InitAttr(0, 0, 0, 0, false);
	InitSpeed(0, 0, 0);
	InitPos(0, 0, 0);

}

void CPersonaje::InitAttr(int health, int weight, int fuel, int arms, bool inmortal)
{
	Health = health;
	Weight = weight;
	Fuel = fuel;
	Arms = arms;
	Inmortal = inmortal;
}

void CPersonaje::InitSpeed(float x, float y, float z)
{
	Speed.Set(0, 0, 0);
}

void CPersonaje::InitPos(float x, float y, float z)
{
	Position.Set(0, 0, 0);
}
