/*/////////////////////////////////////////////////////////////////////////////////////
//
// Definitions of Transitions and tags.
//
	Prefix: CHARS_

	@author Ramon Molla
	@version 2015-11
*/

//#include "Copyright.h"
#include <GameCharacters.h>

//Sensitive string tags
//See UGKCharacter.h for information about their meanings
/**   
   * @fn CHARS_Tags 
   * Chars tags
   * @param CHARS_MAX_CHARTYPE: maximum number of fonts
   * Tag matrix for characters allowed
*/
UGKS_String CHARS_Tags[CHARS_MAX_CHARTYPE] = 
{
	/*
	Predefined characters by UGK
	UGK_NO_CHARACTER,
	UGK_CHARACTER_UNKNOWN, ///<For management purpouses only
	UGK_CAMERA,
	UGK_CAMERA_MNGR,
	UGK_PARTICLE,
	UGK_PARTICLE_SYSTEM,
	UGK_SHOOT,				///<Generic shoots
	UGK_SHOOT_MNGR,			///<Generic shoots Manager
		
		
	CHARS_BACKGROUND,
	CHARS_BONUS,				///<For improving player strenght
	CHARS_BONUS_MNGR,			///<For improving player strenght
	CHARS_BOOMETER,
	CHARS_BRICK,				///<Pieces that make a bunker
	CHARS_BUNKER,				///<A collection of bricks together in order to defend the player
	CHARS_CIRCLESHIP,			///<Alien space ships that turns once and once again in clircles on the screen
	CHARS_GAME,					///<The game itself 
	CHARS_LASER,				///<Additional shooting left or right device for the player
	CHARS_NAVY,					///<Holds all the supplyships and all types of ships
	CHARS_NAVYSHOOT,
	CHARS_PLAYER,				///<The player
	CHARS_REACTOR,				///<Extra power engines and shooters for the player. Obtained after the corresponding bonus has been won
	CHARS_SHIP,					///<Alien normal space ships
	CHARS_SUPPLYSHIP,			///<Super ship of the alien troops
	CHARS_WEAPON,				///<Not available by the moment
	*/

	"",
	"",
	"CAMERA",
	"CAMERAMANAGER",
	"PARTICLE",
	"PARTICLESYSTEM",
	"BACKGROUND",
	"BONUS",
	"BONUSMANAGER",
	"BOOMETER",
	"BRICK",
	"BUNKER",
	"CIRCLESHIP",
	"GAME",
	"LASER",
	"NAVY",
	"PERSONAJE",
	"PLAYER",
	"REACTOR",
	"SHIP",
	"SHOOT",
	"SHOOTMANAGER",
	"SUPPLYSHIP",
	"WEAPON"
};

///Sensitive string tags
/**   
   * @fn CHARS_Transitions 
   * Transitions of characters
   * @param CHARS_MAXTRANSITION: maximum number of transitions
   * Transition matrix for characters allowed
*/
UGKS_String CHARS_Transitions[CHARS_MAXTRANSITION] = 
{
	"DEFAULT",
	"BORNING",
	"GETTING_ALIVE",
	"BURST",
	"DIE"
};

/** Every time a new character is included, this table has to be updated
	Updating is made during the initialization phase by the initialization parser
*/
bool CHARS_COLLISION_TABLE[CHARS_MAX_CHARTYPE][CHARS_MAX_CHARTYPE];

/**
* \fn void InitCollisionTable()
* Initializes the content of the collision table setting every colliding combination to false. No object collides to any other object
*/
void InitCollisionTable()
{
	int f, c;

	for (f = 0; f < CHARS_MAX_CHARTYPE; f++)
		for (c = 0; c < CHARS_MAX_CHARTYPE; c++)
			CHARS_COLLISION_TABLE[f][c] = false;
}