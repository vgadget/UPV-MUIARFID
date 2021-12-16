#ifndef CPE_PERSONAJE
#define CPE_PERSONAJE

#include <GameCharacters.h>
#include <GlobalTiming.h>


class CPersonaje : public CCharacter
{
public:
	int Health;
	int Weight;
	int Fuel;
	int Arms;
	bool Inmortal;

	CPersonaje() { Init(); }
	~CPersonaje();

	void Init();

	void InitAttr(int health, int weight, int fuel, int arms, bool inmortal);
	
	void InitSpeed(float x, float y, float z);

	void InitPos(float x, float y, float z);

	inline void SetLocalTimers(unsigned int Size) { Timer.resize(Size); UpdateSF(TimerManager.GetSF()); }

	inline void	AI_Init(void) {}
	inline void	AI_Die(void) {}

	///What the character has to do on every time tick 
	inline void Update(void) {}

	///Manages all the events received
	inline void ManageEvent(unsigned int Event) {}

	inline void Collided(CCharacter* CollidedChar) {}

	///Shows the character on the screen
	inline void Render(void) {}
	
	///Change the way the character is going to be rendered on the screen
	inline void ChangeRenderMode(CHAR_RENDER_MODE) {}

	inline void MoveTo(UGKALG_NUMERIC_TYPE x, UGKALG_NUMERIC_TYPE y, UGKALG_NUMERIC_TYPE z) {}
	
	///Moving to an absolute position. It does not matter where the character is
	inline void MoveTo(Vector& Move) {}
	inline void MoveTo(SpaceCoords Dim, UGKALG_NUMERIC_TYPE Value) {}

	inline void MoveRelTo(float x, float y, float z) {}
	inline void MoveRelTo(SpaceCoords Dim, UGKALG_NUMERIC_TYPE Value) {}

	inline void MoveRelTo(Vector& Move) {}

};

#endif