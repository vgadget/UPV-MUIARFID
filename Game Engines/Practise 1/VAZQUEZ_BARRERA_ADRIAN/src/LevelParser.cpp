/**	Class that loads the values of a given level of game 

	Prefix CLeP_: Class Level Parser

	@author Ramon Molla
	@version 2012-08
*/

#include <LevelParser.h>
#include <Player.h>
#include <Personaje.h>
#include <Navy.h>

#include <Ship.h>
#include <SupplyShip.h>
#include <CircleShip.h>
#include <Bunker.h>
#include <TextureAnimationManager.h>
#include <Global.h>

#include <UGKCharactersPool.h>
#include <SICharactersFactory.h>

#include <SIGame.h>
#include <iostream>

#define CLeP_TYPE "Level"


//Retrieves the lexeme from the token number
#define CLeP_Lexeme(T) CLeP_Tags[T-ACCELERATION_L]

extern CTextureAnimationManager	AnimationsManager;
extern CSIGame	*Game;				///<Singleton declaration to save the general configuration of the game
extern CNavy	*Navy;

extern BOOL		MainProgramLooping;

unsigned int	RowAvailable,			///<Pointer to the current available row
				ColumnAvailable;		///<Pointer to the current available column

GCHARS_CharacterType	CLeP_CharType;	//se define la variables Type de Caracter
UGKPHY_CINEMATIC_ACTION	CLeP_CinematicAction;
Vector					CLeP_CenterOfCoordinates; 
bool					AnimationExist=false;
UGKS_String				msgAux;

//Default objects that have to be parametrized while being loaded and inserted into the scene graph
CBunker*		currentBunker;
CShip*			Ship;
CSupplyShip*	SShip;
CCircleShip*	CCShip;
CPlayer*		Player;
CPersonaje* Personaje;

Vector	Pos,
		CellSize;

/**   
   @fn CLeP_Tags
   @param MAXTAGS_L maximum number of tags
   @param CLeP_TAG_MAX_LONG the maximum length of the labels
   An array of allowed tags for the analyzer
*/
UGKS_String CLeP_Tags[MAXTAGS_L - MAXTAGS_D] =
{
	"ACCELERATION",
	"ANIMATION2D",
	"ARMS",
	"BOUNCE",
	"BUNKER",
	"CENTER",
	"CHARACTER",
	"CONFIG",
	"DIR",
	"EPS",		//Energy Per Shoot
	"EXPLOSION",
	"FILE",
	"FROM",
	"FUEL",
	"GEOMETRY",
	"HAVE",		//Recursive definition of a character
	"HEALTH",	//How strong is the character. How much has to be hurt before dieing
	"ID",
	"INMORTAL",
	"LEVEL",
	"LINE",
	"LIVES",	//Amount of ships the Player has still before finishing the game
	"MESH",		///3D mesh assigned to a game character
	"MOVEDOWN",
	"MUSIC",
	"NAME",
	"NAVY",		///Enemy navy
	"NUMSS",
	"NUMLINES",
	"PERIODE",
	"POSITION",
	"RADIO",
	"REGENERATION",
	"RENDERMODE",
	"SCORES",
	"SCORE",
	"SHIPSAMOUNT",
	"SHOOT",		//Kind of shooting
	"SHOOTS",
	"SIZE",
	"SIMULATIONMODE",
	"SPEED",
	"SPS",			//Shoots Per Second
	"SUBTYPE",		//Only for SHIPS
	"TEXTURE",
	"TEXTURE2D",
	"TEXTURE3D",
	"TIMECIRCLESHIP",
	"TIMEPLAYER",
	"TIMERENDER",
	"TIMESHIP",
	"TIMESHOOTS",
	"TIMEBONUS",
	"TIMESUPPLYSHIP",
	"TIMEUPDATE",
	"TYPE",
	"UNKNOWN",	//This tag has no matching. It is for management purpouses only. 
	"VERSION",
	"WEIGHT",
	"X",
	"Y",
	"Z",
	"VELOCITY"
};

UGKS_String CLeP_LevelParserMsgs[LPERROR_MAX_LOGMSG] = 
{
	"Tag LINE not in the proper context. CHARACTER does not allow a LINE attribute.",
	"Only supported different kinds of Bunkers. No other character may have \"types\".",
	"Only a NAME or SCORE command may appear in a SCORES group.",
	"Only the attribute SPS or EPS may appear in a SHOOT group. Nothing more.",
	"Only the attribute NAME or FILE may appear in a group TEXTURE. Nothing more.",
	"Tag allowed only for SHIP.",
	"Amount of SupplyShips is lower than the minimum. Reset to 1.",
	"Amount of SupplyShips is higher than the maximum. Reset to 3."
};

UGKS_String CLeP_RenderModes[CHAR_MAX_RENDER_MODE] =
{
	"NO_RENDER",	// CHAR_NO_RENDER. No render for this character: camera, collision objects,...
	"2D",			// CHAR_2D
	"LINES3D",		// CHAR_LINES3D
	"3D"			// CHAR_3D
};

UGKS_String CLeP_SimulationModes[CG_MAX_SIMULATION_MODE] =
{
	"CONTINUOUS",
	"DISCRETE"
};

UGKS_String CLeP_Ani2DTypes[MAX_NUM_ANI_TYPES] = 
{
	"ANITYPE_CICLIC",
	"ANITYPE_PINGPONG",
	"ANITYPE_SERIAL"
};

/**
*	\defgroup	default	Default characters
*	 Used by the parsers while reading objects
*	 Default characters for initialization purpouses only
*    Updated when the initialization parser is running
*	 @{
*/
extern CPersonaje*	defaultPersonaje;
extern CPlayer*			defaultPlayer;
extern CShip*			defaultShip;
extern CSupplyShip*		defaultSShip;
extern CCircleShip*		defaultCShip;
extern CBrick*			defaultBrick;
extern CBunker*			currentBunker;

void GenerateBunkerLine(const UGKS_String &rText)
{
	unsigned int length;
	CB_BRICK_TYPE BrickType;

	UGKS_String		msg;
	unsigned int	i;

	if (CBNK_MAX_COLUMNS <= rText.length())
		length = CBNK_MAX_COLUMNS;
	else length = rText.length();

	for (i = 0; i<length; i++)
	{
		msg = rText[i];
		BrickType = (CB_BRICK_TYPE)atoi(UGKS_string2charstr(msg));
		currentBunker->Brick[RowAvailable][i] = *defaultBrick;
		currentBunker->Brick[RowAvailable][i].SubType = BrickType;
		if (CB_NO_BRICK == BrickType)
			currentBunker->Brick[RowAvailable][i].SetDefault();
		else
			currentBunker->Brick[RowAvailable][i].Activate();
	}

	///The rest of the bricks until reaching CBNK_MAX_COLUMNS have to be dead
	for (; i<CBNK_MAX_COLUMNS; i++)
		currentBunker->Brick[RowAvailable][i].SetDefault();

	Pos = currentBunker->Position;
	Pos.v[YDIM] += RowAvailable*CB_BRICK_LENGTH;

	for (i = 0; i < CBNK_MAX_COLUMNS; i++)
	{
		currentBunker->Brick[RowAvailable][i].MoveTo(Pos);
		currentBunker->Brick[RowAvailable][i].Row = RowAvailable;
		currentBunker->Brick[RowAvailable][i].Column = i;
		Pos.v[XDIM] += CB_BRICK_WIDTH;
	}

	RowAvailable--;
}

void GenerateNavyLine(const UGKS_String &rText)
{
	CS_SHIP_TYPE ShipType;
	CShip* Ship;

	UGKS_String		msg;
	unsigned int	i;

	Navy->Columns = rText.length();
	
	CellSize.v[XDIM] = Navy->AABB[CHAR_BBSIZE][XDIM].Value / Navy->Columns;
	CellSize.v[YDIM] = Navy->AABB[CHAR_BBSIZE][YDIM].Value / Navy->Rows;

	//Ship does not use 100% of one cell space. Only 64% (80%x80%)
	defaultShip->AABB[CHAR_BBSIZE][XDIM].Value = CellSize.v[XDIM] * 0.8f;
	defaultShip->AABB[CHAR_BBSIZE][YDIM].Value = CellSize.v[YDIM] * 0.8f;
	defaultShip->FitMeshIntoBoundingBox();

	Pos = Navy->Position;
	Pos.v[YDIM] += RowAvailable*CellSize.v[YDIM];

	msg = "0";	//Default type Ship, by default
	for (i = 0; i < Navy->Columns; i++)
	{
		msg = rText[i];
		ShipType = (CS_SHIP_TYPE)atoi(UGKS_string2charstr(msg));
		
		if (CS_NO_SHIP != ShipType)
		{
			Ship = (CShip*) CharacterPool->get(CHARS_SHIP, ShipType);
			*Ship = *defaultShip;

			switch (ShipType)
			{
			case CS_HIGH1:
				Ship->SubType = CS_HIGH1;
				Ship->IndAnimation2D = AnimationsManager.SearchIndOfName("ANI2SHIP_HIGH");
				break;
			case CS_HIGH2:
				Ship->SubType = CS_HIGH2;
				Ship->IndAnimation2D = AnimationsManager.SearchIndOfName("ANI2SHIP_HIGH");
				break;
			case CS_HIGH3:
				Ship->SubType = CS_HIGH3;
				Ship->IndAnimation2D = AnimationsManager.SearchIndOfName("ANI2SHIP_HIGH");
				break;
			case CS_MIDDLE:
				Ship->SubType = CS_MIDDLE;
				Ship->IndAnimation2D = AnimationsManager.SearchIndOfName("ANI2SHIP_MIDDLE");
				break;
			case CS_LOW:
				Ship->SubType = CS_LOW;
				Ship->IndAnimation2D = AnimationsManager.SearchIndOfName("ANI2SHIP_LOW");
				break;
			}

			Ship->Activate();
			Ship->AI = NULL;
			Ship->AI_Init();
			if (NULL != Ship->AI)
				Ship->OutEvent(CS_BORNING);

			Ship->MoveTo(Pos);
			Ship->Rotation.v.x = 90.0;
			Ship->Rotation.v.y = 0.0;
			Ship->Rotation.v.z = 0.0;
			Ship->Scale.v.x    = 0.1;
			Ship->Scale.v.y	   = 0.1;
			Ship->Scale.v.z    = 0.1;

			Pos.v[XDIM] += CellSize.v[XDIM];	//Go for next ship position
			SceneGraph.AddCharacter(Ship);
		}//if
	}
	RowAvailable--;
}


CLevelReader::CLevelReader()
{
	Type = CLeP_TYPE;	//Type of parser
}

/**   
   @fn Init
   Initialize Level
   @param UGKS_String FileName : file name
   Number starts at 0 bunkers    
   Starts the default values ??at the beginning of each level in the game
*/
void CLevelReader::Init(UGKS_String FileName)
{
	CHTMLParser::Init(FileName);
	if (!LogFile)	//NAME the file
		MainProgramLooping = false;
	ResetState();
};

/**   
   @fn BeginParse 
   @param DWORD dwAppData represents the open file
   @param bool bAbort is aborted if parser 
   This function starts the debugger parser
*/
void CLevelReader::BeginParse(DWORD dwAppData, bool &bAbort)
{
	UNUSED_ALWAYS(dwAppData);
	bAbort = false;
#ifdef CLeP_DEBUG	//Class HTML Parser Debug activated
	WriteLog(LOG_PARSER_BEGIN_PARSE, " . Level" );
#endif

	ResetState();

	//Initialize default AI
	defaultShip->AI = AIManager.GetAI(SIAI_SHIP_DEFAULT);
	defaultSShip->AI = AIManager.GetAI(SIAI_SUPPLYSHIP_DEFAULT);
	defaultCShip->AI = AIManager.GetAI(SIAI_CIRCLESHIP_DEFAULT);
}

/**   
   @fn StartTag
   @param CLiteHTMLTag pTag represents HTML tags
   @param DWORD dwAppData represents the open file
   @param bool bAbort represents whether the parser is aborted    
   This function defines the HTML tags permitted in the matrix of the analyzer and classified by categories or groups
*/
void CLevelReader::StartTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort)
{
	UGKS_String TagString, AuxString;
	unsigned int Tag;

	TagString = UGKS_CString2string(pTag->getTagName());

#ifdef CLeP_DEBUG	//Class HTML Parser Debug activated
	WriteLog(LOG_PARSER_START_TAG,TagString);
#endif

	UNUSED_ALWAYS(dwAppData);
	bAbort = false;

	Tag = Lexeme2Token(TagString);
	switch(Top())
	{case NIL_D://Starting and ending states
				if (HTML_D == Tag)
				{//It is a HTML file. Change to HTML state
					Push(HTML_D);
					//Do some initializations
					CLeP_CinematicAction	= UGKPHY_TRASLATION;
					CLeP_CharType = (GCHARS_CharacterType) UGK_CHARACTER_UNKNOWN;
					CLeP_CenterOfCoordinates.Reset();
				}
				else ErrorParser(CP_ParserMsgs[LOG_PARSER_NOT_HTML_TAG]);
		break;
	case ANIMATION2D_L:					
					switch (Tag)
					{
					case NAME_L:
					case PERIODE_L:
					case TEXTURE2D_L:
					case TYPE_L:
						Push(Tag);
						break;
					}
		break; 
	 case BODY_D:	//HTML tag
					switch (Tag)
					{
					case BUNKER_L:
					case DIR_L:
					case NUMSS_L:
					case CHARACTER_L:
					case REGENERATION_L:
					case SCORES_L: Push(Tag); //Change to the state specified by the Tag
						break;
					default: 
						TagNotAllowed(TagString, DHTML_TagsArray[BODY_D]); 
					}
		break;
	 case CENTER_L:
			switch (Tag)
			{
				case X_L:
				case Y_L:
				case Z_L:
					Push(Tag);
					CLeP_CinematicAction = UGKPHY_CENTER;
					break;
				default: 
					TagNotAllowed(TagString, CLeP_Lexeme(POSITION_L));
			}
		break;

	case CHARACTER_L:
					switch (Tag)
					{
					case CENTER_L:
						CLeP_CinematicAction = UGKPHY_CENTER;
						Push(Tag); //Change to the state specified by the Tag
						break;
					case LINE_L:
						///This may refers to a Bunker type or a Navy type
						if (CHARS_BUNKER != CLeP_CharType &&  CHARS_NAVY != CLeP_CharType)
							ErrorParser(CLeP_LevelParserMsgs[LPERROR_NOT_LINE_IN_CHARACTER]);
						Push(Tag); //Change to the state specified by the Tag
						break;
					case POSITION_L:
						CLeP_CinematicAction = UGKPHY_TRASLATION;
						Push(Tag); //Change to the state specified by the Tag
						break;
					case SIZE_L:
						CLeP_CinematicAction = UGKPHY_SIZE;
						Push(Tag); //Change to the state specified by the Tag
						break;
					case SPEED_L:
						CLeP_CinematicAction = UGKPHY_SPEED;
						Push(Tag); //Change to the state specified by the Tag
						break;
					case ACCELERATION_L:
					case ANIMATION2D_L:
					case BOUNCE_L:
					case EXPLOSION_L:
					case FROM_L:
					case LIVES_L:
					case WEIGHT_L:
					case FUEL_L:
					case ARMS_L:
					case INMORTAL_L:
					case HEALTH_L:
					case HAVE_L:
					case MESH_L:
					case MOVEDOWN_L:
					case NAME_L:
					case NUMLINES_L:
					case RADIO_L:
					case RENDERMODE_L:
					case SHIPSAMOUNT_L:
					case SHOOT_L:
					case SHOOTS_L:
					case SIMULATIONMODE_L:
					case SUBTYPE_L:
					case TEXTURE_L:
					case TEXTURE2D_L:
					case TEXTURE3D_L:
					case TIMECIRCLESHIP_L:
					case TIMESHIP_L:
					case TIMESHOOTS_L:
					case TIMEBONUS_L:
					case TIMESUPPLYSHIP_L:
					case TIMEPLAYER_L:
					case TIMERENDER_L:
					case TIMEUPDATE_L:
					case TYPE_L:
					case VELOCITY_L:
						Push(Tag); //Change to the state specified by the Tag
						break;
					default:
						TagNotAllowed(TagString, CLeP_Lexeme(CHARACTER_L)); 
					}
					
		break;
	case DIR_L:
					switch (Tag)
					{
					case MUSIC_L:
					case TEXTURE_L:
					case GEOMETRY_L: Push(Tag); //Change to the state specified by the Tag
						break;
					default:
						TagNotAllowed(TagString, CLeP_Lexeme(DIR_L)); 
					}
		break;
	case FROM_L:
				switch (Tag)
				{
					case X_L:
					case Y_L:
					case Z_L: 
							Push(Tag);
							CLeP_CinematicAction = UGKPHY_TRASLATION;
							break;
					default:
						TagNotAllowed(TagString, CLeP_Lexeme(FROM_L));
				}
		break;
	case HAVE_L:
				switch (Tag)
				{
					case EXPLOSION_L:
					case LIVES_L:
					case HEALTH_L:
					case POSITION_L:
					case SPEED_L:
					case MESH_L:
					case TEXTURE_L:
					case TYPE_L:
					case SHOOT_L:
						Push(Tag); //Change to the state specified by the Tag
						break;
					default:
						TagNotAllowed(TagString, CLeP_Lexeme(HAVE_L));
				}
		 break;
	case HEAD_D:		//HTML tag
					switch (Tag)
					{
					case TYPE_L:
					case TITLE_D:
					case VERSION_L:
					case LEVEL_L:
					case SCORE_L:  // ADDED THE SCORE
						Push(Tag); //Change to the state specified by the Tag
						break;
					default:
						TagNotAllowed(TagString, DHTML_TagsArray[HEAD_D]);
					}
		break;
	case HEALTH_L:
					Push(Tag); //Change to the state specified by the Tag
		break;
	case HTML_D:	//HTML Tag
					//The parser is at the very beginning of the iniitalization file
					switch (Tag)
					{
					case HEAD_D:		//A HEAD Tag is expected
					case BODY_D: 
						Push(Tag); //Change to the state specified by the Tag
						break;
					default: 
							ErrorParser(CP_ParserMsgs[LOG_PARSER_ONLY_HEAD_OR_BODY]);
							TagError(CLeP_Tags[Tag]);
					}
		break;
	case NAME_L:
		Push(Tag); //Change to the state specified by the Tag
		break;
	case POSITION_L:
				switch (Tag)
				{
					case X_L:
					case Y_L:
					case Z_L: 
							Push(Tag);
							CLeP_CinematicAction = UGKPHY_TRASLATION;
							break;
					default:
						TagNotAllowed(TagString, CLeP_Lexeme(POSITION_L));
				}
		break;
	case SCORES_L:	//A NAME Tag is expected
					if (NAME_L == Tag ||SCORE_L == Tag)
						Push(Tag);
					else ErrorParser(CLeP_LevelParserMsgs[LPERROR_ONLY_NAME_OR_SCORE]);
		break;
	case SHOOT_L:		//Kind of shooting
					switch (Tag)
					{
					case SPS_L:	//Change to SPS state
					case EPS_L:	//May be that no SPS tag appears or that the SPS section is finished.
								//There is a EPS part also
						Push(Tag); //Change to the state specified by the Tag
						break;
					default: ErrorParser(CLeP_LevelParserMsgs[LPERROR_ONLY_EPS_OR_SPS]);
					}
		break;
	case SIZE_L:
				switch (Tag)
				{
					case X_L:
					case Y_L:
					case Z_L:
							Push(Tag);
							CLeP_CinematicAction = UGKPHY_SIZE;
							break;
					default: 
						TagNotAllowed(TagString, CLeP_Lexeme(SIZE_L));
				}
		break;
	case SPEED_L:
				switch (Tag)
				{
					case X_L:
					case Y_L:
					case Z_L:
							Push(Tag);
							CLeP_CinematicAction = UGKPHY_SPEED;
							break;
					default:
						TagNotAllowed(TagString, CLeP_Lexeme(POSITION_L));
				}
		break;
	case TEXTURE_L:
					if (NAME_L == Tag || FILE_L == Tag)
						Push(Tag);
					else ErrorParser(CLeP_LevelParserMsgs[LPERROR_ONLY_NAME_OR_FILE]);
		break;
	default:;
	};
}

/**   
   @fn EndTag
   @param CLiteHTMLTag pTag represents HTML tags
   @param DWORD dwAppData represents the open file
   @param bool bAbort represents whether the parser is aborted    
   This function defines the end of a label acuardo permitted its category, If not shown an error message
*/
void CLevelReader::EndTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort)
{
	UGKS_String			TagString, msg;
	unsigned int	Tag;
	bool			Error = false;

	TagString = UGKS_CString2string(pTag->getTagName());

	#ifdef CLeP_DEBUG	//Class HTML Parser Debug activated
		WriteLog(LOG_PARSER_END_TAG_END, TagString );
	#endif
	//printf("Etiqueta fin: %s\n", pTag->getTagName());
	UNUSED_ALWAYS(dwAppData);
	bAbort = false;

	Tag = Lexeme2Token(TagString);
	switch(Top())
	{
	 case ACCELERATION_L:
	 case BODY_D:
	 case BOUNCE_L:
	 case BUNKER_L:
	 case CENTER_L:
	 case CHARACTER_L:
	 case DIR_L:
	 case EPS_L:		//Energy Per Shoot
	 case EXPLOSION_L:
	 case FROM_L:
	 case FILE_L:
	 case GEOMETRY_L:
	 case HAVE_L:		//Recursive definition of a character
	 case HEAD_D:	
	 case WEIGHT_L:
	 case FUEL_L:
	 case ARMS_L:
	 case INMORTAL_L:
	 case HEALTH_L:		//How strong is the character. How much has to be hurt before dieing
	 case HTML_D:	//The parser is at the very end of the initialization file					
	 case ID_L:
	 case LEVEL_L:
	 case LINE_L:
	 case LIVES_L:
	 case MESH_L:
	 case MOVEDOWN_L:
	 case MUSIC_L:
	 case NAME_L:
	 case NUMSS_L:
	 case NUMLINES_L:
	 case PERIODE_L:
	 case POSITION_L:
	 case RADIO_L:
	 case REGENERATION_L:
	 case RENDERMODE_L:
	 case SCORES_L:
	 case SCORE_L:
	 case SHIPSAMOUNT_L:
	 case SHOOT_L:		
	 case SHOOTS_L:
	 case SIZE_L:
	 case SIMULATIONMODE_L:
	 case SPEED_L:
	 case SPS_L:		//Shoots Per Second
	 case SUBTYPE_L:
	 case TEXTURE_L:
	 case TEXTURE2D_L:
	 case TEXTURE3D_L:
	 case TIMECIRCLESHIP_L:
	 case TIMESHIP_L:
	 case TIMESHOOTS_L:
	 case TIMEBONUS_L:
	 case TIMESUPPLYSHIP_L:
	 case TIMEPLAYER_L:
	 case TIMERENDER_L:
	 case TIMEUPDATE_L:
	 case TITLE_D:		
	 case TYPE_L:
	 case VERSION_L:
	 case VELOCITY_L:
	 case X_L:			//Pos. X
	 case Y_L:			//Pos. Y
	 case Z_L:
		if (Top() == Tag) Pop();	//Change to the previous state
		else Error = true;
		break;
	 case ANIMATION2D_L:
					switch (CLeP_CharType)
					{
					case CHARS_BONUS:
							break;
					case CHARS_BRICK:
							break;
					case CHARS_BUNKER:
							break;
					case CHARS_PLAYER:
						if(!AnimationExist)
							Players[CurrentPlayer]->IndAnimation2D = AnimationsManager.Animations.size()-1;
						else
							Players[CurrentPlayer]->IndAnimation2D = AnimationsManager.SearchIndOfName(msgAux);
							break;
					case CHARS_SHIP:
						//ACHTUNG: PENDIENTE DE CREAR SHIPS DESDE MATRIZ DE LEVEL
						/*if(!AnimationExist)
							Navy->Ship[i].IndAnimation2D = AnimationsManager.Animations.size()-1;
						else
							Navy->Ship[i].IndAnimation2D = AnimationsManager.SearchIndOfName(msgAux);*/
							break;
					case CHARS_SUPPLYSHIP:
						if(!AnimationExist)
							SShip->IndAnimation2D = AnimationsManager.Animations.size()-1;
						else
							SShip->IndAnimation2D = AnimationsManager.SearchIndOfName(msgAux);
							break;
					case CHARS_CIRCLESHIP:
						if(!AnimationExist)
							CCShip->IndAnimation2D = AnimationsManager.Animations.size()-1;
						else
							CCShip->IndAnimation2D = AnimationsManager.SearchIndOfName(msgAux);
							break;
						break;
					case CHARS_LASER:
						if(!AnimationExist)
							for(int i=0;i<CPL_MAX_LASERS;i++)
								Players[CurrentPlayer]->Laser[i].IndAnimation2D = AnimationsManager.Animations.size()-1;
						else
							for(int i=0;i<CPL_MAX_LASERS;i++)
								Players[CurrentPlayer]->Laser[i].IndAnimation2D = AnimationsManager.SearchIndOfName(msgAux);
							break;
					case CHARS_BACKGROUND:
						if(!AnimationExist)
							Background.IndAnimation2D = AnimationsManager.Animations.size()-1;
						else
							Background.IndAnimation2D = AnimationsManager.SearchIndOfName(msgAux);
						break;
					}
					AnimationExist=false;
					Pop();	//Change to the previous state
		 break;
	 case NIL_D:
	 case MAXTAGS_L:	//Closing a label when all the lables are closed is an error
					msg  = TagString;
					msg += CP_ParserMsgs[LOG_PARSER_END_TAG_MAXTAGS];
					ErrorParser(msg);
		break;
	 case UNKNOWN_L:	//This tag is for management purpouses only. There is no correspondence to any string label in the HTML file
					msg  = TagString;
					msg += CP_ParserMsgs[LOG_PARSER_END_TAG_UNK];
					ErrorParser(msg);
		break;
	default:;
	};	//Switch

	if (Error)
	{	msg  = TagString;
		msg += CP_ParserMsgs[LOG_PARSER_END_TAG_ERROR2];
		msg += CLeP_Tags[Top()];
		ErrorParser(msg);
	}
}

/**   
   @fn Characters
   @param UGKS_String &rText represents the text
   @param DWORD dwAppData represents the open file
   @param bool bAbort represents if the the parser is aborted
   This function defines the text for labels for the game   
   Otherwise it shows an error message
*/
void CLevelReader::Characters(const UGKS_String &rText, DWORD dwAppData, bool &bAbort)
{
UGKS_String		msg;
float			f;
int				intAux;
unsigned int	i;

CCharacter *C;

if (bAbort) return;

#ifdef CLeP_DEBUG	//Class HTML Parser Debug activated
	WriteLog(LOG_PARSER_TEXT, rText );
#endif

	UNUSED_ALWAYS(dwAppData);
	bAbort = false;

	switch(Top())
	{
	case ACCELERATION_L:
		Players[CurrentPlayer]->Acceleration.v[XDIM] = atof(UGKS_string2charstr(rText));
		break;
	case BOUNCE_L:
		if (atoi(UGKS_string2charstr(rText)))
			Game->BouncingMode = true;
		else
			Game->BouncingMode = false;
		break;
	case EPS_L:		//Energy Per Shoot
			f = atof(UGKS_string2charstr(rText));
		break;
	case EXPLOSION_L:
		f =  atof(UGKS_string2charstr(rText));
					switch (CLeP_CharType)
					{case CHARS_BONUS:
						 ///There is no possible explosion for the bonuses. No action performed
						break;
					case CHARS_BUNKER:
						currentBunker->Hit_duration = f;
						break;
					 case CHARS_PLAYER:
						 Players[CurrentPlayer]->Hit_duration = f;
						 break;
					 case CHARS_SHIP:						
						Ship->Hit_duration = f;
						break;					
					 case CHARS_SUPPLYSHIP:
						 SShip->Hit_duration = f;
						 break;
					 case CHARS_CIRCLESHIP:
						 CCShip->Hit_duration = f;
						 break;
					 case CHARS_WEAPON:
						///No action possible for this game version
					 default:;
					}
		break;
	case FILE_L:
			//TO DO. Insert here the file name of the texture into the current character texture file	
		break;
	case GEOMETRY_L:
			msg = rText;
			MeshesManager.SetFilesPath(UGKS_string2CString(msg));
		break;

	case WEIGHT_L:
		intAux = atoi(UGKS_string2charstr(rText)); //Health of this given character
		switch (CLeP_CharType)
		{
		case CHARS_PERSONAJE:
			Personaje->Weight = intAux;
			break;
		}
		break;

	case FUEL_L:
		intAux = atoi(UGKS_string2charstr(rText)); //Health of this given character
		switch (CLeP_CharType)
		{
		case CHARS_PERSONAJE:
			Personaje->Fuel = intAux;
			break;
		}
		break;

	case ARMS_L:
		intAux = atoi(UGKS_string2charstr(rText)); //Health of this given character
		switch (CLeP_CharType)
		{
			case CHARS_PERSONAJE:
				Personaje->Arms = intAux;
				break;
		}
		break;

	case INMORTAL_L:
		intAux = atoi(UGKS_string2charstr(rText)); //Health of this given character
		switch (CLeP_CharType)
		{
		case CHARS_PERSONAJE:
			Personaje->Inmortal = intAux;
			break;
		}
		break;

	case HEALTH_L:	//How strong is the character. How much has to be hurt before dieing
					//It is supposed that this attribute is assigned to the current character
					intAux =  atoi(UGKS_string2charstr(rText)); //Health of this given character
					switch (CLeP_CharType)
					{
					case CHARS_BUNKER:
						 if(intAux>0)
							currentBunker->Health = currentBunker->MaxHealth = intAux;
						break;
					case CHARS_PERSONAJE:
						Personaje->Health = intAux;
						break;
					case CHARS_PLAYER:
						 if(intAux>0)
								Players[CurrentPlayer]->Health = Players[CurrentPlayer]->MaxHealth = intAux;
						 else	if (intAux == CHAR_HEALTH_INFINITE)
								Players[CurrentPlayer]->Health = Players[CurrentPlayer]->MaxHealth = CHAR_HEALTH_INFINITE;
						 break;
					 case CHARS_SHIP:
						 if(intAux > 0)
							 Ship->Health = Ship->MaxHealth = intAux;						 
						 else if (intAux == CHAR_HEALTH_INFINITE)
							 Ship->Health = Ship->MaxHealth = CHAR_HEALTH_INFINITE;
						 break;
					 case CHARS_SUPPLYSHIP:
						 if(intAux>0)
							SShip->Health = SShip->MaxHealth = intAux;
						 else if (intAux == CHAR_HEALTH_INFINITE)
								SShip->Health = SShip->MaxHealth = CHAR_HEALTH_INFINITE;
						 break;
					 case CHARS_CIRCLESHIP:
						 if(intAux>0)
							 CCShip->Health = CCShip->MaxHealth = intAux;
						 else if(intAux==CHAR_HEALTH_INFINITE)
							 CCShip->Health = CCShip->MaxHealth = CHAR_HEALTH_INFINITE;
						 break;
					 case CHARS_BONUS:
					 case CHARS_WEAPON:
					 default:;
					}
		break;
	case ID_L:
		///Not useful in this version
		break;
	case LEVEL_L:
		Game->Level =  atoi(UGKS_string2charstr(rText)); 
		break;
	case LINE_L:
		///If line does not refer to a Bunker or a Navy, there is nothing to do
		switch (CLeP_CharType)
		{
		case CHARS_BUNKER:
			GenerateBunkerLine(rText);
			break;
		case CHARS_NAVY:
			GenerateNavyLine(rText);
			break;
		}
		break;
	case LIVES_L:		//Amount of ships the Player has still before finishing the game
			Players[CurrentPlayer]->Lives =  atoi(UGKS_string2charstr(rText));
		break;
	case MESH_L:
			switch (CLeP_CharType)
			{
				case CHARS_NAVY:
					Navy->SetMeshName(rText);	///Ships mesh default
					break;
				case CHARS_SUPPLYSHIP:
					msg = rText;
					if (msg.compare(defaultSShip->GetMeshName()))
					{		//return 0 if strings are equal
						int ind = MeshesManager.AddModel(msg);
						SShip->IndMesh	= ind;
						SShip->Mesh		= MeshesManager.GetMesh(SShip->IndMesh);
						SShip->SetMeshName(SShip->Mesh->GetFileName());
					}
					else
					{
						SShip->IndMesh		= defaultSShip->IndMesh;
						SShip->Mesh		= defaultSShip->Mesh;
						SShip->SetMeshName(defaultSShip->GetMeshName());
					}
					break;
				case CHARS_SHIP:
					msg = rText;
					break;
				case CHARS_CIRCLESHIP:
					msg = rText;
					if (msg.compare(defaultCShip->GetMeshName()))
					{		//return 0 if strings are equal
						int ind = MeshesManager.AddModel(msg);
						CCShip->IndMesh		= ind;
						CCShip->Mesh		= MeshesManager.GetMesh(CCShip->IndMesh);
						CCShip->SetMeshName(CCShip->Mesh->GetFileName());
					}
					else
					{
						CCShip->IndMesh		= defaultCShip->IndMesh;
						CCShip->Mesh		= defaultCShip->Mesh;
						CCShip->SetMeshName(defaultCShip->GetMeshName());
					}
					break;
				case CHARS_PLAYER:
					msg = rText;
					if (msg.compare(defaultPlayer->GetMeshName())){		//return 0 if strings are equal
						int ind = MeshesManager.AddModel(msg);
						Players[CurrentPlayer]->IndMesh	= ind;
						Players[CurrentPlayer]->Mesh		= MeshesManager.GetMesh(Players[CurrentPlayer]->IndMesh);
						Players[CurrentPlayer]->SetMeshName(Players[CurrentPlayer]->Mesh->GetFileName());
					}
					else
					{
						Players[CurrentPlayer]->IndMesh	= defaultPlayer->IndMesh;
						Players[CurrentPlayer]->Mesh		= defaultPlayer->Mesh;
						Players[CurrentPlayer]->SetMeshName(defaultPlayer->GetMeshName());
					}			
					break;
				case CHARS_LASER:
					msg = rText;
					if (msg.compare(defaultPlayer->Laser[CPL_LEFT_LASER].GetMeshName()))
					{		//return 0 if strings are equal
						int ind = MeshesManager.AddModel(msg);
						Players[CurrentPlayer]->Laser[CPL_LEFT_LASER].IndMesh	= ind;
						Players[CurrentPlayer]->Laser[CPL_LEFT_LASER].Mesh		= MeshesManager.GetMesh(Players[CurrentPlayer]->Laser[CPL_LEFT_LASER].IndMesh);
						Players[CurrentPlayer]->Laser[CPL_LEFT_LASER].SetMeshName(Players[CurrentPlayer]->Laser[CPL_LEFT_LASER].Mesh->GetFileName());
				
						Players[CurrentPlayer]->Laser[CPL_RIGHT_LASER].IndMesh = MeshesManager.MeshesAmount() - 1;	//From 0 to size-1
						Players[CurrentPlayer]->Laser[CPL_RIGHT_LASER].Mesh		= MeshesManager.GetMesh(Players[CurrentPlayer]->Laser[CPL_RIGHT_LASER].IndMesh);
						Players[CurrentPlayer]->Laser[CPL_RIGHT_LASER].SetMeshName(Players[CurrentPlayer]->Laser[CPL_RIGHT_LASER].Mesh->GetFileName());
					}
					else
					{		//return 0 if strings are equal
						MeshesManager.AddModel(msg);
						Players[CurrentPlayer]->Laser[CPL_LEFT_LASER].IndMesh	= defaultPlayer->Laser[CPL_LEFT_LASER].IndMesh;
						Players[CurrentPlayer]->Laser[CPL_LEFT_LASER].Mesh		= defaultPlayer->Laser[CPL_LEFT_LASER].Mesh;
						Players[CurrentPlayer]->Laser[CPL_LEFT_LASER].SetMeshName(defaultPlayer->Laser[CPL_LEFT_LASER].Mesh->GetFileName());
				
						Players[CurrentPlayer]->Laser[CPL_RIGHT_LASER].IndMesh		= defaultPlayer->Laser[CPL_LEFT_LASER].IndMesh;
						Players[CurrentPlayer]->Laser[CPL_RIGHT_LASER].Mesh		= defaultPlayer->Laser[CPL_LEFT_LASER].Mesh;
						Players[CurrentPlayer]->Laser[CPL_RIGHT_LASER].SetMeshName(defaultPlayer->Laser[CPL_LEFT_LASER].Mesh->GetFileName());
					}
				
					break;
				case CHARS_BACKGROUND:
					msg = rText;
					if (msg.compare(Background.GetMeshName())){
						int ind = MeshesManager.AddModel(msg);
						Background.IndMesh	= ind;
						Background.Mesh		= MeshesManager.GetMesh(Background.IndMesh);
						Background.SetMeshName(Background.Mesh->GetFileName());
					}
					break;
				case CHARS_BONUS:
				case CHARS_WEAPON:
				default:;
			}
		break;
	case MOVEDOWN_L:
			Navy->ShipsMoveDown =  atoi(UGKS_string2charstr(rText));
		break;
	case MUSIC_L:
					Game->Directories[CG_MUSIC_DIR] = rText;
		break;
	/*	In this case, A new node for the Scene Graph is performed.
		Type, casting and dynamic memory allocation has to be done
	*/
	case NAME_L:
		switch(SubTop())
		{
		case ANIMATION2D_L:
				switch (CLeP_CharType)
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
						if(-1 == AnimationsManager.SearchIndOfName(msg))
							AnimationsManager.CreateAnimation(msg,(double) 0,(ANI_ANIMATION_TYPE)0);
						else{
							AnimationExist=true;
							msgAux = msg;
						}
					break;
				}
				break;
			case CHARACTER_L:
				msg = rText;
				CLeP_CharType = (GCHARS_CharacterType) CharacterFactory->Lexeme2Token(msg);
				switch (CLeP_CharType)
				{
					case CHARS_BONUS:
						///They are predefined by the program. No allowed yet in this version.
						break;
					case CHARS_BUNKER:
						///The bunkers are already just created by default. There is an array of bunkers. No action performed
						///Bricks distribution will be created later
						currentBunker = (CBunker*) CharacterPool->get(CHARS_BUNKER, UGKOBJM_NO_SUBTYPE);
						currentBunker->Activate();
						SceneGraph.AddCharacter(currentBunker);
#ifdef CHAR_USE_QUADTREE
						currentBunker->GetQTRoot()->AddBigCharacter(currentBunker);
#endif

						RowAvailable = CBNK_MAX_LINES-1; //One less since numeration begins from 0, not from 1
						ColumnAvailable = 0;	///< Pointer to the current available column. Reset the current line bricks to the initial column
						break;
					case CHARS_CIRCLESHIP:
						CCShip = (CCircleShip*) CharacterPool->get(CHARS_CIRCLESHIP, CCS_LEFTTYPE);
						*CCShip = *defaultCShip;
						CCShip->AI_Init();
						CCShip->OutEvent(CCS_BORNING); //v 0->1
						CCShip->Scale.v[XDIM] = 0.25;
						CCShip->Scale.v[YDIM] = 0.25;
						CCShip->Scale.v[ZDIM] = 0.25;

						CCShip->Rotation.v[XDIM] = 90.0;
						CCShip->Rotation.v[YDIM] = 0.0;
						CCShip->Rotation.v[ZDIM] = 0.0;
						
						SceneGraph.AddCharacter(CCShip);
						break;
					case CHARS_GAME:
						///Updated during playing time in 3D mode. Not available for specification during level loading 
						break;
					case CHARS_LASER:
						///Updated during playing time in 3D mode. Not available for specification during level loading 
						break;
					case CHARS_NAVY:
						Navy->Rows = 0;
						ColumnAvailable = 0;	///< Pointer to the current available column. Reset the current line bricks to the initial column
						RowAvailable = CN_MAX_ROWS - 1; //One less since numeration begins from 0, not from 1
						break;
					
					case CHARS_PERSONAJE:
						Personaje = (CPersonaje*) CharacterFactory->createNewCharacter(CHARS_PERSONAJE, UGKOBJM_NO_SUBTYPE);
						defaultPersonaje = Personaje;
						SceneGraph.AddCharacter(Personaje);

						break;
					case CHARS_PLAYER:
						//The Players are be defined in Global definitions. If it is not defined, a new player has to be created here.
						CurrentPlayer++;
						Players.resize(CurrentPlayer + 1);
						Players[CurrentPlayer] = (CPlayer*)	CharacterPool->get(CHARS_PLAYER, UGKOBJM_NO_SUBTYPE);
						*Players[CurrentPlayer] = *defaultPlayer;	//Initialization by default
						Players[CurrentPlayer]->State = CPL_BORN;

						Players[CurrentPlayer]->Scale.v[XDIM] = 0.1;
						Players[CurrentPlayer]->Scale.v[YDIM] = 0.1;
						Players[CurrentPlayer]->Scale.v[ZDIM] = 0.1;
						
						SceneGraph.AddCharacter(Players[CurrentPlayer]);
						break;
					case CHARS_SHIP:
						break;
					case CHARS_BACKGROUND:
						///Not available by the moment
						break;
					case CHARS_SUPPLYSHIP:
						//Create a new node to set inside the scene graph
						SShip = (CSupplyShip*) CharacterPool->get(CHARS_SUPPLYSHIP, CSS_NO_SUPPLY_SHIP);
						*SShip = *defaultSShip;
						SShip->AI_Init();
						SShip->OutEvent(CSS_BORNING);	//v 0->1

						SShip->Scale.v[XDIM] = 0.25;
						SShip->Scale.v[YDIM] = 0.25;
						SShip->Scale.v[ZDIM] = 0.25;						
						
						SShip->Rotation.v[XDIM] = 90.0;
						SShip->Rotation.v[YDIM] = 0.0;
						SShip->Rotation.v[ZDIM] = 0.0;
						
						SceneGraph.AddCharacter(SShip);
						break;
					case CHARS_WEAPON:
						///Not available by the moment
						break;
					case UGK_CHARACTER_UNKNOWN:
						msg = rText;
						TagError(msg);
						break;
					};
				break;
			case SCORES_L:
				break;
			case TEXTURE_L:
				break;
		};
		break;
	case NUMSS_L:
				Game->SSxLEV= atoi(UGKS_string2charstr(rText));
				if(Game->SSxLEV<1){
						msg  =	CLeP_LevelParserMsgs[LPERROR_MIN_SSHIP];
						msg += rText;
						Game->SSxLEV = 1;
				}
				if(Game->SSxLEV>3){
						msg  = CLeP_LevelParserMsgs[LPERROR_MAX_SSHIP];
						msg += rText;
						Game->SSxLEV = 3;
				}
		break;
	case NUMLINES_L:
		switch (CLeP_CharType)
			{
			case CHARS_NAVY:
				Navy->Rows = atoi(UGKS_string2charstr(rText));
				break;
			}
		break;
	case PERIODE_L:
		switch (SubTop())
		{
		case ANIMATION2D_L:
			switch (CLeP_CharType)
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
				if (!AnimationExist)
					AnimationsManager.Animations[AnimationsManager.Animations.size() - 1]->SetPeriod(atof(UGKS_string2charstr(rText)));
				else
					AnimationsManager.Animations[AnimationsManager.SearchIndOfName(msgAux)]->SetPeriod(atof(UGKS_string2charstr(rText)));
				break;
			}
		}
		break;
	case POSITION_L:
		break;
	case RADIO_L:
			switch (CLeP_CharType)
			{
			case CHARS_CIRCLESHIP:
				CCShip->Radius =  atof(UGKS_string2charstr(rText));
				break;
			}
		break;
	case RENDERMODE_L:
		if (rText == CLeP_RenderModes[CHAR_NO_RENDER]){
			Game->NextRenderMode = CHAR_NO_RENDER;
		}
		else if (rText == CLeP_RenderModes[CHAR_2D]){
			Game->NextRenderMode = CHAR_2D;
		}
		else if (rText == CLeP_RenderModes[CHAR_LINES3D]){
			Game->NextRenderMode = CHAR_LINES3D;
		}
		else if (rText == CLeP_RenderModes[CHAR_3D]){
			Game->NextRenderMode = CHAR_3D;
		}
		break;
	case REGENERATION_L:
			int M1;
			M1= atoi(UGKS_string2charstr(rText));
/*			if(M1==1)
				//Navy->ReGenShips=true;
			else
				//Navy->ReGenShips=false;
*/		break;
	case SCORE_L:
		Game->Score =  atoi(UGKS_string2charstr(rText));
		if(Game->Score > Game->HiScore) Game->HiScore = Game->Score;
		break;
	case SHIPSAMOUNT_L:
		//Navy->ShipsAmount= atoi(UGKS_string2charstr(rText));
		break;
	case SHOOTS_L:
			Navy->WithShoots =  atoi(UGKS_string2charstr(rText));
		break;
	case SIMULATIONMODE_L:
		Game->DiscreteSimulation = rText.compare(CLeP_SimulationModes[CG_CONTINUOUS_SIMUL]);
		break;
	case SPEED_L:
		break;
	case SPS_L:		//Shoots Per Second
		break;
	case SUBTYPE_L:
			CS_SHIP_TYPE SubType;
			SubType = (CS_SHIP_TYPE)atoi(UGKS_string2charstr(rText));
			switch (CLeP_CharType)
				{
					case CHARS_SHIP:
						Ship = (CShip*) Ship;
						Ship->SubType = SubType;
						break;
					default: 
						ErrorParser(CLeP_LevelParserMsgs[LPERROR_ONLY_FOR_SHIP]);
				};
		break;	
	case TEXTURE_L:
			Game->Directories[CG_TEXTURE_DIR] = rText;
		break;
	case TEXTURE2D_L:
		switch (SubTop())
		{
		case CHARACTER_L:
			switch (CLeP_CharType)
			{
			case CHARS_BONUS:
				break;
			case CHARS_BRICK:
				break;
			case CHARS_BUNKER:
				break;
			case CHARS_PLAYER:
				if (rText.compare(TexturesManager.Textures[defaultPlayer->IndTexture2D]->GetFileName()))
					Players[CurrentPlayer]->IndTexture2D = TexturesManager.CreateTexture(rText);
				break;
			case CHARS_SHIP:
				if (rText.compare(TexturesManager.Textures[defaultShip->IndTexture2D]->GetFileName()))
					Ship->IndTexture2D = TexturesManager.CreateTexture(rText);
				break;
			case CHARS_SUPPLYSHIP:
				if (rText.compare(TexturesManager.Textures[defaultSShip->IndTexture2D]->GetFileName()))
					SShip->IndTexture2D = TexturesManager.CreateTexture(rText);
				break;
			case CHARS_CIRCLESHIP:
				if (rText.compare(TexturesManager.Textures[defaultCShip->IndTexture2D]->GetFileName()))
					CCShip->IndTexture2D = TexturesManager.CreateTexture(rText);
				break;
			case CHARS_BACKGROUND:
				break;
			}
		case ANIMATION2D_L:
			switch (CLeP_CharType)
			{
			case CHARS_BONUS:
				break;
			case CHARS_BRICK:
				break;
			case CHARS_BUNKER:
				break;
			case CHARS_PLAYER:
				if (!AnimationExist){
					TexturesManager.CreateTexture(rText);
					AnimationsManager.Animations.back()->AddPhotogram(TexturesManager.Textures.back());
				}
				break;
			case CHARS_SHIP:
				if (!AnimationExist){
					TexturesManager.CreateTexture(rText);
					AnimationsManager.Animations.back()->AddPhotogram(TexturesManager.Textures.back());
				}
				break;
			case CHARS_SUPPLYSHIP:
				if (!AnimationExist){
					TexturesManager.CreateTexture(rText);
					AnimationsManager.Animations.back()->AddPhotogram(TexturesManager.Textures.back());
				}
				break;
			case CHARS_CIRCLESHIP:
				if (!AnimationExist){
					TexturesManager.CreateTexture(rText);
					AnimationsManager.Animations.back()->AddPhotogram(TexturesManager.Textures.back());
				}
				break;
			case CHARS_BACKGROUND:
				break;
			}
			break;
		}
		break;
	case TEXTURE3D_L:
		if (CHARACTER_L == SubTop())
			switch (CLeP_CharType)
				{
				case CHARS_BONUS:
					 break;
				case CHARS_BRICK:
					 break;
				case CHARS_BUNKER:
					 break;
				case CHARS_PLAYER:
					if (rText.compare(TexturesManager.Textures[defaultPlayer->IndTexture3D]->GetFileName()))
						Players[CurrentPlayer]->IndTexture3D = TexturesManager.CreateTexture(rText);
					 break;
				case CHARS_SHIP:
					if (rText.compare(TexturesManager.Textures[defaultShip->IndTexture3D]->GetFileName()))
						Ship->IndTexture3D = TexturesManager.CreateTexture(rText);
					 break;
				case CHARS_SUPPLYSHIP:
					if (rText.compare(TexturesManager.Textures[defaultSShip->IndTexture3D]->GetFileName()))
						SShip->IndTexture3D = TexturesManager.CreateTexture(rText);
					 break;
				case CHARS_CIRCLESHIP:
					if (rText.compare(TexturesManager.Textures[defaultCShip->IndTexture3D]->GetFileName()))
						CCShip->IndTexture3D = TexturesManager.CreateTexture(rText);
					 break;
				case CHARS_BACKGROUND:
					if (rText.compare(TexturesManager.Textures[Background.IndTexture3D]->GetFileName()))
						Background.IndTexture3D = TexturesManager.CreateTexture(rText);
					break;
				}
		break;
	case TIMEBONUS_L:
		Game->DefaultUpdPeriod[CHARS_BONUS] = atof(UGKS_string2charstr(rText));
		break;
	case TIMECIRCLESHIP_L:
		Game->DefaultUpdPeriod[CHARS_CIRCLESHIP] = atof(UGKS_string2charstr(rText));
		break;
	case TIMEPLAYER_L:
		Game->DefaultUpdPeriod[CHARS_PLAYER] = atof(UGKS_string2charstr(rText));
		break;
	case TIMERENDER_L:
		Game->DefaultRndPeriod[CHARS_GAME] = atof(UGKS_string2charstr(rText));
		break;
	case TIMESHIP_L:
		Game->DefaultUpdPeriod[CHARS_SHIP] = atof(UGKS_string2charstr(rText));
		Game->DefaultUpdPeriod[CHARS_NAVY] = atof(UGKS_string2charstr(rText));
		break;
	case TIMESHOOTS_L:
		Game->DefaultUpdPeriod[CHARS_SHOOT] = atof(UGKS_string2charstr(rText));
		Game->DefaultUpdPeriod[CHARS_SHOOT_MNGR] = atof(UGKS_string2charstr(rText));
		break;
	case TIMESUPPLYSHIP_L:
		Game->DefaultUpdPeriod[CHARS_SUPPLYSHIP] = atof(UGKS_string2charstr(rText));
		break;
	case TIMEUPDATE_L:
		Game->DefaultUpdPeriod[CHARS_GAME] = atof(UGKS_string2charstr(rText));
		break;
	case TITLE_D:	//HTML tag
		if (rText.compare(Game->GetTitle()/*Game title*/))
					{
						ErrorParser (CP_ParserMsgs[LOG_PARSER_NOT_THIS_VG]);
						bAbort = true;
					}
		break;
	case TYPE_L:
		switch (SubTop())
		{
		case ANIMATION2D_L:
			switch (CLeP_CharType)
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
				if(!AnimationExist){
					if (0 == rText.compare( CLeP_Ani2DTypes[ANITYPE_CICLIC]))
						AnimationsManager.Animations[AnimationsManager.Animations.size()-1]->Type = ANITYPE_CICLIC;
					if (0 == rText.compare( CLeP_Ani2DTypes[ANITYPE_PINGPONG]))
						AnimationsManager.Animations[AnimationsManager.Animations.size()-1]->Type = ANITYPE_PINGPONG;
					if (0 == rText.compare( CLeP_Ani2DTypes[ANITYPE_SERIAL]))
						AnimationsManager.Animations[AnimationsManager.Animations.size()-1]->Type = ANITYPE_SERIAL;
				}else{
					if (0 == rText.compare( CLeP_Ani2DTypes[ANITYPE_CICLIC]))
						AnimationsManager.Animations[AnimationsManager.SearchIndOfName(msgAux)]->Type = ANITYPE_CICLIC;
					if (0 == rText.compare( CLeP_Ani2DTypes[ANITYPE_PINGPONG]))
						AnimationsManager.Animations[AnimationsManager.SearchIndOfName(msgAux)]->Type = ANITYPE_PINGPONG;
					if (0 == rText.compare( CLeP_Ani2DTypes[ANITYPE_SERIAL]))
						AnimationsManager.Animations[AnimationsManager.SearchIndOfName(msgAux)]->Type = ANITYPE_SERIAL;
				}
				break;
			}
		break;
		case HEAD_D:
			if (rText.compare(Type))
			{
				FileTypeMismatch(rText, CLeP_Tags[LEVEL_L]);
				bAbort = true;
			}
			break;
		case CHARACTER_L:
			if (CBNK_TypeNames[CBNK_BLANK].compare(rText))
				if (CBNK_TypeNames[CBNK_HOME].compare(rText))
					if (CBNK_TypeNames[CBNK_USER_DEFINED].compare(rText))
					{
						MessageBox(HWND_DESKTOP, L"Bunker type not known. Creating a blank one by default", L"Error", MB_OK | MB_ICONEXCLAMATION);
						currentBunker->SubType = CBNK_BLANK;
					}
					else	currentBunker->SubType = CBNK_USER_DEFINED;
				else		currentBunker->SubType = CBNK_HOME;
			else			currentBunker->SubType = CBNK_BLANK;
			break;
		}
		break;
	case VELOCITY_L:
			CCShip->Velocity = atof(UGKS_string2charstr(rText));
			break;
	case VERSION_L:	if (rText.compare( Game->GetVersion()))
					{
						VersionMismatch(rText);
						bAbort = true;
					}
		break;
	case X_L:
		f =  atof(UGKS_string2charstr(rText));
		switch(CLeP_CinematicAction)
		{
		case UGKPHY_CENTER:
				switch (CLeP_CharType)
				{
				case CHARS_CIRCLESHIP:
					CCShip->Center.v[XDIM] = f;
				break;
				}
			break;
		case UGKPHY_TRASLATION:
				switch (CLeP_CharType)
				{
				 case CHARS_BUNKER:
					 currentBunker->TranslateBunker(Vector(f, FLT_MAX, FLT_MAX));
					 break;
				 case CHARS_NAVY:
					 Navy->Position.v[XDIM] = f;
					 break;

				 case CHARS_PERSONAJE:

					 Personaje->Position.v[XDIM] = f;

					 break;

				 case CHARS_PLAYER:
					 Players[CurrentPlayer]->MoveTo(f, Players[CurrentPlayer]->Position.v[YDIM], Players[CurrentPlayer]->Position.v[ZDIM]);
					 break;
				 case CHARS_SUPPLYSHIP:
					 SShip->MoveTo(f, SShip->Position.v[YDIM], SShip->Position.v[ZDIM]);
					 break;
				 case CHARS_BONUS:
				 case CHARS_SHIP:
					 Ship->MoveTo(f, Ship->Position.v[YDIM], Ship->Position.v[ZDIM]);
					 break;
				 case CHARS_WEAPON:
				 default:;
				}	//switch CLeP_CharType
			break;
		case UGKPHY_SPEED:
				switch (CLeP_CharType)
				{
				 case CHARS_BUNKER:
					 currentBunker->Speed.v[XDIM] = f;
					 break;
				 case CHARS_NAVY:
					 Navy->Speed.v[XDIM] = f;

					 for (i = 0; i < SceneGraph.Size(); i++)
					 {
						 C = SceneGraph.Object(i);
						 if (CHARS_SHIP == C->Type)
							 C->Speed.v[XDIM] = f;
					 }//For
					 defaultShip->Speed.v[XDIM] = f;
					 break;

				 case CHARS_PERSONAJE:
					 Personaje->Speed.v[XDIM] = f;
					 defaultPersonaje->Speed.v[XDIM] = f;
					 break;
				 case CHARS_PLAYER:
					 Players[CurrentPlayer]->Speed.v[XDIM] = f;
					 break;
				 case CHARS_SUPPLYSHIP:
					 SShip->Speed.v[XDIM] = f;
					 break;
				 case CHARS_BONUS:
				 case CHARS_SHIP:
					 Ship->Speed.v[XDIM] = f;
					 break;
				 case CHARS_WEAPON:
				 default:;
				}	//switch CLeP_CharType
		case UGKPHY_SIZE:
			switch (CLeP_CharType)
				{
				 case CHARS_SHIP:
					 Ship->AABB[CHAR_BBSIZE][XDIM].Value = f;
					 break;
				 default:;
				}	//switch CLeP_CharType
			}	//switch(CLeP_CinematicAction)
		break;
	case Y_L:
		f =  atof(UGKS_string2charstr(rText));
		switch(CLeP_CinematicAction)
		{
		case UGKPHY_CENTER:
				switch (CLeP_CharType)
				{
				case CHARS_CIRCLESHIP:
					CCShip->Center.v[YDIM] = f;
				break;
				}
			break;
		case UGKPHY_TRASLATION:
				switch (CLeP_CharType)
				{
				 case CHARS_BUNKER:
					 currentBunker->TranslateBunker(Vector(FLT_MAX,f,FLT_MAX));
					 break;
				 case CHARS_NAVY:
					 Navy->Position.v[YDIM] = f;
					 break;

				 case CHARS_PERSONAJE:
					 Personaje->Position.v[YDIM] = f;
					 break;

				 case CHARS_PLAYER:
					 Players[CurrentPlayer]->MoveTo(Players[CurrentPlayer]->Position.v[XDIM], f, Players[CurrentPlayer]->Position.v[ZDIM]);
					 break;
				 case CHARS_SUPPLYSHIP:
					 SShip->MoveTo(SShip->Position.v[XDIM], f, SShip->Position.v[ZDIM]);
					 break;
				 case CHARS_BONUS:
				 case CHARS_SHIP:
					 Ship->MoveTo(Ship->Position.v[XDIM], f, Ship->Position.v[ZDIM]);
					 break;
				 case CHARS_WEAPON:
				 default:;
				}	//switch CLeP_CharType
			break;
		case UGKPHY_SPEED:
				switch (CLeP_CharType)
				{
				 case CHARS_BUNKER:
					 currentBunker->Speed.v[YDIM] = f;
					 break;
				 case CHARS_NAVY:
					 Navy->Speed.v[YDIM] = f;
					 for (i = 0; i < SceneGraph.Size(); i++)
					 {
						 C = SceneGraph.Object(i);
						 if (CHARS_SHIP == C->Type)
							 C->Speed.v[YDIM] = f;
					 }//For
					 break;
				 case CHARS_PERSONAJE:
					 Personaje->Speed.v[YDIM] = f;
					 defaultPersonaje->Speed.v[YDIM] = f;
					 break;
				 case CHARS_PLAYER:
					 Players[CurrentPlayer]->Speed.v[YDIM] = f;
					 break;
				 case CHARS_SUPPLYSHIP: 
					 SShip->Speed.v[YDIM] = f;
					 break;
				 case CHARS_BONUS:
				 case CHARS_SHIP:
					 Ship->Speed.v[YDIM] = f;
					 break;
				 case CHARS_WEAPON:
				 default:;
				}
		case UGKPHY_SIZE:
			switch (CLeP_CharType)
				{
				 case CHARS_SHIP:
					 Ship->AABB[CHAR_BBSIZE][YDIM].Value = f;
					 break;
				 default:;
				}	//switch CLeP_CharType
		}	//switch(CLeP_CinematicAction)
		break;
	case Z_L:
		f =  atof(UGKS_string2charstr(rText));
		switch(CLeP_CinematicAction)
		{
			case UGKPHY_CENTER:
				switch (CLeP_CharType)
				{
				case CHARS_CIRCLESHIP:
					CCShip->Center.v[ZDIM] = f;
				break;
				}
			break;
		
		case UGKPHY_TRASLATION:
				switch (CLeP_CharType)
				{
				 case CHARS_BUNKER:
					 currentBunker->TranslateBunker(Vector(FLT_MAX,FLT_MAX,f));
					 break;
				 case CHARS_NAVY:
					 Navy->Position.v[ZDIM] = f;
					 break;
				 case CHARS_PERSONAJE:
					 Personaje->Position.v[ZDIM] = f;
					 break;
				 case CHARS_PLAYER:
					 Players[CurrentPlayer]->MoveTo(Players[CurrentPlayer]->Position.v[XDIM], Players[CurrentPlayer]->Position.v[YDIM], f);
					 break;
				 case CHARS_SUPPLYSHIP:
					 SShip->MoveTo(SShip->Position.v[XDIM], SShip->Position.v[YDIM], f);
					 break;
				 case CHARS_BONUS:
				 case CHARS_SHIP:
					 Ship->MoveTo(Ship->Position.v[XDIM], Ship->Position.v[YDIM], f);
					 break;
				 case CHARS_WEAPON:
				 default:;
				}	//switch CLeP_CharType
			break;
		case UGKPHY_SPEED:
				switch (CLeP_CharType)
				{
				 case CHARS_BUNKER:
					 currentBunker->Speed.v[ZDIM] = f;
					 break;
				 case CHARS_NAVY:
					 Navy->Speed.v[ZDIM] = f;
					 for (i = 0; i < SceneGraph.Size(); i++)
					 {
						 C = SceneGraph.Object(i);
						 if (CHARS_SHIP == C->Type)
							 C->Speed.v[ZDIM] = f;
					 }//For
					 break;

				 case CHARS_PERSONAJE:
					 Personaje->Speed.v[ZDIM] = f;
					 defaultPersonaje->Speed.v[ZDIM] = f;
					 break;
				 case CHARS_PLAYER:
					 Players[CurrentPlayer]->Speed.v[ZDIM] = f;
					 break;
				 case CHARS_SUPPLYSHIP:
					 SShip->Speed.v[ZDIM] = f;
					 break;
				 case CHARS_BONUS:
				 case CHARS_SHIP:
					 Ship->Speed.v[ZDIM] = f;
					 break;
				 case CHARS_WEAPON:
				 default:;
				}
		case UGKPHY_SIZE:
			switch (CLeP_CharType)
				{
				 case CHARS_SHIP:
					 Ship->AABB[CHAR_BBSIZE][ZDIM].Value = f;
					 break;
				 default:;
				}	//switch CLeP_CharType//switch CLeP_CharType
		}	//switch(CLeP_CinematicAction)
		break;
	//default:;		//Tags not supported are not managed, even if they have content associated
	};	//Switch
}

/**   
   @fn Comment
   @param UGKS_String &rComment represents the string of comments
   @param DWORD dwAppData represents the open file
   @param bool bAbort represents if the parser is aborted    
   This function defines Entries for tags
*/
void CLevelReader::Comment(const UGKS_String &rComment, DWORD dwAppData, bool &bAbort)
{

	#ifdef CLeP_DEBUG	//Class HTML Parser Debug activated
		WriteLog(LOG_PARSER_COMMENTS, rComment);
	#endif

	UNUSED_ALWAYS(dwAppData);
	bAbort = false;
}

/**   
   @fn EndParse
   @param DWORD dwAppData represents the open file
   @param bool bAbort represents if the parser is aborted  
   This function closes the file if the parser is aborted  
*/
void CLevelReader::EndParse(DWORD dwAppData, bool bIsAborted)
{
	UNUSED_ALWAYS(dwAppData);

	#ifdef CLeP_DEBUG	//Class HTML Parser Debug activated
		if (bIsAborted) WriteLog(LOG_PARSER_END_PARSE, "");
	#endif
};
