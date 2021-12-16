/**	Declaration of all the parser methods to understand the initialization files

Prefix IP_: Default HTML Labels for all Parsers

*	@author Ramon Molla, Carlos Martinez Perez
*	@version 2011-02
*	@author Ramón Mollá
*	@version 2015-12
*/

#include <InitializationParser.h>
#include <Global.h>
#include <SIGame.h>
#include <GameSounds.h>

#include <Player.h>
#include <Personaje.h>
#include <Ship.h>
#include <CircleShip.h>
#include <SupplyShip.h>
#include <Laser.h>
#include <Bunker.h>
#include <SITexturesResources.h>
#include <UGKCharactersFactory.h>
#include <SICharactersFactory.h>
#include <Navy.h>

#define CIP_DEBUG	//Class HTML Parser Debug activated

//Retrieves the lexeme from the token number
UGKS_String IP_Lexeme(unsigned int T)
{
	if (T >= MAXTAGS_D)
		 return CIP_Tags[T - MAXTAGS_D];
	else return DHTML_TagsArray[T];
}

//Externals
extern UGKS_String CHARS_Tags[CHARS_MAX_CHARTYPE];

///Sensitive string tags
UGKS_String CIP_Tags[MAXTAGS_IP - MAXTAGS_D] =
{
	"ACCELERATION",
	"AI",
	"ANIMATION2D",
	"CENTER",
	"CHARACTER",
	"COLLIDED",
	"COLLISION",
	"COLLISION_TABLE",
	"CONFIG",
	"DIR",
	"EPS",							///<Energy Per Shoot
	"EXPLOSION",
	"FILE",
	"GEOMETRY",
	"HAVE",							///<Recursive definition of a character
	"HEALTH",						///<How strong is the character. How much has to be hurt before dieing
	"LANGUAGE",
	"LEVEL",
	"LINE",
	"LIVES",						///<Amount of ships the Player has still before finishing the game
	"MESH",
	"MUSIC",
	"NAME",
	"PERIODE",
	"POSITION",
	"RADIO",
	"ROTATION",
	"SCALE",
	"SCORE",
	"SCORES",
	"SHOOT",						///<Kind of shooting
	"SPEED",
	"SPS",							///<Shoots Per Second
	"SOUNDS",
	"TEXTURE",
	"TEXTURE2D",
	"TEXTURE3D",
	"VELOCITY",
	"X",							///<X dimension of the character described
	"Y",							///<Y dimension of the character described
	"Z"								///<Z dimension of the character described
};

UGKS_String CIP_ParserMsgs[CIP_MAX_MSGS] = 
{
	"Minimum amount of lives reported (",
	"Maximum amount of lives reported (",
	") is lower than the minimum ",
	") is higher than the maximum ",
	"amount allowed by the program (1).",
	" Value cut to the ",
	"minimum.",
	"maximum.",
	"Initialization",
	"Speed coordinates not defined. Tag ",
	" erroneous.",
	"Tag ",
	" not allowed in context "
};

UGKS_String CIP_Ani2DTypes[MAX_NUM_ANI_TYPES] = 
{
	"ANITYPE_CICLIC",
	"ANITYPE_PINGPONG",
	"ANITYPE_SERIAL"
};

extern CSIGame	*Game;
extern CNavy	*Navy;

/**
*	\defgroup	default	Default characters
*	 Used by the parsers while reading objects
*	 Default characters for initialization purpouses only
*    Updated when the initialization parser is running
*	 @{
*/
CCharacter*		defaultChar;

CBonus*			defaultBonus;
CBunker*		defaultBunker;
CBrick*			defaultBrick;
CPlayer*		defaultPlayer;
CPersonaje* defaultPersonaje;
CShip*			defaultShip;
CSupplyShip*	defaultSShip;
CCircleShip*	defaultCShip;

GCHARS_CharacterType CharType;
bool MeshSemaphore[CHARS_MAX_CHARTYPE];

/**
@fn LoadTextures
Load basic application textures.
*/
void LoadTextures(void)
{
	TexturesManager.Textures.clear();

	TexturesManager.SetFilesPath(Game->Directories[CG_TEXTURE_DIR]);

	for (unsigned int indexList = 0; indexList<CTM_MAX_TEXTURE; indexList++)
		TexturesManager.CreateTexture(CTM_TexturesFiles[indexList], indexList);
}

CInitializationParser::CInitializationParser()
{
	Type = CIP_ParserMsgs[CIP_INITIALIZATION];	//Type of parser

	AssignCharacterTags();
}

/**   
   @fn void CInitializationParser::Init(UGKS_String FileName)
   @param UGKS_String FileName : file name
   Number starts at 0 bunkers    
   Starts the default values ??at the beginning of each level in the game
*/
void CInitializationParser::Init(UGKS_String FileName)
{
	CHTMLParser::Init(FileName);
};

void CInitializationParser::InitializeSounds2DefaultChar()
{
	unsigned int l;

	//Default Bonus sounds initialization
	defaultBonus->SetSoundsAmount(CBN_MAX_SND);
	defaultBonus->SetSound(SoundsManager.GetSound(CGS_SHIELD_SND), CBN_SHIELD_SND);
	defaultBonus->SetSound(SoundsManager.GetSound(CGS_BONUS_SND), CBN_BONUS_SND);

	//Default Bunker sounds initialization 
	defaultBunker->SetSoundsAmount(CB_MAX_SOUND);
	defaultBunker->SetSound(SoundsManager.GetSound(CGS_TOUCH_SND), CB_TOUCH_SND);

	//Default Brick sounds initialization
	defaultBrick->SetSoundsAmount(CB_MAX_SOUND);
	defaultBrick->SetSound(SoundsManager.GetSound(CGS_TOUCH_SND), CB_TOUCH_SND);

	//Default Player sounds initialization 
	defaultPlayer->SetSoundsAmount(CPL_MAX_SND);
	defaultPlayer->SetSound(SoundsManager.GetSound(CGS_EXPLOSION_SND), CPL_EXPLOSION_SND);
	defaultPlayer->SetSound(SoundsManager.GetSound(CGS_SHOOT_SND), CPL_SHOOT_SND);
	defaultPlayer->SetSound(SoundsManager.GetSound(CGS_SHOOT3D_SND), CPL_SHOOT3D_SND);
	defaultPlayer->SetSound(SoundsManager.GetSound(CGS_TOUCH_SND), CPL_TOUCH_SND);

	for (l = 0; l < CPL_MAX_LASERS; l++)
	{
		defaultPlayer->Laser[l].SetSoundsAmount(CPL_MAX_SND);
		defaultPlayer->Laser[l].SetSound(SoundsManager.GetSound(CGS_EXPLOSION_SND), CPL_EXPLOSION_SND);
		defaultPlayer->Laser[l].SetSound(SoundsManager.GetSound(CGS_SHOOT_SND), CPL_SHOOT_SND);
		defaultPlayer->Laser[l].SetSound(SoundsManager.GetSound(CGS_SHOOT3D_SND), CPL_SHOOT3D_SND);
		defaultPlayer->Laser[l].SetSound(SoundsManager.GetSound(CGS_TOUCH_SND), CPL_TOUCH_SND);
	}
	//Default Ship sounds initialization 
	defaultShip->SetSoundsAmount(CN_MAX_SND);
	defaultShip->SetSound(SoundsManager.GetSound(CGS_DESCEND_SND), CN_DESCEND_SND);		//Every time the whole navy drops down a little bit
	defaultShip->SetSound(SoundsManager.GetSound(CGS_EXPLOSION_SND), CN_EXPLOSION_SND);	//Every time any ship, supplyship, circleship,... burst out

	//Default Supply Ship sounds initialization 
	defaultSShip->SetSoundsAmount(CN_MAX_SND);
	defaultSShip->SetSound(SoundsManager.GetSound(CGS_DESCEND_SND), CN_DESCEND_SND);		//Every time the whole navy drops down a little bit
	defaultSShip->SetSound(SoundsManager.GetSound(CGS_EXPLOSION_SND), CN_EXPLOSION_SND);	//Every time any ship, supplyship, circleship,... burst out

	//Default Circle Ship sounds initialization 
	defaultCShip->SetSoundsAmount(CN_MAX_SND);
	defaultCShip->SetSound(SoundsManager.GetSound(CGS_DESCEND_SND), CN_DESCEND_SND);		//Every time the whole navy drops down a little bit
	defaultCShip->SetSound(SoundsManager.GetSound(CGS_EXPLOSION_SND), CN_EXPLOSION_SND);	//Every time any ship, supplyship, circleship,... burst out
}


void CInitializationParser::AssignTMG(CCharacter *Character)
{
	Character->Directory.resize(CHARS_MAX_REFERENCES);
	Character->Directory[CHARS_TEXTURES_MNGR_REF] = (CCharacter *)&TexturesManager;
	Character->Directory[CHARS_GAME_REF] = (CCharacter *)Game;

	//Assign msg dispatcher to both the SI Game singleton and Navy one
	Character->SetMsgDispatcher(RTDESK_Engine->GetMsgDispatcher());
}

void CInitializationParser::InitializeDefaults()
{
	int i;

	defaultChar		=				CharacterPool->get(UGK_CHARACTER_UNKNOWN, UGKOBJM_NO_SUBTYPE);
	defaultBonus	= (CBonus*)		CharacterPool->get(CHARS_BONUS, UGKOBJM_NO_SUBTYPE);
	defaultBunker	= (CBunker*)	CharacterPool->get(CHARS_BUNKER, UGKOBJM_NO_SUBTYPE);
	defaultBrick	= (CBrick*)		CharacterPool->get(CHARS_BRICK, UGKOBJM_NO_SUBTYPE);
	defaultPlayer	= (CPlayer*)	CharacterPool->get(CHARS_PLAYER, UGKOBJM_NO_SUBTYPE);
	defaultShip		= (CShip*)		CharacterPool->get(CHARS_SHIP, CS_NO_SHIP);
	defaultSShip	= (CSupplyShip*)CharacterPool->get(CHARS_SUPPLYSHIP, CSS_NO_SUPPLY_SHIP);
	defaultCShip	= (CCircleShip*)CharacterPool->get(CHARS_CIRCLESHIP, CCS_LEFTTYPE);

	//Set the default values for the default characters
	//DEFAULT CHARACTER
	//BONUS
	AssignTMG(defaultChar);

	//BONUS
	defaultBonus->AssignTMG(Game);
	//BUNKER
	AssignTMG(defaultBunker);
	//BRICK
	AssignTMG(defaultBrick);
	defaultBrick->Column = 0;
	defaultBrick->Row    = 0;

	//PLAYER
	defaultPlayer->MoveTo(0.0f, SIGLBD_PG_BOTTOM, 0.0f);
	defaultPlayer->AssignTMG(Game);

	defaultPlayer->Init();
	defaultPlayer->ResizeDirectory(CHARS_MAX_REFERENCES);
	defaultPlayer->AssignTextMngr((CCharacter *)&TexturesManager);
	defaultPlayer->Explosion.SubType = CE_PLAYER_EXPLOSION;

	defaultPlayer->UpdateSF(TimerManager.GetSF());
	//defaultPlayer->Timer[CPL_RND_PERIOD].SetAlarm(DefaultRndPeriod[CHARS_PLAYER]);
	defaultPlayer->Antialiasing = Application.Window.Antialiasing_active;

	for (i = 0; i<CPL_MAX_LASERS; i++)
	{
		defaultPlayer->Laser[i].Billboard = Application.Window.Billboard;
		defaultPlayer->Laser[i].BonusManager = BonusManager;
		defaultPlayer->Laser[i].AssignTMG(Game);
		defaultPlayer->Laser[i].Init();
		defaultPlayer->Laser[i].Explosion.SubType = CE_LASER_EXPLOSION;
	}

	//NAVY
	AssignTMG(Navy);

	//SHIP
	defaultShip->AssignTMG(Game);
	defaultShip->Explosion.SubType	= CE_SHIP_EXPLOSION;
	//SUPPLY SHIP
	defaultSShip->AssignTMG(Game);
	defaultSShip->Explosion.SubType = CE_SUPPLYSHIP_EXPLOSION;
	//CIRCLE SHIP
	defaultCShip->AssignTMG(Game);
	defaultCShip->Explosion.SubType = CE_SUPPLYSHIP_EXPLOSION;
	//BACKGROUND
	Background.Init();

	//Initialize defaults timers
	//BONUS
	defaultBonus->SetLocalTimers(CBN_MAX_TIMERS);
	defaultBonus->Timer[CBN_RND_PERIOD].SetAlarm(BonusManager->Timer[CBN_RND_PERIOD].GetAlarmPeriod());
	defaultBonus->Timer[CBN_UPD_PERIOD].SetAlarm(BonusManager->Timer[CBN_UPD_PERIOD].GetAlarmPeriod());

	//SHIP
	defaultShip->SetLocalTimers(CS_MAX_TIMERS);
	defaultShip->Timer[CS_RND_PERIOD].SetAlarm(Navy->Timer[CN_RND_PERIOD].GetAlarmPeriod());
	defaultShip->Timer[CS_UPD_PERIOD].SetAlarm(Navy->Timer[CN_UPD_PERIOD].GetAlarmPeriod());

	//SUPPLY SHIP
	defaultSShip->SetLocalTimers(CSS_MAX_TIMERS);
	defaultSShip->Timer[CS_RND_PERIOD].SetAlarm(Navy->Timer[CN_RND_PERIOD].GetAlarmPeriod());
	defaultSShip->Timer[CS_UPD_PERIOD].SetAlarm(Navy->Timer[CN_UPD_PERIOD].GetAlarmPeriod());

	//CIRCLE SHIP
	defaultCShip->SetLocalTimers(CCS_MAX_TIMERS);
	defaultCShip->Timer[CCS_RND_PERIOD].SetAlarm(Navy->Timer[CN_RND_PERIOD].GetAlarmPeriod());
	defaultCShip->Timer[CCS_UPD_PERIOD].SetAlarm(Navy->Timer[CN_UPD_PERIOD].GetAlarmPeriod());

	//PLAYER
	defaultPlayer->SetLocalTimers();
	defaultPlayer->Timer[CPL_RND_PERIOD].SetAlarm(Game->Timer[CSIG_UPD_PERIOD].GetAlarmPeriod());
	defaultPlayer->Timer[CPL_UPD_PERIOD].SetAlarm(Game->Timer[CSIG_RND_PERIOD].GetAlarmPeriod());
}

void  CInitializationParser::BeginParse(DWORD dwAppData, bool &bAbort)
{
	UNUSED_ALWAYS(dwAppData);
	bAbort = false;
#ifdef CIP_DEBUG	//Class HTML Parser Debug activated
	WriteLog(LOG_PARSER_BEGIN_PARSE, " . Initialization.");
#endif
	ResetState();			//Stack reset

	for (unsigned int i = 0; i< CHARS_MAX_CHARTYPE;i++)
		MeshSemaphore[i] = true;

	InitializeDefaults();
}

void CInitializationParser::StartTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort)
{
	UGKS_String TagString, msg;
	unsigned int Tag;

	TagString = UGKS_CString2string(pTag->getTagName());

#ifdef CIP_DEBUG	//Class HTML Parser Debug activated
	WriteLog(LOG_PARSER_START_TAG, TagString);
#endif

	UNUSED_ALWAYS(dwAppData);
	bAbort = false;

	Tag = Lexeme2Token(TagString);
	switch(Top())
	{case NIL_D:	//Starting and ending states
				if (HTML_D == Tag)
					//It is a HTML file. Change to HTML state
					Push(HTML_D);
				else ErrorParser(CP_ParserMsgs[LOG_PARSER_NOT_HTML_TAG]);
		break;
	case ANIMATION2D:
					switch (Tag)
					{
					case NAME:
					case PERIODE_ANI2D_CHAR:
					case TEXTURE2D:
					case TYPE_D:
						Push(Tag);
						break;
					}
		break;
	case BODY_D:		//HTML tag
					switch (Tag)
					{
					case DIR:
					case CHARACTER:
					case COLLISION_TABLE:
					case SCORES:
						Push(Tag); //Change to the state specified by the Tag
						break;
					default:
						TagNotAllowed(TagString, DHTML_TagsArray[BODY_D]);
					}
		break;
	case CHARACTER:
			if(COLLISION == SubTop())
				TagNotAllowed(TagString, IP_Lexeme(CHARACTER));
			else
					switch (Tag)
					{
					case TYPE_D:
					case TEXTURE3D:
					case ANIMATION2D:
					case ACCELERATION:
					case CENTER:
					case EXPLOSION:
					case HEALTH:
					case LIVES:
					case NAME:
					case MESH:
					case RADIO:
					case SPEED:
					case SHOOT:
					case TEXTURE2D:
					case VELOCITY:
						Push(Tag); //Change to the state specified by the Tag
						break;
					default:
						TagNotAllowed(TagString, IP_Lexeme(CHARACTER));
					}
		break;
	case COLLIDED:	//End tag. No other tag is allowed inside Collided tag. Error
			TagNotAllowed(TagString, IP_Lexeme(COLLIDED));
		break;
	case COLLISION:
					switch (Tag)
					{
					case CHARACTER:
					case COLLIDED:
						Push(Tag); //Change to the state specified by the Tag
						break;
					default:
						TagNotAllowed(TagString, IP_Lexeme(COLLISION));
					}
		break;
	case COLLISION_TABLE:
					if (COLLISION == Tag)
						Push(Tag); //Change to the state specified by the Tag
					else 
						TagNotAllowed(TagString, IP_Lexeme(COLLISION_TABLE));
		break;
	case DIR:
					switch (Tag)
					{
					case AI:
					case CONFIG:
					case GEOMETRY:
					case LANGUAGE:
					case LEVEL:
					case MUSIC:
					case SOUNDS:
					case TEXTURE:
						Push(Tag); //Change to the state specified by the Tag
						break;
					default:
						TagNotAllowed(TagString, IP_Lexeme(DIR));
					}
		break;
	case HEAD_D:		//HTML tag
					switch (Tag)
					{
					case TITLE_D:
					case VERSION_D:
					case TYPE_D:	Push(Tag); //Change to the state specified by the Tag
						break;
					default:
						TagNotAllowed(TagString, DHTML_TagsArray[HEAD_D]);
					}
		break;		
	case HTML_D:		//HTML Tag
					//The parser is at the very beginning of the iniitalization file
					switch (Tag)
					{
					case HEAD_D:		//A HEAD Tag is expected
					case BODY_D:
						Push(Tag); //Change to the state specified by the Tag
						break;
					default:
						TagNotAllowed(TagString, DHTML_TagsArray[HTML_D]);
					}					
		break;
	case NAME:	//End tag. No other tag is allowed inside Name tag. Error
	case SCORE:	//End tag. No other tag is allowed inside Score tag. Error
		TagNotAllowed(TagString, IP_Lexeme(Top()));
		break;
	case SCORES:	//A NAME Tag is expected
					switch (Tag)
					{
					case NAME:
					case SCORE:
						Push(Tag);
						break;
					default: 
						TagNotAllowed(TagString, IP_Lexeme(SCORES));
					}
					break;
	case SHOOT:		//Kind of shooting
					switch (Tag)
					{
					case SPS:	//Change to SPS state
					case EPS:	//May be that no SPS tag appears or that the SPS section is finished.
								//There is a EPS part also
						Push(Tag); //Change to the state specified by the Tag
						break;
					default:
						TagNotAllowed(TagString, IP_Lexeme(SHOOT));
					}
		break;
	case CENTER:
	case POSITION_IP:
	case ROTATION_IP:
	case SCALE_IP:
	case SPEED:
					switch (Tag)
					{
					case X_IP:
					case Y_IP:
					case Z_IP:
						Push(Tag); //Change to the state specified by the Tag
						break;
					default: 
						TagNotAllowed(TagString, IP_Lexeme(Top()));
					}
		break;
	case TEXTURE:
					switch (Tag)
					{
					case NAME:		//Change to NAME state
					case FILE_IP:	//Change to FILE state
						Push(Tag);	//Change to the state specified by the Tag
						break;
					default:
						TagNotAllowed(TagString, IP_Lexeme(TEXTURE));
					}
		break;
	case X_IP:
	case Y_IP:
	case Z_IP:
		//End tag. No other tag is allowed inside X,Y or Z tag. Error
		TagNotAllowed(TagString, IP_Lexeme(Top()));
		break;
	default:;
	};
}

void CInitializationParser::EndTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort)
{
	UGKS_String TagString;
	int Tag, StateStackTop;
	bool Error = false;
	UGKS_String msg;

	TagString = UGKS_CString2string(pTag->getTagName());

#ifdef CIP_DEBUG	//Class HTML Parser Debug activated
	WriteLog(LOG_PARSER_END_TAG_END, TagString);
#endif

	UNUSED_ALWAYS(dwAppData);
	bAbort = false;

	Tag = Lexeme2Token(TagString);
	StateStackTop = Top();
	switch(StateStackTop)
	{
	 case ACCELERATION:
	 case AI:
	 case BODY_D:
	 case CENTER:
	 case COLLIDED:	
	 case COLLISION:
	 case COLLISION_TABLE:
	 case CONFIG:
	 case CHARACTER:
	 case DIR:
	 case EPS:		//Energy Per Shoot
	 case EXPLOSION:
	 case FILE_IP:
	 case GEOMETRY:
	 case HAVE:		//Recursive definition of a character
	 case HEAD_D:		
	 case HEALTH:	//How strong is the character. How much has to be hurt before dieing
	 case HTML_D:		//The parser is at the very end of the initialization file					
	 case LANGUAGE:
	 case LEVEL:
	 case LIVES:	
	 case MESH:
	 case MUSIC:
 	 case PERIODE_ANI2D_CHAR:
	 case SOUNDS:
	 case RADIO:
	 case SCORE:
	 case SCORES:	
	 case SHOOT:		
	 case SPEED:
	 case SPS:		//Shoots Per Second
	 case TEXTURE:
	 case TEXTURE2D:
	 case TEXTURE3D:
	 case TITLE_D:		
	 case TYPE_D:
	 case VERSION_D:
	 case VELOCITY:
	 case X_IP:
	 case Y_IP:
	 case Z_IP:
			if (StateStackTop == Tag) Pop();	//Change to the previous state
			else Error = true;
		break;
	 case ANIMATION2D:
			if (StateStackTop == Tag)
			{
				Pop();	//Change to the previous state
				switch (CharType)
				{
				case CHARS_BONUS:
						break;
				case CHARS_BRICK:
						break;
				case CHARS_BUNKER:
						break;
				case CHARS_PLAYER:
					defaultPlayer->IndAnimation2D = AnimationsManager.Animations.size()-1;
						break;
				case CHARS_SHIP:
					defaultShip->IndAnimation2D = AnimationsManager.Animations.size()-1;
						break;
				case CHARS_SUPPLYSHIP:
					defaultSShip->IndAnimation2D = AnimationsManager.Animations.size()-1;
						break;
				case CHARS_CIRCLESHIP:
					defaultCShip->IndAnimation2D = AnimationsManager.Animations.size()-1;
					break;
				case CHARS_LASER:
					for(int i=0;i<CPL_MAX_LASERS;i++)
						defaultPlayer->Laser[i].IndAnimation2D = AnimationsManager.Animations.size()-1;
						break;
				case CHARS_BACKGROUND:
					Background.IndAnimation2D = AnimationsManager.Animations.size()-1;
					break;
				}
			}
			else Error = true;
		 break;
	 case NAME:
		 switch (SubTop())
		 {
		 case TEXTURE:
		 case CHARACTER:
		 case SCORES:
		 case ANIMATION2D:
			 Pop();	//Change to the previous state
			 break;
		 default:
			 Error = true;
		 }
		 break;
	 case NIL_D:	//This tag is for management purpouses only. There is no correspondence to any string label in the HTML file
			msg  = TagString;
			msg += CP_ParserMsgs[LOG_PARSER_END_TAG_UNK];
			ErrorParser(msg);
		break;
	default:;
	};
	if (Error)
	{	msg  = TagString;
		msg += CP_ParserMsgs[LOG_PARSER_END_TAG_ERROR2];
		msg += IP_Lexeme(StateStackTop);
		ErrorParser(msg);
	}
}

void CInitializationParser::Characters(const UGKS_String &rText, DWORD dwAppData, bool &bAbort)
{
double			ms;
unsigned int	i;
int				CurrentLives,
				TextureId;
static GCHARS_CharacterType	Char2Collide;
GCHARS_CharacterType		Collided;

UGKS_String		msg;
CTexture        *Texture;
CTextureAnimation *aniAux;

#ifdef CIP_DEBUG	//Class HTML Parser Debug activated
	WriteLog(LOG_PARSER_TEXT, rText );
#endif

	UNUSED_ALWAYS(dwAppData);
	bAbort = false;

	switch(Top())
	{
	
	case ACCELERATION:
		switch (CharType)
		{
		case CHARS_PLAYER:
			defaultPlayer->Acceleration.v[XDIM] = atof(UGKS_string2charstr(rText));
			break;
		case CHARS_SHIP:
			defaultShip->Acceleration.v[XDIM] = atof(UGKS_string2charstr(rText));
			break;
		}
		break;
	case AI:
		Game->Directories[CG_AI_DIR] = rText;
		break;
	case CENTER:
		break;
	case CHARACTER:
		if (COLLISION == SubTop())
			Char2Collide = (GCHARS_CharacterType) CharLexeme2Token(rText);
		break;
	case COLLIDED:
		if (COLLISION == SubTop())
		{
			Collided = (GCHARS_CharacterType)CharLexeme2Token(rText);
			CHARS_COLLISION_TABLE[Char2Collide][Collided] = true;	//Activate the collisionbetween both characters
		}
		break;
	case CONFIG:
		Game->Directories[CG_CONFIG_DIR] = rText;
		break;
	case EPS:		//Energy Per Shoot
		break;
	case EXPLOSION:
			switch (CharType)
			{
			case CHARS_BONUS:
				 defaultBonus->Hit_duration	= atof(UGKS_string2charstr(rText));
				break;
			case CHARS_BRICK:
				 defaultBrick->Hit_duration	= atof(UGKS_string2charstr(rText));
				 break;
			case CHARS_BUNKER:
				 defaultBunker->Hit_duration	= atof(UGKS_string2charstr(rText));
				 break;
			case CHARS_CIRCLESHIP:
 				 defaultCShip->Hit_duration	= atof(UGKS_string2charstr(rText));
				 break;
			case CHARS_LASER:
				defaultPlayer->Laser[CPL_LEFT_LASER].Hit_duration	= atof(UGKS_string2charstr(rText));
				defaultPlayer->Laser[CPL_RIGHT_LASER].Hit_duration	= atof(UGKS_string2charstr(rText));
				 break;
			 case CHARS_PLAYER:
				 defaultPlayer->Hit_duration	= atof(UGKS_string2charstr(rText));
				 break;
			case CHARS_SHIP:
				 defaultShip->Hit_duration	= atof(UGKS_string2charstr(rText));
				 break;
			case CHARS_SUPPLYSHIP:
 				 defaultSShip->Hit_duration	= atof(UGKS_string2charstr(rText));
				 break;
			}
		break;
	case FILE_IP:
			if (TEXTURE == SubTop())
			///Save the texture folder on a game singleton attribute 
			Game->Directories[CG_TEXTURE_DIR] = rText;
		break;
	case GEOMETRY:
			MeshesManager.SetFilesPath(rText);
			Game->Directories[CG_MESH_DIR] = rText;
		break;
	case HEALTH:	//How strong is the character. How much has to be hurt before dieing
			switch (CharType)
			{
			case CHARS_BONUS:
				 defaultBonus->Health	= atof(UGKS_string2charstr(rText));
				break;
			case CHARS_BRICK:
				 defaultBrick->Health	= atof(UGKS_string2charstr(rText));
				 break;
			case CHARS_BUNKER:
				 defaultBunker->Health	= atof(UGKS_string2charstr(rText));
				 break;
			case CHARS_CIRCLESHIP:
 				 defaultCShip->Health	= atof(UGKS_string2charstr(rText));
				 break;
			case CHARS_LASER:
				defaultPlayer->Laser[CPL_LEFT_LASER].Health	= atof(UGKS_string2charstr(rText));
				defaultPlayer->Laser[CPL_RIGHT_LASER].Health	= atof(UGKS_string2charstr(rText));
				 break;
			 case CHARS_PLAYER:
				 defaultPlayer->Health	= atof(UGKS_string2charstr(rText));
				 break;
			case CHARS_SHIP:
				 defaultShip->Health		= atof(UGKS_string2charstr(rText));
				 break;
			 case CHARS_SUPPLYSHIP:
 				 defaultSShip->Health	= atof(UGKS_string2charstr(rText));
				 break;
			}
		break;
	case LANGUAGE:
		msg = rText;
		Application.LanguageFolder = msg;
		break;
	case LEVEL:
			Game->Directories[CG_LEVEL_DIR] = rText;
			break;
	case LIVES:		//Amount of ships the Player has still before finishing the game
			CurrentLives = atoi(UGKS_string2charstr(rText));
			if (CHAR_HEALTH_INFINITE == CurrentLives)
					defaultPlayer->Lives = CHAR_HEALTH_INFINITE;
			else if (0 > CurrentLives)
				{
					msg  = CIP_ParserMsgs[CIP_MIN_LIVES_NOT_DEF];
					msg += rText;
					msg += CIP_ParserMsgs[CIP_LOWER_THAN_MIN];
					msg += CIP_ParserMsgs[CIP_AMOUNT_ALLOWED];
					msg += CIP_ParserMsgs[CIP_VALUE_CUT];
					msg += CIP_ParserMsgs[CIP_MINIMUM];
					ErrorParser (msg);
					defaultPlayer->Lives = 1;
				}
				else if (SHRT_MAX > CurrentLives) defaultPlayer->Lives = CurrentLives;
					 else	
					 {
						msg  = CIP_ParserMsgs[CIP_MAX_LIVES_NOT_DEF];
						msg += rText;
						msg += CIP_ParserMsgs[CIP_LOWER_THAN_MIN];
						msg += CIP_ParserMsgs[CIP_AMOUNT_ALLOWED];
						msg += CIP_ParserMsgs[CIP_VALUE_CUT];
						msg += CIP_ParserMsgs[CIP_MAXIMUM];
						ErrorParser (msg);
						defaultPlayer->Lives = SHRT_MAX;
					}
		break;
	case MESH:
			switch (CharType)
			{
			case CHARS_BONUS:
				 break;
			case CHARS_BRICK:
				 break;
			case CHARS_BUNKER:
				 break;
			case CHARS_PLAYER:
				if (MeshSemaphore[CHARS_PLAYER])
				{
					MeshSemaphore[CHARS_PLAYER] = false;
					int ind = MeshesManager.AddModel(rText);
					defaultPlayer->IndMesh = ind;
					defaultPlayer->Mesh = MeshesManager.GetMesh(ind);
					defaultPlayer->SetMeshName(defaultPlayer->Mesh->GetFileName());
				}
				 break;
			case CHARS_SHIP:
				if (MeshSemaphore[CHARS_SHIP])
				{
					MeshSemaphore[CHARS_SHIP] = false;
					int ind = MeshesManager.AddModel(rText);
					defaultShip->IndMesh = ind;
					defaultShip->Mesh = MeshesManager.GetMesh(ind);
					defaultShip->SetMeshName(defaultShip->Mesh->GetFileName());
				}
				 break;
			case CHARS_SUPPLYSHIP:
				if (MeshSemaphore[CHARS_SUPPLYSHIP])
				{
					MeshSemaphore[CHARS_SUPPLYSHIP] = false;
					int ind = MeshesManager.AddModel(rText);
					defaultSShip->IndMesh = ind;
					defaultSShip->Mesh = MeshesManager.GetMesh(ind);
					defaultSShip->SetMeshName(defaultSShip->Mesh->GetFileName());
				}
				 break;
			case CHARS_CIRCLESHIP:
				if (MeshSemaphore[CHARS_CIRCLESHIP])
				{
					MeshSemaphore[CHARS_CIRCLESHIP] = false;
					int ind = MeshesManager.AddModel(rText);
					defaultCShip->IndMesh = ind;
					defaultCShip->Mesh = MeshesManager.GetMesh(ind);
					defaultCShip->SetMeshName(defaultCShip->Mesh->GetFileName());
					}
				break;
			case CHARS_LASER:
				if (MeshSemaphore[CHARS_LASER])
				{
					MeshSemaphore[CHARS_LASER] = false;
					int ind = MeshesManager.AddModel(rText);
					for(i=0;i<CPL_MAX_LASERS;i++)
					{
						defaultPlayer->Laser[i].IndMesh = ind;
						defaultPlayer->Laser[i].Mesh = MeshesManager.GetMesh(ind);
						defaultPlayer->Laser[i].SetMeshName(defaultPlayer->Laser[i].Mesh->GetFileName());
					}
				}
				 break;
			case CHARS_BACKGROUND:
				if (MeshSemaphore[CHARS_BACKGROUND])
				{
					MeshSemaphore[CHARS_BACKGROUND] = false;
					int ind = MeshesManager.AddModel(rText);
					Background.IndMesh = ind;
					Background.Mesh = MeshesManager.GetMesh(ind);
					Background.SetMeshName(Background.Mesh->GetFileName());
				}
				break;
			}
		break;
	case MUSIC:
			SoundsManager.SetMusicPath(rText);
			Game->Directories[CG_MUSIC_DIR] = rText;
			break;
	case NAME:
		switch (SubTop())
		{
		case CHARACTER:
			CharType = (GCHARS_CharacterType)CharacterFactory->Lexeme2Token(rText);
			switch (CharType)
			{
			case CHARS_BONUS:
			case CHARS_BRICK:
			case CHARS_BUNKER:
			case CHARS_PLAYER:
			case CHARS_SHIP:
			case CHARS_SUPPLYSHIP:
			case CHARS_LASER:
			case CHARS_CIRCLESHIP:
			case CHARS_BACKGROUND:
				break;
			case UGK_CHARACTER_UNKNOWN:
				TagError(rText);
				break;
			};
			break;
		case ANIMATION2D:
			switch (CharType)
			{
			case CHARS_BONUS:
			case CHARS_BRICK:
			case CHARS_BUNKER:
			case CHARS_PLAYER:
			case CHARS_SHIP:
			case CHARS_SUPPLYSHIP:
			case CHARS_CIRCLESHIP:
			case CHARS_LASER:
			case CHARS_BACKGROUND:
				msg = rText;
				AnimationsManager.CreateAnimation(msg, 0.0, (ANI_ANIMATION_TYPE)0);
				break;
			}
			break;
		case SCORES:
			//Name of the player that has got a top mark in this game.
			break;
		default:
			break;
		}
		break;
	case PERIODE_ANI2D_CHAR:
			switch (CharType)
			{
			case CHARS_BONUS:
			case CHARS_BRICK:
			case CHARS_BUNKER:
			case CHARS_PLAYER:
			case CHARS_SHIP:
			case CHARS_SUPPLYSHIP:
			case CHARS_CIRCLESHIP:
			case CHARS_LASER:
			case CHARS_BACKGROUND:
					ms = atof(UGKS_string2charstr(rText));
					AnimationsManager.Animations[AnimationsManager.Animations.size()-1]->SetPeriod(ms);
				break;
			}
		break;
	case RADIO:
		switch (CharType)
		{
		case CHARS_CIRCLESHIP:
			defaultCShip->Radius = atof(UGKS_string2charstr(rText));
			break;
		}
		break;
	case SCORE:
		//Mark obtained by one of the top players at this game
		switch (SubTop())
		{
		case SCORES:
			//Assign the score to the current player and store it into the Scores Manager
			break;
		default:;
		}
		break;
	case SOUNDS:
		SoundsManager.SetTrackPath(rText);
		Game->Directories[CG_SOUNDS_DIR] = rText;
		break;
	case SPS:		//Shoots Per Second
		break;
	case TEXTURE:
			TexturesManager.SetFilesPath(rText);
			Game->Directories[CG_TEXTURE_DIR] = rText;
			//Load basic application textures.
			LoadTextures();
		break;
	case TEXTURE2D:
		switch (SubTop())
		{
		case CHARACTER:
			switch (CharType)
				{
				case CHARS_BONUS:
					defaultBonus->IndTexture2D = TexturesManager.CreateTexture(rText);
					 break;
				case CHARS_BRICK:
					defaultBrick->IndTexture2D = TexturesManager.CreateTexture(rText);
					break;
				case CHARS_BUNKER:
					defaultBunker->IndTexture2D = TexturesManager.CreateTexture(rText);
					 break;
				case CHARS_PLAYER:
					defaultPlayer->IndTexture2D = TexturesManager.CreateTexture(rText);
					break;
				case CHARS_SHIP:
					defaultShip->IndTexture2D = TexturesManager.CreateTexture(rText);
					 break;
				case CHARS_SUPPLYSHIP:
					defaultSShip->IndTexture2D = TexturesManager.CreateTexture(rText);
					 break;
				case CHARS_CIRCLESHIP:
					defaultCShip->IndTexture2D = TexturesManager.CreateTexture(rText);
					 break;
				case CHARS_BACKGROUND:
					break;
				default:;
				}
			break;
		case ANIMATION2D:
			switch (CharType)
			{
			case CHARS_BONUS:
				break;
			case CHARS_BRICK:
				break;
			case CHARS_BUNKER:
				break;
			case CHARS_PLAYER:
				TextureId = TexturesManager.CreateTexture(rText);
				AnimationsManager.Animations.back()->AddPhotogram(TexturesManager.Textures[TextureId]);
				break;
			case CHARS_SHIP:
				TextureId = TexturesManager.CreateTexture(rText);
				Texture = TexturesManager.Textures[TextureId];
				aniAux = AnimationsManager.Animations[AnimationsManager.Animations.size()-1];
				aniAux->AddPhotogram(Texture);
				break;
			case CHARS_SUPPLYSHIP:
				TextureId = TexturesManager.CreateTexture(rText);
				AnimationsManager.Animations.back()->AddPhotogram(TexturesManager.Textures[TextureId]);
				break;
			case CHARS_CIRCLESHIP:
				TextureId = TexturesManager.CreateTexture(rText);
				AnimationsManager.Animations.back()->AddPhotogram(TexturesManager.Textures[TextureId]);
				break;
			case CHARS_BACKGROUND:
				break;
			default:;
			}
			break;
		default:;
		}
		break;
	case TEXTURE3D:
		switch (CharType)
			{
			case CHARS_BONUS:
				 break;
			case CHARS_BRICK:
				 break;
			case CHARS_BUNKER:
				defaultBunker->IndTexture3D = TexturesManager.CreateTexture(rText);
				 break;
			case CHARS_PLAYER:
				defaultPlayer->IndTexture3D = TexturesManager.CreateTexture(rText);
				 break;
			case CHARS_SHIP:
				defaultShip->IndTexture3D = TexturesManager.CreateTexture(rText);
				 break;
			case CHARS_SUPPLYSHIP:
				defaultSShip->IndTexture3D = TexturesManager.CreateTexture(rText);
				 break;
			case CHARS_CIRCLESHIP:
				defaultCShip->IndTexture3D = TexturesManager.CreateTexture(rText);
				 break;
			case CHARS_BACKGROUND:
				Background.IndTexture3D = TexturesManager.CreateTexture(rText);
				break;
			default:;
			}
		break;
	case TITLE_D:		//HTML tag
		if (rText.compare(Title))
			{ErrorParser (CP_ParserMsgs[LOG_PARSER_NOT_THIS_VG]);
				bAbort = true;}
			else ;	//File content corresponds to the expected one for this videogame.
					//Go on. Nothing is done
		break;
	case TYPE_D:
		switch (SubTop())
		{
		case HEAD_D:
			if (rText.compare(Type))
			{
				FileTypeMismatch(rText, CIP_ParserMsgs[CIP_INITIALIZATION]);
				bAbort = true;
			}
		case CHARACTER:
			break;
		case ANIMATION2D:
			switch (CharType)
			{
			case CHARS_BONUS:
			case CHARS_BRICK:
			case CHARS_BUNKER:
			case CHARS_PLAYER:
			case CHARS_SHIP:
			case CHARS_SUPPLYSHIP:
			case CHARS_CIRCLESHIP:
			case CHARS_LASER:
			case CHARS_BACKGROUND:
				if (0 == rText.compare(CIP_Ani2DTypes[ANITYPE_CICLIC]))
					AnimationsManager.Animations[AnimationsManager.Animations.size() - 1]->Type = ANITYPE_CICLIC;
				else if (0 == rText.compare(CIP_Ani2DTypes[ANITYPE_PINGPONG]))
					AnimationsManager.Animations[AnimationsManager.Animations.size() - 1]->Type = ANITYPE_PINGPONG;
				else if (0 == rText.compare(CIP_Ani2DTypes[ANITYPE_SERIAL]))
					AnimationsManager.Animations[AnimationsManager.Animations.size() - 1]->Type = ANITYPE_SERIAL;
				break;
			}
			break;
		default:;
		}
		break;
	case VELOCITY:
		defaultCShip->Velocity = atof(UGKS_string2charstr(rText));
		break;
	case VERSION_D:	
			if (rText.compare(Version))
			{
				VersionMismatch (rText);
				bAbort = true;
			}
			else ;	//File version corresponds to the one from this videogame.
					//Go on. Nothing is done
		break;
	case X_IP:
		ChangeDimValue(XDIM, atof(UGKS_string2charstr(rText)));
		break;
	case Y_IP:
		ChangeDimValue(YDIM, atof(UGKS_string2charstr(rText)));
		break;
	case Z_IP:
		ChangeDimValue(ZDIM, atof(UGKS_string2charstr(rText)));
		break;
	default:;		//Tags not supported are not managed, even if they have content associated
	};
}

void CInitializationParser::Comment(const UGKS_String &rComment, DWORD dwAppData, bool &bAbort)
{
#ifdef CIP_DEBUG	//Class HTML Parser Debug activated
	WriteLog(LOG_PARSER_COMMENTS, rComment );
#endif

	UNUSED_ALWAYS(dwAppData);
	bAbort = false;
}

void CInitializationParser::EndParse(DWORD dwAppData, bool bIsAborted)
{
	UNUSED_ALWAYS(dwAppData);
#ifdef CIP_DEBUG	//Class HTML Parser Debug activated
	WriteLog(LOG_PARSER_END_PARSE, "");
	if (bIsAborted) LogFile << ". Parsing aborted" << UGKS_NEW_LINE_STRING;
#endif
}

/**
@fn void CInitializationParser::ChangeDimValue(SpaceCoords Dim, double Value)
@param SpaceCoords Dim	: Dimension where to apply the change
@param double Value		: New value to apply 
Depending on the context, the center, position, rotation, scale or speed is changed
*/
void CInitializationParser::ChangeDimValue(SpaceCoords Dim, double Value)
{
	CCharacter *GenericCharacter;

	switch (CharType)
	{
	case CHARS_BONUS:
		GenericCharacter = defaultBonus;
		break;
	case CHARS_BRICK:
		GenericCharacter = defaultBrick;
		break;
	case CHARS_BUNKER:
		GenericCharacter = defaultBunker;
		break;
	case CHARS_PLAYER:
		GenericCharacter = defaultPlayer;
		break;
	case CHARS_SHIP:
		GenericCharacter = defaultShip;
		break;
	case CHARS_SUPPLYSHIP:
		GenericCharacter = defaultSShip;
		break;
	case CHARS_CIRCLESHIP:
		GenericCharacter = defaultCShip;
		break;
	default:
		return;
	}

	switch (SubTop())
	{
	case CENTER:
		ChangeCenter(GenericCharacter, Dim, Value);
		break;
	case POSITION_IP:
		ChangePosition(GenericCharacter, Dim, Value);
		break;
	case ROTATION_IP:
		ChangeRotation(GenericCharacter, Dim, Value);
		break;
	case SCALE_IP:
		ChangeScale(GenericCharacter, Dim, Value);
		break;
	case SPEED:
		ChangeSpeed(GenericCharacter, Dim, Value);
		break;
	}
}

/**
@fn void CInitializationParser::ChangeCenter(SpaceCoords Dim, double Value);
@param SpaceCoords Dim	: Dimension where to apply the change
@param double Value		: New value to apply
Depending on the character being updated the dimension Dim of the Center is changed by the new Value
*/
void CInitializationParser::ChangeCenter(CCharacter *Character, SpaceCoords Dim, UGKALG_NUMERIC_TYPE Value)
{
	UGKS_String		msg;

	switch (CharType)
	{
	case CHARS_CIRCLESHIP:
		defaultCShip->Center.v[Dim] = Value;
		break;
	case CHARS_BONUS:
	case CHARS_BRICK:
	case CHARS_BUNKER:
	case CHARS_PLAYER:
	case CHARS_SHIP:
	case CHARS_SUPPLYSHIP:
	case CHARS_WEAPON:
		msg = "Cannot change the Center attribute of the generic character ";
		msg += CHARS_Token2Lexeme(CharType);
		ErrorParser(msg);
		break;
	default:
		msg = "Cannot change the Center attribute of a non-known character object";
		ErrorParser(msg);
	}
}