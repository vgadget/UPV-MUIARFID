/*	Class that loads the values of a given level of game 

	Prefix CLeP_: Class Level Parser

	@author Ramon Molla
	@version 2007-09-11
*/

#ifndef CLeP_LEVEL_PARSER_H
#define CLeP_LEVEL_PARSER_H

#include <UGKHTMLParser.h>

#define CLeP_DEBUG				//Class HTML Parser Debug activated
#define CLeP_MAXSTACK	  16	//Maximun deepness of the stack

typedef enum {
	LPERROR_NOT_LINE_IN_CHARACTER,
	LPERROR_ONLY_BUNKERS,
	LPERROR_ONLY_NAME_OR_SCORE,
	LPERROR_ONLY_EPS_OR_SPS,
	LPERROR_ONLY_NAME_OR_FILE,
	LPERROR_ONLY_FOR_SHIP,
	LPERROR_MIN_SSHIP,
	LPERROR_MAX_SSHIP,
	LPERROR_MAX_LOGMSG
} CLeP_LogMsg;

/**   
   It defines the types of labels that can match the scanner during the scan that exist in the game
   @param enum CLeP_TagType which lists the types of labels to be analyzed
*/
typedef enum {
	ACCELERATION_L = MAXTAGS_D,		//This tag is for management purpouses only. There is no correspondence to any string label in the HTML file
	ANIMATION2D_L,
	ARMS_L,
	BOUNCE_L,
	BUNKER_L,
	CENTER_L,
	CHARACTER_L,
	CONFIG_l,
	DIR_L,
	EPS_L,		//Energy Per Shoot
	EXPLOSION_L,
	FILE_L,
	FROM_L,
	FUEL_L,
	GEOMETRY_L,
	HAVE_L,		//Recursive definition of a character
	HEALTH_L,		//How strong is the character. How much has to be hurt before dieing
	ID_L,
	INMORTAL_L,
	LEVEL_L,
	LINE_L,
	LIVES_L,		//Amount of ships the Player has still before finishing the game
	MESH_L,
	MOVEDOWN_L,
	MUSIC_L,	
	NAME_L,
	NAVY_L,			//Enemy navy
	NUMSS_L,
	NUMLINES_L,
	PERIODE_L,
	POSITION_L,		//Posición específica de un personaje
	RADIO_L,
	REGENERATION_L, //Regeneración Sí/No
	RENDERMODE_L,
	SCORES_L,
	SCORE_L,
	SHIPSAMOUNT_L,
	SHOOT_L,		//Kind of shooting
	SHOOTS_L,		//Max. nº de disparos en el nivel.
	SIZE_L,
	SIMULATIONMODE_L,
	SPEED_L,
	SPS_L,			//Shoots Per Second
	SUBTYPE_L,		//Only for SHIPS
	TEXTURE_L,
	TEXTURE2D_L,
	TEXTURE3D_L,
	TIMECIRCLESHIP_L,
	TIMEPLAYER_L,
	TIMERENDER_L,
	TIMESHIP_L,
	TIMESHOOTS_L,
	TIMEBONUS_L,
	TIMESUPPLYSHIP_L,
	TIMEUPDATE_L,
	TYPE_L,
	UNKNOWN_L,		//This tag is for management purpouses only. There is no correspondence to any string label in the HTML file
	VERSION_L,
	WEIGHT_L,
	X_L,			//Pos. X
	Y_L,			//Pos. Y
	Z_L,			//Pos. Z
	VELOCITY_L,
	MAXTAGS_L		//This tag is for management purpouses only. There is no correspondence to any string label in the HTML file
} CLeP_TagType;

/* Class that loads the values by default in the very beginning of every level in the game
*/
/**   
   Reading class is defined with derivation Level or class inheritance ILiteHTMLReaderEvents    
   Where is the constructor and destructor in addition to starting the analysis file
   @param CE_EXPLOSION_TYPE subType predefined variable for the types of explosion
*/
using namespace UGK;

class CLevelReader : public CHTMLParser
{

private:
    std::ofstream	LogFile;
	
	void BeginParse(DWORD dwAppData, bool &bAbort); //Parsing begining
    void StartTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort); //Initial Tag
    void EndTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort); //Ending tag
    void Characters(const UGKS_String &rText, DWORD dwAppData, bool &bAbort); //Text containing information
    void Comment(const UGKS_String &rComment, DWORD dwAppData, bool &bAbort); //Comments
    void EndParse(DWORD dwAppData, bool bIsAborted); //Fin de  analisis
	
public:
	void Init(UGKS_String FileName);

	//Constructor and destructor
	CLevelReader();
};

extern UGKS_String CLeP_Tags[MAXTAGS_L - MAXTAGS_D];

#endif