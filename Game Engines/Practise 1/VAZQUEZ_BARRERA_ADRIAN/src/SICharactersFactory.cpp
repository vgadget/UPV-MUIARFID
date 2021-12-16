/*	Definition of the class S.I. Characters Factory

	Prefix: CSIF_

	@author Ramon Molla
	@version 2016-05
*/

#include <SICharactersFactory.h>

#include <Bonus.h>
#include <Brick.h>
#include <Bunker.h>
#include <Camera.h>			// Header File class Camera
#include <CircleShip.h>
#include <Laser.h>
#include <Player.h>
#include <Personaje.h>
#include <Reactor.h>
#include <Ship.h>
#include <ShootsManager.h>
#include <SIGame.h>
#include <SupplyShip.h>
#include <BonusManager.h>
#include <Navy.h>
#include <Navy.h>

extern RTDESK_CEngine	*RTDESK_Engine;
extern CSIGame	*Game;						// The game singleton
extern CTexturesManager	TexturesManager;	// The Textures Manager singleton

using namespace UGK;

/**
* @brief CSICharactersFactory \n
* Represents a Factory of CCharacters objects. Implements CCharactersFactory.
/*
* @fn     createNewCharacter
* @param  int characterType Type of CCharacter to be created.
* @return Pointer to the new CCharacter created.
* @brief  Creates a new CCharacter of the specified type.
*/
CCharacter* CSICharactersFactory::createNewCharacter(int Type, int subType)
{
	CCharacter *CAux;

	switch (Type)
	{
	case  CHARS_BONUS:				CAux = new CBonus();
		break;
	case  CHARS_BONUS_MNGR:			CAux = new CBonusManager();
		break;
	case  CHARS_BRICK:				CAux = new CBrick();
		break;
	case  CHARS_BUNKER:				CAux = new CBunker();
		break;
	case  CHARS_CIRCLESHIP:			CAux = new CCircleShip();
		break;
	case  CHARS_GAME:				CAux = new CSIGame();
		break;
	case  CHARS_LASER:				CAux = new CLaser();
		break;
	case  CHARS_NAVY:				CAux = new CNavy();
		break;
	case  CHARS_BACKGROUND:			CAux = new CCharacter();
		break;
	case  CHARS_PLAYER:				CAux = new CPlayer();
		break;
	case CHARS_PERSONAJE:			CAux = new CPersonaje();
		break;
	case  CHARS_REACTOR:			CAux = new CReactor();
		break;
	case  CHARS_SHIP:				
		CAux = new CShip();
		CAux->SubType = subType;
		break;
	case  CHARS_SHOOT:				
		CAux = new CShoot();
		break;
	case  CHARS_SHOOT_MNGR:			CAux = new CShootsManager();
		break;
	case  CHARS_SUPPLYSHIP:			CAux = new CSupplyShip();
		break;
	case  CHARS_WEAPON:				CAux = new CCharacter();
		break;
	default:						CAux = new CCharacter();
		break;
	}

#ifdef CHAR_USE_QUADTREE
	CAux->SetQTRoot(QTRoot);
#elif CHAR_USE_AABB
#endif

	CAux->SetMsgDispatcher(RTDESK_Engine->GetMsgDispatcher());
	CAux->Directory.resize(CHARS_MAX_REFERENCES);
	CAux->Directory[CHARS_GAME_REF] = Game;	//Neccesary to send back a msg to the main game singleton indicating that the player life is lost
	CAux->Directory[CHARS_TEXTURES_MNGR_REF] = &TexturesManager;	//Neccesary to access the textures used for every character to be rendered
	
	CAux->SubType = subType;

	return CAux;
}

/**
* @fn     getNumDistinctCharacterTypes
* @return The number of different types of Characters that this factory can produce.
* @brief  Gets the total number of different types of Characters that this factory can produce.
*/
int CSICharactersFactory::getNumDistinctCharacterTypes()
{
	return CHARS_MAX_CHARTYPE;
}

