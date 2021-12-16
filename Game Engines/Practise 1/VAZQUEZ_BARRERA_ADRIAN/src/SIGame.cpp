/**The place where resides the class game
*  General class to support all the attribute general to all the levels, players, ships,...
*  Prefix: CSIG_

*	@author Ramon Molla
*	@version 2016-01
*/

//Generic Reader and Parsers
#include <UGKDeviceParser.h>
#include <UGKLanguageParser.h>
#include <UGKKeyboardParser.h>
#include <FSMParser.h>

//Specific game dependent Reader and Parsers
#include <InitializationParser.h>
#include <LevelParser.h>
#include <UGKHTMLReader.h>

#include <AI/AIManager.h>
#include <BonusManager.h>	// Header File class Bonus Manager
#include <FontsManager.h>
#include <UGKMeshManager.h>
#include <ScoreManager.h>
#include <ShootsManager.h>	/// Header File class Manager for the shoots
#include <TexturesManager.h>/// Header File class Manager for the Textures.
#include <TextureAnimationManager.h>
#include <SICharactersFactory.h>

#include <Application.h>
#include <Navy.h>			// Header File class Navy-> Ships and supply ships that fight in the enemy navy

#include <Personaje.h>
#include <Player.h>
#include <Laser.h>
#include <Reactor.h>
#include <Ship.h>
#include <CircleShip.h>
#include <SupplyShip.h>
#include <Bunker.h>
#include <UGKCamera.h>			// Header File class Camera

#include <SIKeyboard.h>
#include <UGKGraphicsCard.h>

#include <SIGame.h>
#include <GUI.h>
#include <TextoGL.h>
#include <UGKLog.h>
#include <UGKglError.h>
#include <Scene.h>

#include <GameSounds.h>
#include <NetWork.h>
#include <TextResources.h>
#include <SITexturesResources.h>

#include <SIMessage.h>
#include <UGKSceneGraph.h>

//General headers
#include <glTexFont.h>
#include <glext.h>
#include <list>
#include <MMSystem.h>

extern CAIManager		AIManager;
extern CTextureAnimationManager	AnimationsManager;
extern CBonusManager	*BonusManager;
extern CFontsManager	FontsManager;
extern CMeshesManager	MeshesManager;
extern CScoresManager	ScoresManager;
extern CShootsManager	*ShootsManager;
extern CSoundsManager	SoundsManager;
extern CTexturesManager	TexturesManager;
extern CBunker			*defaultBunker;

extern CCamera			*Camera;				///<Singleton declaration of the game camera
extern CSICharactersFactory	*CharacterFactory;	///<Singleton declaration of the character factory that creates as many given characters as demanded
extern CLog				Log;			///<For logging performance, tracing,...
extern CNavy			*Navy;			///<Singleton to save the general configuration of the enemy Navy
extern CApplication		Application;
extern CScene			Scene;			///<Singleton to manage the Scene
extern CGUI				GUI;			///<Singleton declaration to render the general configuration of the game interface

extern CSIKeyboard		SIKeyboard;		///<Singleton to manage the application keyboard
extern CGraphicCard		GraphicsCard;	///<Singleton to manage the application Graphics Card

extern CCharacter		Background;
extern CPlayer			*defaultPlayer;
extern CPersonaje* Personaje;

extern CMsgFactory		MsgFactory;
extern RTDESK_CEngine	*RTDESK_Engine;

extern BOOL				MainProgramLooping;
extern CSceneGraph		SceneGraph;

//Internal local reader
CHTMLReader				HTMLReader;		//The real HTML reader. This is charge of every parsing whatever it is for
CCharactersPool			*CharacterPool;

UGKS_String CSIG_DefaultDirectories [CG_MAX_DIR] =
{
	"AI",
	"Configuration",
	"Languages",
	"Levels",
	"UserLevels",
	"Models",
	"Sounds",
	"Sounds",
	"Images",
	"Devices"
};

//Directories where to retrieve the information required by the video game
UGKS_String CSIG_PredefinedFiles[CSIG_MAX_PREDEF_FILES] =
{
	"Initialization",
	"Keyboard",
	"Devices",
	"ULevel",

	"FSM_game",
	"FSM_circleship",
	"FSM_supplyship",
	"FSM_ship"
};

//ACHTUNG: Since the Game class constructor uses the method SetDefaultDirectories 
//and it requires the array CSIG_DefaultDirectories to be set, it is MANDATORY to place the 
//declaration of the Game singleton AFTER the initialization of the array CSIG_DefaultDirectories 

CSIGame					*Game;			///<Singleton declaration to save the general configuration of the game

///Different name states for the FSMs that control the SS behaviour
UGKS_String CSIG_NameState[CSIG_MAXSTATE] = 
{
	"INITIAL",			///The game is in the initialization phase
	"FADING IN",		///The game is appearing from black while is reading the level information and updating internal data structures
	"FADING OUT PLAY",	///The game is Fading from the loading phase into the playing phase
	"PLAYING",			///The Player is playing now the game
	"SAVING",			///The Player is saving now the game
	"CHANGING TO 3D",
	"INTERMISSION",
	"LOST LIFE",		///The Player has lost one life
	"WON",				///The Player has won the game
	"LOST GAME",		///The Player has lost the game
	"FADING OUT WON",	///The game is Fading to black while releasing all data internal structures
	"FADING OUT END",	///The game is Fading to black while releasing all data internal structures
	"FADING OUT LOST",	///The game is Fading to black while releasing all data internal structures
	"PAUSED",			///The game stays on hold.
	"WAITING FOR KEY",	///The game waits till the player presses any key.
	"END"				///The game is Fading to black while releasing all data internal structures

};

///Sensitive string tags
UGKS_String CSIG_NameTransition[CSIG_MAXTRANSITION] = 
{
	"DEFAULT",			///<For management purpouses only
	"STARTING",			///<The game is just borning. Loading initialization data
	"LOADING",			///<Loading level to play
	"FADING_OUT",		///<Finishing the loading and going into the match
	"LOADED",			///<The current level has just being loaded
	"LIFE LOST",		///<One life has been lost
	"PLAY",				///<Go on playing
	"SAVE",				///<Go on saving
	"SAVED",			///<Game Saved
	"CHANGING_TO_3D",	///<When a CBN_BONUS_3D Bonus is touched, a change to 3D view mode is performed
	"3D_CHANGED",		///<The transition to 3D view mode is finished. Now, turn back to CSIG_PLAYING state
	"INTERMISS_ENDED",	///<The intermission has finished
	"FADING2WON",		///<The current level is going to being won
	"FADING2END",		///<The game is going to end
	"FADING2LOST",		///<The current level is going to being lost
	"FADED",			///<The current level has just been won or lost or the game has just ended
	"NEXT_LEVEL",		///<The level has been won and now a new level has to be loaded
	"UPDATE",			///<Event given to any state to evolve/update itself
	"PAUSE",			///<The player asks the game to pause
	"CONTINUE",			///<The player asks the game to continue
	"KEY_PRESSED",		///<The player pressed a key to start a new game or level.
	"END"
};

///Timers names
UGKS_String TimersName[SIGLBT_TIMER_AMOUNT] = {
									"Application",
									"RTDESK",
									"Update",
									"Render",
									"Idle",
									"System",
									"FPS"
									};

typedef enum {
	CSIG_SHOOTS, 
	CSIG_SAVED_GAME_DELETED,
	CSIG_DELETE_SAVED_GAME,
	CSIG_INIT_FILE,
	CSIG_UPDATE_GAME,
	CSIG_CLEARING_MANAGERS,
	CSIG_ENV_VBLES,
	CSIG_READING_NEXT_LEVEL,
	CSIG_LOAD_SIM_KERNEL,
	CSIG_LOAD_MESHES,
	CSIG_MESHES2CHARS,
	CSIG_MAX_LOG_MESSAGES
} CSIG_LOG_MESSAGES;

typedef enum {
	CSIG_FN_NULL,			///<Global non function

	CSIG_FN_CSS_MOVE,		///< Supply Ship "move" action
	CSIG_FN_CSS_DISPLAY,	///< Supply Ship "display" action
	CSIG_FN_CSS_HEALTH,		///< Supply Ship "healthing" action
	CSIG_FN_CSS_DIE,		///< Supply Ship "dying" action
	CSIG_FN_CSS_DEATH,		///< Supply Ship "death" action

	CSIG_FN_CS_INIT,		///< Ship "initialization" action
	CSIG_FN_CS_MOVE,		///< Ship "move" action
	CSIG_FN_CS_DISPLAY,		///< Ship "display" action
	CSIG_FN_CS_BURST,		///< Ship "burst" action
	CSIG_FN_CS_KILL,		///< Ship "kill" action
	
	CSIG_FN_CCS_MOVE,		///< Circle Ship "move" action
	CSIG_FN_CCS_DISPLAY,	///< Circle Ship "display" action
	CSIG_FN_CCS_HEALTH,		///< Circle Ship "healthing" action
	CSIG_FN_CCS_DIE,		///< Circle Ship "dying" action
	CSIG_FN_CCS_DEATH,		///< Circle Ship "death" action
	
	CSIG_MAX_FN
} CSIG_AI_FUNCTIONS;

///AI function names
UGKS_String AIFunctionName[CSIG_MAX_FN] = {
	"NULL",
	
	"CSS_FSM_Move",
	"CSS_display",
	"CSS_FSM_Healthing",
	"CSS_FSM_Dye",
	"CSS_FSM_Death",

	"init_ship",
	"CS_Move",
	"CS_Display",
	"CS_Bursting",
	"CS_Kill",

	"CCS_FSM_Move",
	"CCS_display",
	"CCS_FSM_Healthing",
	"CCS_FSM_Dye",
	"CCS_FSM_Death"
};
///Log messages
UGKS_String LogMessages[CSIG_MAX_LOG_MESSAGES] = {
	"Shoots...",
	"SAVED game successfully deleted",
	"Error deleting SAVED game",
	"Reading initialization file...",
	"Update Game",
	"Clearing application Managers...",
	"Environment variables...",
	"Reading next level of the game...",
	"Loading simulation kernel...",
	"Loading Meshes...",
	"Assigning Meshes to characters"
};

/**
*	Values always betweeen 0 (No sound) and 255 (Maximun power). One byte.
*/
USHORT CSIG_SoundVolume[CGS_MAX_SND] =
{
	0,		//For management purpouses only
	40,		//2D Shoot
	20,		//3D Shoot
	120,	//Explosion
	250,	//Applause
	200,	//Player Shield
	255,	//Changing from 2d mode to 3D
	255,	//Bonus
	200		//Key pressed
};


//External global variables
extern UGKS_String CGS_SoundsFiles[CGS_MAX_SND];

//Other local variables only for this file
Vector control_points[6];

cTextoGL txtFPS, txtRND, txtUPD, txtIDL, txtSUM, txtCP, txtRTDSKMM;

//2D Texts Colors
GLfloat red[3]={1.0f, 0.0f, 0.0f};
GLfloat gren[3]={0.0f, 1.0f, 0.0f};
GLfloat yelow[3]={1.0f, 1.0f, 0.0f};
GLfloat blue[3]={0.0f, 0.0f, 1.0f};
char texto[50];

CSIGame::CSIGame()
{
	Init();
}

CSIGame::~CSIGame()
{
}

void CSIGame::SetDefaultDirectories()
{
	for(unsigned int i = CG_AI_DIR; i<CG_MAX_DIR;i++)
		Directories[i] = CSIG_DefaultDirectories[i];
}

void CSIGame::Init(void)
{
	unsigned int i;

	//Game general attributes
	SetVersion	(CSIG_VERSION);	// CURRENT VERSION
	SetTitle	(CSIG_TITTLE);	// TITLE

	Log.Version = GetVersion();	// For debugging purpouses

	AI			= NULL;
	quadratic	= NULL;

	//Visualization
	RenderMode			= CHAR_2D;
	NextRenderMode		= CHAR_2D;

	Text_2D				= true;		// 2D TEXT AFFICHE ?
	Spectrum			= false;	// SPECTRUM (BEAT DETECTION)?
	Paused				= false;
	DiscreteSimulation	= false;
	NextSimulation		= RTDESKT_NOTIME;

	//Level control
	OnlyOneLevel		= false;
	StopAfterOneLevel	= false;
	Level				= 0;		// First LEVEL of the game
	LevelReaded			= false;	// Level reading not done
	ReadLevel			= true;		// Activate level reading by parser
	SSxLEV				= 1;		// Amount of Supply Ships per level...

	//Game control
	EndByTime				= false;
	EndByFrame				= false;
	BouncingMode			= false;
	WayToEnd				= CSIG_END_BY_FRAME;

	//Visualization control
	intermede_angle_total	= 0;	// INTERMEDE ANGLE TOTAL pour l'animation de l'Game->intermede
	active_splash			= false;// SPLASH SCREEN activé après l'intermède
	initialWidthW			= 600;
	initialHeightW			= 500;

	SetDefault();	// Field of the father class CCharacter 
	Type					= CHARS_GAME;// Field of the father class CCharacter

	//In miliseconds
	for (i = UGK_CHARACTER_UNKNOWN; i < CHARS_MAX_CHARTYPE; i++)
	{
		DefaultUpdPeriod[i] = SIGLBD_MIN_UPDATETIME_OBJECTS;
		DefaultRndPeriod[i] = SIGLBD_MAX_RENDERTIME;
	}
	
	GenericMsg[UMSG_RENDER]				= new cMsgGame;
	GenericMsg[UMSG_RENDER]->Type		= UMSG_MSG_GAME;
	GenericMsg[UMSG_RENDER]->SubType	= UMSG_RENDER;
	GenericMsg[UMSG_RENDER]->Propietary	= true;

	GenericMsg[UMSG_UPDATE]				= new cMsgGame;
	GenericMsg[UMSG_UPDATE]->Type		= UMSG_MSG_GAME;
	GenericMsg[UMSG_UPDATE]->SubType	= UMSG_UPDATE;
	GenericMsg[UMSG_UPDATE]->Propietary	= true;

	SetDefaultDirectories();

	Timer.resize(CSIG_MAX_TIMERS);
}

bool CSIGame::OutEvent(CSIG_GAME_TRANSITIONS EventName){ return AI->outEvent(CSIG_NameTransition[EventName], NULL, this); }
bool CSIGame::PlayerWon() { return !Navy->AliveShips(); }

void CSIGame::SetEndingCriteria(bool ByTime, bool ByFrame)
{
	unsigned int i;

	EndByTime	= ByTime;
	EndByFrame	= ByFrame;

	//Now set the way of finishing for every object in the system
	//Bonuses
	BonusManager->SetEndingCriteria(ByTime, ByFrame);
	//Shoots manager
	ShootsManager->SetEndingCriteria(ByTime, ByFrame);
	//Players
	for (i = 0; i < Players.size(); i++)
		Players[i]->SetEndingCriteria(ByTime, ByFrame);
	//Enemy navy
	Navy->SetEndingCriteria(ByTime, ByFrame);

	//Change ending criteria for the shoots
}

void CSIGame::SetSimulationType(CG_SIMULATION_MODE SimulationType)
{
	DiscreteSimulation				 = CG_DISCRETE_SIMUL == SimulationType;
	BonusManager->DiscreteSimulation = DiscreteSimulation;
	TimerManager.DiscreteSimulation  = DiscreteSimulation;

	//Players
	for (unsigned int i = 0; i < Players.size(); i++)
		Players[i]->DiscreteSimulation = DiscreteSimulation;

}

/*___________________________________________________________________
|                                                                    |
|                           LOAD INITIALIZATION                      |
|____________________________________________________________________|
*/

//////////////////////////////////////////////////////////////////////////////////////
//
// Load the initialization file
//
//
void CSIGame::LoadInitialization(void)
{
	CInitializationParser	InitParser;		//The parser used to understand the contents of the initialization file

	InitParser.AssignTags(CIP_Tags, MAXTAGS_IP);

	HTMLReader.ParseFile(CSIG_PredefinedFiles[CSIG_INITIALIZATION_FILE], Directories[CG_CONFIG_DIR], &InitParser);
}

/*___________________________________________________________________
|                                                                    |
|                           LOAD KEYBOARD	                         |
|____________________________________________________________________|
*/

//////////////////////////////////////////////////////////////////////////////////////
//
// Load the keyboard. This is the key map specification
//
//
void CSIGame::LoadKeyboard(void)
{
	CKeyboardParser KeyboardParser;

	KeyboardParser.SetKeyboard(&SIKeyboard);

	HTMLReader.ParseFile(CSIG_PredefinedFiles[CSIG_KEYBOARD_FILE], Directories[CG_CONFIG_DIR], &KeyboardParser);
}


/*___________________________________________________________________
|                                                                    |
|                           LOAD DEVICES                             |
|____________________________________________________________________| 
*/

//////////////////////////////////////////////////////////////////////////////////////
//
// Load the devices
//
//
void CSIGame::LoadDevice(void)
{
	//HTML Reader
	CDeviceParser	DeviceReader;	//The parser used to understand the contents of the initialization file
	
	HTMLReader.ParseFile(CSIG_PredefinedFiles[CSIG_DEVICES_FILE], Directories[CG_CONFIG_DIR], &DeviceReader);
	return;
}


/*___________________________________________________________________
|                                                                    |
|                           LOAD LEVELS                              |
|____________________________________________________________________| 
*/

//////////////////////////////////////////////////////////////////////////////////////
//
// Load game levels
//

void CSIGame::LoadLevel(int lev)
{
	UGKS_String		LevelFile;
	char			cLevel[3];

	LevelFile = "Level_";
	_itoa(lev, cLevel, 10);
	LevelFile += cLevel;
	
	LoadGame (LevelFile, Directories[CG_LEVEL_DIR]);
}


void CSIGame::LoadLevelFile(int HtmlFileInd)
{
	UGKS_String		UserLevelPath;

	UserLevelPath = Directories[CG_USER_LEVEL_DIR] + UGKOS_DIRECTORY_SEPARATOR + CSIG_PredefinedFiles[CSIG_ULEVEL_FILE];
	LoadGame (HtmlFiles[HtmlFileInd], UserLevelPath);
}

void CSIGame::PromoteLevel(){
	if(!OnlyOneLevel){
		HiScore=Score;
		Level++;
		LevelReaded	= false;
		Navy->Init();

		if(Level<=5)
		{
			//Reset of the differents sets
			ResetObjects();
		
			OutEvent(CSIG_NEXT_LEVEL);	//v Controlado 9->2
		}
		else
		{
			//END GAME WITH GAME WON
			OutEvent(CSIG_FADING2END);	//v Controlado 9->12
		}
	}
	else 
		//END GAME WITH GAME WON
		OutEvent(CSIG_FADING2END);	//v Controlado 9->12
}

//////////////////////////////////////////////////////////////////////////////////////
//
// Load saved game
//
//
void CSIGame::LoadGame (UGKS_String  filename, UGKS_String  folder)
{	
	CLevelReader	LevelReader;	//The parser used to understand the contents of any Level file. This does not have to be constructed and destroyed every time a new level is loaded

	LevelReader.AssignTags(CLeP_Tags, MAXTAGS_L);

	HTMLReader.ParseFile(filename, folder, &LevelReader);
	
	/*UGKS_String msg1 = "Poner aquí el resultado de la actualización de los contenidos del objeot creado por los alumnos. ";
	
	msg1 = msg1 + "\nSe puede insertar caracteres de retorno de carro para cada aspecto a presentar.";
	CString msg = UGKS_string2CString(msg1);

	MessageBox(NULL, msg, L"Resultado", MB_OK);
	*/

	std::stringstream ss;
	ss << "LOADED CHARACTER PERSONAJE: \n";
	ss << "\tSpeed=( x=" << Personaje->Speed.v[XDIM] << ", z=" << Personaje->Speed.v[YDIM] << ", z=" << Personaje->Speed.v[ZDIM] << "\n";
	ss << "\tPosition=( x=" << Personaje->Position.v[XDIM] << ", z=" << Personaje->Position.v[YDIM] << ", z=" << Personaje->Position.v[ZDIM] << "\n";
	ss << "\tHealth=" << Personaje->Health << "\n";
	ss << "\tWeight=" << Personaje->Fuel << "\n";
	ss << "\tFuel=" << Personaje->Fuel << "\n";
	ss << "\tArms=" << Personaje->Arms << "\n";
	ss << "\tInmortal=" << (Personaje->Inmortal? "ES INMORTAL":"NO ES INMORTAL") << "\n";
	
		CString msg = UGKS_string2CString(ss.str());
	MessageBox(NULL, msg, L"Resultado parser", MB_OK);

	Navy->SetShipsAttributes();
}

void CSIGame::StartCameraMov()
{
	Camera->Position = UGK::Bezier(control_points, 5, 0.5);
}

//AI methods
///Initializes the Game AI with the especified predefined FSM State 
void CSIGame::AI_Init(SIAI_AI_TYPE Type) 
{
	CFSMReader	reader;		//The parser used to understand the contents of the FSM file
	FSM			*NewAI; ///<Declares the FSM to deliver to the AIManager 

	switch(Type)
	{
		case SIAI_GAME_DEFAULT:
			//ADD FUNCTIONS TO BE USED BY THE FSM
			reader.addPublicFunction(AIFunctionName[CSIG_FN_NULL], NULL);

			HTMLReader.ParseFile(CSIG_PredefinedFiles[CSIG_GAME_AI_FILE], Directories[CG_AI_DIR], GetTitle(), GetVersion(), &reader);

			NewAI = new FSM();
			NewAI->addStates(reader.getFSMStatesVector(),reader.getFSMNumStates()); ///<Setting the game FSM states
			NewAI->SetState(reader.getFSMInitState());  ///<Sets the game 'FSM' current state as the initial.

			AIManager.SetAI(NewAI,SIAI_GAME_DEFAULT);  ///<Informs the 'AIManager' about the 'FSM' that the game must have.
			AI = AIManager.GetAI(SIAI_GAME_DEFAULT);  ///<Asks the 'AIManager' for the 'FSM' corresponding to the game appropiately initialized.

			break;
		case SIAI_SUPPLYSHIP_DEFAULT:
			//ADD FUNCTIONS TO BE USED BY THE FSM
			reader.addPublicFunction(AIFunctionName[CSIG_FN_NULL], NULL);
			reader.addPublicFunction(AIFunctionName[CSIG_FN_CSS_MOVE], (mFuncPtr)CSS_FSM_Move);
			reader.addPublicFunction(AIFunctionName[CSIG_FN_CSS_DISPLAY], (mFuncPtr)CSS_display);
			reader.addPublicFunction(AIFunctionName[CSIG_FN_CSS_HEALTH], (mFuncPtr)CSS_FSM_Healthing);
			reader.addPublicFunction(AIFunctionName[CSIG_FN_CSS_DIE], (mFuncPtr)CSS_FSM_Dye);
			reader.addPublicFunction(AIFunctionName[CSIG_FN_CSS_DEATH], (mFuncPtr)CSS_FSM_Death);
			
			HTMLReader.ParseFile(CSIG_PredefinedFiles[CSIG_SUPPLYSHIP_AI_FILE], Directories[CG_AI_DIR], Title, Version, &reader);

			NewAI = new FSM();
			NewAI->addStates(reader.getFSMStatesVector(), reader.getFSMNumStates()); ///<Sets the 'SupplyShip' FSM states
			NewAI->SetState(reader.getFSMInitState());
			AIManager.SetAI(NewAI, SIAI_SUPPLYSHIP_DEFAULT);  ///<Informs the AIManager about the FSM that the 'SupplyShip' must have
			break;
		case SIAI_SHIP_DEFAULT:
			//ADD FUNCTIONS TO BE USED BY THE FSM
			reader.addPublicFunction(AIFunctionName[CSIG_FN_NULL], NULL);
			reader.addPublicFunction(AIFunctionName[CSIG_FN_CS_INIT], (mFuncPtr)init_ship);
			reader.addPublicFunction(AIFunctionName[CSIG_FN_CS_DISPLAY], (mFuncPtr)CS_Move);
			reader.addPublicFunction(AIFunctionName[CSIG_FN_CS_DISPLAY], (mFuncPtr)CS_Display);
			reader.addPublicFunction(AIFunctionName[CSIG_FN_CS_BURST], (mFuncPtr)CS_Bursting);
			reader.addPublicFunction(AIFunctionName[CSIG_FN_CS_KILL], (mFuncPtr)CS_Kill);

			HTMLReader.ParseFile(CSIG_PredefinedFiles[CSIG_SHIP_AI_FILE], Directories[CG_AI_DIR], Title, Version, &reader);

			NewAI = new FSM();
			NewAI->addStates(reader.getFSMStatesVector(), reader.getFSMNumStates()); ///<Sets the 'Ship' FSM states
			NewAI->SetState(reader.getFSMInitState());
			AIManager.SetAI(NewAI, SIAI_SHIP_DEFAULT);  ///<Informs the AIManager about the FSM that the 'Ship' must have
			break;
		case SIAI_CIRCLESHIP_DEFAULT:
			//ADD FUNCTIONS TO BE USED BY THE FSM
			reader.addPublicFunction(AIFunctionName[CSIG_FN_NULL], NULL);
			reader.addPublicFunction(AIFunctionName[CSIG_FN_CCS_MOVE], (mFuncPtr)CCS_FSM_Move);
			reader.addPublicFunction(AIFunctionName[CSIG_FN_CCS_DISPLAY], (mFuncPtr)CCS_display);
			reader.addPublicFunction(AIFunctionName[CSIG_FN_CCS_HEALTH], (mFuncPtr)CCS_FSM_Healthing);
			reader.addPublicFunction(AIFunctionName[CSIG_FN_CCS_DIE], (mFuncPtr)CCS_FSM_Dye);
			reader.addPublicFunction(AIFunctionName[CSIG_FN_CCS_DEATH], (mFuncPtr)CCS_FSM_Death);
			HTMLReader.setEventHandler(&reader);

			HTMLReader.ParseFile(CSIG_PredefinedFiles[CSIG_CIRCLESHIP_AI_FILE], Directories[CG_AI_DIR], Title, Version, &reader);

			NewAI = new FSM();
			NewAI->addStates(reader.getFSMStatesVector(), reader.getFSMNumStates()); ///<Sets the 'CircleShip' FSM states
			NewAI->SetState(reader.getFSMInitState());
			AIManager.SetAI(NewAI, SIAI_CIRCLESHIP_DEFAULT);  ///<Informs the AIManager about the FSM that the 'CircleShip' must have
			break;
	}//Behaviour switch
}

/*___________________________________________________________________
|                                                                    |
|        D R A W  (D I S P L A Y  2 D / 3 D  E L E M E N T S)        |
|____________________________________________________________________| 
*/

void CSIGame::Render(void)
{
	//Amount of pixels from the right margin to wirte real time information
	#define TEXT_X_POS (initialWidthW-150)
	#define TEXT_Y_POS 50
	//Amount of pixels to increment for every line written
	#define TEXT_Y_INC 25

	unsigned int i,
				 posX, posY;

	switch(RenderMode)
	{
	case CHAR_NO_RENDER:
		return;
	case CHAR_2D:
	case CHAR_LINES3D:
	case CHAR_3D:
		UGKGLE_glResetError();
		Application.Window.SetRenderingContext();

		glClear  (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		Application.Window.Reshape(45.0f);

		Camera->SetLook();

		// Camera zoom modification during the 2D to 3D transition

		if (CSIG_PASSING_2_3D == AI->GetState())
			glTranslatef ( 0.0, 0.0, Camera->zoom_total_bonus);					// Camera->zoom pendant le passage à mode 3D
		else
			glTranslatef ( 0.0, 0.0, Camera->zoom_total );						// Camera->zoom utilisateur
		
		// CAMERA -> modification pendant l'intermède (rotation de la scène)
		//
		// la rotation de l'intermède ne se fait pas directement avec un angle incrémenté
		// pour éviter des opérations trop nombreuses sur la matrice MODEL_VIEW, mais sur
		// un angle global après la réinitialisation de la matrice par glLoadIdentity() (ci-dessus)
		glRotatef(intermede_angle_total, -0.3f, 0.55f, 1.0f);			// rotation de l'intermède -.3 .5 1.0

		// CAMERA -> modification utilisateur (clic et déplacement de la souris)
		glRotatef(Scene.Angle.v[YDIM], 1.0, 0.0, 0.0);								// rotation utilisateur (par rapport à l'axe des x)
		glRotatef(Scene.Angle.v[XDIM], 0.0, 1.0, 0.0);								// rotation utilisateur (par rapport à l'axe des y)

		// Background	
		//GUI.display_background(false);										

		//********************************** Start Render with Blending *************************
		glEnable(GL_BLEND);		
		// Display Player Health
		GUI.display_Health();
		// Number of lives
		GUI.display_below_lives();
		// Score, Hiscore, Lives
		GUI.display_score_hiscore_lives();									

		//Shoots
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		ShootsManager->Render();

		//Bonus
		BonusManager->Render();

		if (CSIG_PASSING_2_3D == AI->GetState()) GUI.display_ring_2d_to_3d();

		//********************************** Finish Render with Blending *************************
		
		if (CHAR_2D != RenderMode)
		{
			/// End of SHIPS Lights
			glEnable(SIGLB_PLAYER_LIGHT);	// éteint la lumière Player
			glDisable(SIGLB_SHIP_LIGHT);
			glDisable(SIGLB_SHIP2_LIGHT);
		
			/// End 3D WRAPPER
			glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
			glDisable(GL_DEPTH_TEST);		// désactive le test de profondeur
			glEnable(GL_BLEND);				// active le mélange de couleurs (ici transparence)
			glDisable(GL_CULL_FACE);		// on affiche les faces "cachées" (le dos des éléments en mode 2D)
		}else 
			glDisable(GL_BLEND);


		//Scene Graph
		SceneGraph.Render();

		//Measures calculation
		TimerManager.calcFpsPct();

		posX = TEXT_X_POS;
		posY = TEXT_Y_POS;
		sprintf(texto, "FPS %.2f", TimerManager.fps);
		txtFPS.text2D_draw(posX, posY, texto, initialWidthW, initialHeightW, yelow);
		posY += TEXT_Y_INC;
		sprintf(texto, "Rnd %.2f%%", TimerManager.pct[SIGLBT_RENDER_TIMING]);
		txtRND.text2D_draw(posX, posY, texto, initialWidthW, initialHeightW, yelow);
		posY += TEXT_Y_INC;
		sprintf(texto, "Upd %.2f%%", TimerManager.pct[SIGLBT_UPDATE_TIMING]);
		txtUPD.text2D_draw(posX, posY, texto, initialWidthW, initialHeightW, yelow);
		posY += TEXT_Y_INC;
		sprintf(texto, "Idl %.2f%%", TimerManager.pct[SIGLBT_IDLE_TIMING]);
		txtIDL.text2D_draw(posX, posY, texto, initialWidthW, initialHeightW, yelow);
		
		//Draw Info Texts
		if(DiscreteSimulation)
		{	
			posY += TEXT_Y_INC;
			sprintf(texto, "Rtdsk %.2f%%", TimerManager.pct[SIGLBT_RTDESK_TIMING]);
			txtIDL.text2D_draw(posX, posY, texto, initialWidthW, initialHeightW, yelow);
		}

		posY += TEXT_Y_INC;
		sprintf(texto, "Suma %% %.2f", TimerManager.pctSuma);
		txtSUM.text2D_draw(posX, posY, texto, initialWidthW, initialHeightW, yelow);

		Application.Window.Refresh();
		break;
	}
}

void CSIGame::ChangeRenderMode(CHAR_RENDER_MODE Mode)
{
	unsigned int i;

	RenderMode = Mode;
	
	switch(RenderMode)
	{
	case CHAR_NO_RENDER:
		break;
	case CHAR_2D:
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		break;
	case CHAR_LINES3D:
	case CHAR_3D:
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);	//ACHTUNG: Verify this is OK
		break;
	default: return;
	}

	//Transfer the event to every character in the world
	//Enemy navy
	Navy->ChangeRenderMode(RenderMode);

	//Players
	for(i=0;i<Players.size();i++)	Players[i]->ChangeRenderMode(RenderMode);

	//Bonus
	BonusManager->ChangeRenderMode(RenderMode);
	BonusManager->ChangeRenderMode(RenderMode);

	//Bunkers
	SceneGraph.ChangeRenderMode(RenderMode);

	//Shoots
	ShootsManager->ChangeRenderMode(RenderMode);
	ShootsManager->ChangeRenderMode(RenderMode);
}

void CSIGame::Update ()		//Update the world in continuous simulation.
{
	unsigned int i;
	CCharacter * DefaultChar;
	bool		 Won = true;

	SIGLB_AddLog(LogMessages[CSIG_UPDATE_GAME]);

	//Manage possible messages 
	NextSimulation = RTDESK_Engine->Simulate();

	//Player Update
	Players[CurrentPlayer]->Update();

	//***********Updating Managers***************
	//Update Active Bonus (Time is supervised by each one)
	BonusManager->UpdateActiveBonus();
	//Bonus Manager
	BonusManager->Maintenance();

	//Update Active Shoots (Time is supervised by each one)
	ShootsManager->Update();
	//Shoots Manager
	ShootsManager->Maintenance();

	Application.Update();

	SceneGraph.Update();

	//***********Checking for Win*************
	//Test if there exist any enemy
	for (i = 0; i < SceneGraph.Size(); i++)
	{
		DefaultChar = SceneGraph.Object(i);
		switch (DefaultChar->Type)
		{
		case CHARS_SHIP:
		case CHARS_SUPPLYSHIP:
		case CHARS_CIRCLESHIP:
			if (DefaultChar->Alive())
			{
				Won = false;
				break;
			}
		}
	}

	if (Won){
		NextRenderMode = RenderMode;
		OutEvent(CSIG_FADING2WON);  //v Controlado 4->11
		ScoresManager.SetHiscore(Score);
	}
}

void CSIGame::DiscreteUpdate ()		//Update the world. Similar to render() but updating
{
	#ifdef XM_UPD_TIME_DISC
	TimerManager.GetTimer(SIGLBT_UPDATE_TIMING)->InitCounting();
	#endif

	SIGLB_AddLog(LogMessages[CSIG_UPDATE_GAME]);

	//***********Updating Managers***************
	//Bonus Manager
	BonusManager->Maintenance();

	//Shoots Manager
	ShootsManager->Maintenance();

	///Now the update is through the parser
	SIKeyboard.Update();
	Application.Update();

	SceneGraph.Update();

	#ifdef DEF_EXP_CSV_FILE
	//***********Checking for end time***************
	switch (WayToEnd)
	{
	case CSIG_END_BY_TIME:
		if (DEF_MAX_TICKS < TimerManager.GetTimer(SIGLBT_APP_TIMING)->TicksUntilNow())
		{
			EndByTime	= true;
			EndByFrame	= false;
		}
		break;
	case CSIG_END_BY_FRAME:
		if (DEF_END_IN_FRAME <= TimerManager.AllnFrames)
		{
			EndByTime	= false;
			EndByFrame	= true;
		}
		break;
	}
	#endif

	if (PlayerWon()){
		NextRenderMode = RenderMode;
		OutEvent(CSIG_FADING2WON);  //v Controlado 4->11
	}

	#ifdef XM_UPD_TIME_DISC
	TimerManager.EndAccCounting(SIGLBT_UPDATE_TIMING);
	#endif
}

/*___________________________________________________________________
|                                                                    |
|                           INITIALISATION                           |
|____________________________________________________________________| 
*/

//////////////////////////////////////////////////////////////////////////////////////
//
// Game Initialization
//

/**   
	* @fn DWORD WINAPI CSIG_ClearManagers(LPVOID lpParam) 
	* Thread for clearing all the manager internal queues
*/
DWORD WINAPI CSIG_ClearManagers(LPVOID lpParam) 
{
	SIGLB_AddLog(LogMessages[CSIG_CLEARING_MANAGERS]);
	
	Navy->Timer[CN_RND_PERIOD].SetAlarm(Game->DefaultRndPeriod[CHARS_NAVY]);
	Navy->Timer[CN_UPD_PERIOD].SetAlarm(Game->DefaultUpdPeriod[CHARS_NAVY]);

	TexturesManager.Textures.clear();
	MeshesManager.ClearAllMeshes();
	AnimationsManager.Animations.clear();
	SoundsManager.ClearAllSounds();
	
	//Shoots manager initialize
	ShootsManager->ShootsAct.clear();

	//Initialize bonus managers
	BonusManager->EndByFrame	= Game->EndByFrame;
	BonusManager->EndByTime		= Game->EndByTime;
	BonusManager->Bottom		= SIGLBD_PG_BOTTOM;
	BonusManager->Ceiling		= SIGLBD_PG_CEILING;
	BonusManager->UpdateSF(TimerManager.GetSF());
	BonusManager->Timer[CBN_RND_PERIOD].SetAlarm(Game->DefaultRndPeriod[CHARS_BONUS_MNGR]);
	BonusManager->Timer[CBN_UPD_PERIOD].SetAlarm(Game->DefaultUpdPeriod[CHARS_BONUS_MNGR]);
	
	BonusManager->DiscreteSimulation = Game->DiscreteSimulation;

	BonusManager->Init();

	return NULL;
}

/**   
	* @fn DWORD WINAPI CSIG_InitGraphics(LPVOID lpParam) 
	* Thread for initializing all the OpenGL internal values
*/
void CSIGame::InitGraphics() 
{
	UGKS_String aux = "initializing all the OpenGL internal values...";

	static float PlayerLightPos[] = { -10.0, 20.0,   1.0, 0.0 }; // for Player
	static float ShipsLightPos[]  = { -10.0, 10.0,   5.0, 0.0 }; // for ships
	static float ShipsLightPos2[] = {  10.0,  0.0, -20.0, 0.0 }; // for ships

	//SIGLB_AddLog(aux);
	//Create Camera before  Application.Window.Create() since WndProc needs to access to the camera as soon as the window is created. 
	//A race condition may arise here. Be careful
	Camera = (CCamera*)CharacterPool->get(UGK_CAMERA, UGKOBJM_NO_SUBTYPE);
	Camera->Position.v[XDIM] = 0; Camera->Position.v[YDIM] = 0; Camera->Position.v[ZDIM] = 20;

	if (!Application.Window.FullScreen && !Application.Window.quickFullScreen)
		if (Application.Window.Width < GraphicsCard.DeskTop.Width && Application.Window.Height < GraphicsCard.DeskTop.Height) // on ne centre que si la fenêtre est + petite que le bureau
	{
		Application.Window.LeftUpX = (GraphicsCard.DeskTop.Width - Application.Window.Width) / 2;
		Application.Window.LeftUpY = (GraphicsCard.DeskTop.Height - Application.Window.Height) / 2;
	}

	Application.Window.SetDeskTop(&GraphicsCard.DeskTop);
	MainProgramLooping = Application.Window.Create();

	// OPENGL -> INITIALISATION
	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);						// Black Background
	glClearDepth (1.0f);										// Depth Buffer Setup
	glEnable(GL_TEXTURE_2D);									// Enable Texture Mapping
	glDisable(GL_CULL_FACE);									// Front and Back faces of polygons are drawn

	glDepthFunc(GL_LEQUAL);										// Define Depth
	glDisable(GL_DEPTH_TEST);									// No depth test

	glBlendFunc(GL_SRC_ALPHA,GL_ONE);							// Define Alpha Blending
	glEnable(GL_BLEND);											// Enable Alpha Blending
	
	// OPENGL -> QUALITÉ
	glShadeModel (GL_SMOOTH);									// Smooth Shading for lines and polygons (for 3D objects)
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Hint for perspective calculations (most accurate)
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);						// Hint for lines rasterization (progress bar, 3D objects in Lines3D mode...)
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);					// Hint for polygon rasterization (3D objects...)

	// OPENGL -> Lights (Only for 3D MODE)

	static float ambient[]			=	{0.9f, 0.9f, 0.9f, 1.0f};
	static float diffuse[]			=	{0.25f, 0.25f, 0.25f, 1.0f};
	static float specular[]			=	{1.0f, 1.0f, 1.0f, 1.0f};
	
	static float ambient2[]			=	{0.2f, 0.2f, 0.2f, 1.0f};
	static float diffuse2[]			=	{0.05f, 0.05f, 0.05f, 0.05f};

	static float lmodel_ambient[]	=	{0.1f, 0.1f, 0.1f, 1.0f};
	static float lmodel_localviewer[] =	{1.0f};

	static float mat_shininess[]	=	{10.0f};
	static float mat_specular[]		=	{0.8f, 0.8f, 0.8f, 1.0f};
	static float mat_ambient[]		=	{0.6f, 0.6f, 0.6f, 1.0f};
	static float mat_diffuse[]		=	{0.4f, 0.4f, 0.4f, 1.0f};

	static float spot_direction[] =		{0.0f, -1.0f, 0.0f};

	glEnable(GL_LIGHTING);

	// Setting up lights for every group in 3D mode
	// Setting up light for the player in 3D mode
	glLightfv(SIGLB_PLAYER_LIGHT, GL_AMBIENT, ambient);
	glLightfv(SIGLB_PLAYER_LIGHT, GL_DIFFUSE, diffuse);
	glLightfv(SIGLB_PLAYER_LIGHT, GL_SPECULAR, specular);
	glLightfv(SIGLB_PLAYER_LIGHT, GL_POSITION, PlayerLightPos);

	// Setting up first light for the ships of the enemy in 3D mode
	glLightfv(SIGLB_SHIP_LIGHT, GL_AMBIENT, ambient);
	glLightfv(SIGLB_SHIP_LIGHT, GL_DIFFUSE, diffuse);
	glLightfv(SIGLB_PLAYER_LIGHT, GL_SPECULAR, specular);
	glLightfv(SIGLB_SHIP_LIGHT, GL_POSITION, ShipsLightPos);

	// Setting up second light for the ships of the enemy in 3D mode
	glLightfv(SIGLB_SHIP2_LIGHT, GL_AMBIENT, ambient2);
	glLightfv(SIGLB_SHIP2_LIGHT, GL_DIFFUSE, diffuse2);
	glLightfv(SIGLB_SHIP2_LIGHT, GL_POSITION, ShipsLightPos2);

	// lumière (réfléchie sur les matériaux)
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	
	// PASSAGE 2D à 3D -> camera path, courbe de Bézier (mouvement de caméra pour le passage au mode 3D)
	control_points[0].v[XDIM] =  0; control_points[0].v[YDIM] = 0; control_points[0].v[ZDIM] = 20;
	control_points[1].v[XDIM] = -8; control_points[1].v[YDIM] = 0; control_points[1].v[ZDIM] = 18;//15
	control_points[2].v[XDIM] = -2; control_points[2].v[YDIM] = 0; control_points[2].v[ZDIM] =  5;//5
	control_points[3].v[XDIM] =  2; control_points[3].v[YDIM] = 0; control_points[3].v[ZDIM] = -7;//3, -2
	control_points[4].v[XDIM] =  5; control_points[4].v[YDIM] = 0; control_points[4].v[ZDIM] = 18;//15
	control_points[5].v[XDIM] =  0; control_points[5].v[YDIM] = 0; control_points[5].v[ZDIM] = 20;

	// SPHERE
	quadratic=gluNewQuadric();							// Create A Pointer To The Quadric Object (Return 0 If No Memory)
	gluQuadricNormals(quadratic, GLU_SMOOTH);			// Create Smooth Normals 
	gluQuadricTexture(quadratic, GL_TRUE);				// Create Texture Coords 

	// LOADING -> vue orthogonale 2D
	Application.Window.Reshape(90.0f);
	Camera->Reshape(10.0f);
}

/**   
	* @fn DWORD WINAPI CSIG_InitSounds(LPVOID lpParam) 
	* Thread for initializing all the sounds internal values
*/
DWORD WINAPI CSIG_InitSounds(LPVOID lpParam) 
{
	SIGLB_AddLog(SIGLB_GetText(TR_GAME_LOADING_SOUNDS));	//Loading Sounds...

	Game->Spectrum = false;
	SoundsManager.SetAudioHardware(&Game->soundcard);
	// Load the sound manager
	Game->SoundSystem.Init();
	SoundsManager.Init();
	
	SoundsManager.SetMusicPath(Game->Directories[CG_MUSIC_DIR]);
	SoundsManager.SetTrackPath(Game->Directories[CG_SOUNDS_DIR]);
	SoundsManager.LoadSounds(CGS_MAX_SND, CGS_SoundsFiles);

	return NULL;
}

/**
* @fn void CSIGame::AssignSounds2defaultObjects()
* Assign global sounds to specific objects as they will need them during the playing
*/

void CSIGame::AssignAI2defaultObjects()
{
	unsigned int i;
	CCharacter *C;

	// Default Artificial Intelligence for the navy
	for (i = 0; i < SceneGraph.Size(); i++)
	{
		C = SceneGraph.Object(i);
		switch (C->Type)
		{
		case CHARS_SHIP:
			C->AI = AIManager.GetAI(SIAI_SHIP_DEFAULT);
			break;
		case CHARS_SUPPLYSHIP:
			C->AI = AIManager.GetAI(SIAI_SUPPLYSHIP_DEFAULT);
			break;
		case CHARS_CIRCLESHIP:
			C->AI = AIManager.GetAI(SIAI_CIRCLESHIP_DEFAULT);
			break;
		default:;
		}
	}
}


/**
* @fn void CSIGame::AssignSounds2defaultObjects()
* Assign global sounds to specific objects as they will need them during the playing
*/

void CSIGame::AssignSounds2Objects()
{
	unsigned int i, l;
	CCharacter* C;

	//Bonus Manager sounds assignment
	BonusManager->SetSoundsAmount(CBN_MAX_SND);
	BonusManager->SetSound(SoundsManager.GetSound(CGS_SHIELD_SND), CBN_SHIELD_SND);
	BonusManager->SetSound(SoundsManager.GetSound(CGS_BONUS_SND), CBN_BONUS_SND);

	//Bonus sounds assignment is performed by the bonus manager once it is updated as before
	BonusManager->AssignSounds2Bonus();

	//Player sounds assignment
	for (i = 0; i < Players.size(); i++)
	{
		Players[i]->SetSoundsAmount(CPL_MAX_SND);
		Players[i]->SetSound(SoundsManager.GetSound(CGS_EXPLOSION_SND), CPL_EXPLOSION_SND);
		Players[i]->SetSound(SoundsManager.GetSound(CGS_SHOOT_SND), CPL_SHOOT_SND);
		Players[i]->SetSound(SoundsManager.GetSound(CGS_SHOOT3D_SND), CPL_SHOOT3D_SND);
		Players[i]->SetSound(SoundsManager.GetSound(CGS_TOUCH_SND), CPL_TOUCH_SND);

		for (l = 0; l < CPL_MAX_LASERS; l++)
		{
			Players[i]->Laser[l].SetSoundsAmount(CPL_MAX_SND);
			Players[i]->Laser[l].SetSound(SoundsManager.GetSound(CGS_EXPLOSION_SND), CPL_EXPLOSION_SND);
			Players[i]->Laser[l].SetSound(SoundsManager.GetSound(CGS_SHOOT_SND), CPL_SHOOT_SND);
			Players[i]->Laser[l].SetSound(SoundsManager.GetSound(CGS_SHOOT3D_SND), CPL_SHOOT3D_SND);
			Players[i]->Laser[l].SetSound(SoundsManager.GetSound(CGS_TOUCH_SND), CPL_TOUCH_SND);
		}
	}

	//Set sounds to the enemy navy: ships, supplyships, circleships,...
	Navy->SetSoundsAmount(CN_MAX_SND);
	Navy->SetSound(SoundsManager.GetSound(CGS_DESCEND_SND),	  CN_DESCEND_SND);		//Every time the whole navy drops down a little bit
	Navy->SetSound(SoundsManager.GetSound(CGS_EXPLOSION_SND), CN_EXPLOSION_SND);	//Every time any ship, supplyship, circleship,... burst out

	//Set sounds to the shoots.
	ShootsManager->SetSoundsAmount(CSH_MAX_SND);
	ShootsManager->SetSound(SoundsManager.GetSound(CGS_EXPLOSION_SND), CSH_EXPLOSION_SND);
	ShootsManager->SetSound(SoundsManager.GetSound(CGS_TOUCH_SND), CSH_TOUCH_SND);
	ShootsManager->AssignSounds2Shoots();
}

/**
	* @fn bool CSIGame::Initialize (void)
	* Any GL Init Code & User Initialization Goes Here
*/
bool CSIGame::Initialize (void)
{
	HANDLE  hThread;
	DWORD   dwThreadId;

	GLenum Error;

	UGKS_String temp, Fonts = "font.tga";

	//Set Title and Versions
	//Initialize steady characteristics of the HTMLReader before any reading is performed
	HTMLReader.SetTitle(GetTitle());
	HTMLReader.SetVersion(GetVersion());

	//Initialize by default the collision table
	InitCollisionTable();

	//Initialize global variables before reading the parser initialization(in case the values are not included)
	Navy = (CNavy*)CharacterPool->get(CHARS_NAVY, UGKOBJM_NO_SUBTYPE);

	Navy->Title		= GetTitle();
	Navy->Version	= GetVersion();

	//Initialize bonus, bunkers and shoots managers
	BonusManager	= (CBonusManager*)	CharacterPool->get(CHARS_BONUS_MNGR,	UGKOBJM_NO_SUBTYPE);
	ShootsManager	= (CShootsManager*)	CharacterPool->get(CHARS_SHOOT_MNGR,	UGKOBJM_NO_SUBTYPE);

	ShootsManager->Pool = CharacterPool;

	//Scene Graph set up
	SceneGraph.SetPool(CharacterPool);
	//SceneGraph.Reset();

	//Initialization of general Timers
	InitTimers();

	//Creates a thread in order to speed up the initialization of the program
	hThread = CreateThread( 
		NULL,               // default security attributes
		0,                  // use default stack size  
		CSIG_ClearManagers,	// thread function name
		NULL,				// argument to thread function 
		0,                  // use default creation flags 
		&dwThreadId);		// returns the thread identifier 

	//Verifies that everything is fine
	if (NULL == dwThreadId)
		CSIG_ClearManagers(NULL);	//Do it blocking. HW does not allow threads

	//Initialization of the graphics
	InitGraphics();
	
	InitObjectReferences();

	//Initialization load
	LoadInitialization();

	//Init the textures manager
	//TexturesManager.SetFilesPath(CSIG_DefaultDirectories[CG_TEXTURE_DIR]);
	//TexturesManager.LoadTextures(CTM_TexturesFiles, CTM_MAX_TEXTURE);

	//Internationalization. Language specific texts load
	Application.LoadCurrentLanguage();

	//Game AI initialization
	AI_Init (SIAI_GAME_DEFAULT);		//AI initialization by default
	//FSM initialization for Ships, SupplyShips and CircleShips.
	AI_Init(SIAI_SHIP_DEFAULT);			//AI Ship initialization by default
	AI_Init(SIAI_SUPPLYSHIP_DEFAULT);	//<AI Supply Ship by default
	AI_Init(SIAI_CIRCLESHIP_DEFAULT);	//<AI Circle Ship initialization by default

	// Loading characters (fonts) images
	UGKS_GenerateFullPathName(temp, CSIG_DefaultDirectories[CG_TEXTURE_DIR], Fonts);
	fontLoad (temp);

	//Once the initialization is done and all the default values for any given character are loaded
	//Load sounds for all objects in the game
	//Creates a thread in order to speed up the loading of the sounds
	hThread = CreateThread(
		NULL,               // default security attributes
		0,                  // use default stack size  
		CSIG_InitSounds,    // thread function name
		NULL,				// argument to thread function 
		0,                  // use default creation flags 
		&dwThreadId);		// returns the thread identifier 

	//Verifies that everything is fine
	if (NULL == dwThreadId)
		CSIG_InitSounds(NULL);	//Do it blocking. HW does not allow threads

	Navy->Antialiasing	= Application.Window.Antialiasing_active;
	GUI.Antialiasing	= Application.Window.Antialiasing_active;

	//Start Animations
	for (unsigned int i = 0; i<AnimationsManager.Animations.size(); i++)
		AnimationsManager.Animations[i]->Start();

	// Load HiScore
	HiScore = ScoresManager.GetHiscore();

	/*Reads the particular configuration of level 0*/
	if(ReadLevel)
	{   //Level reading is activated
		if(!LevelReaded)
		{ //If the level is not read

			bool	loadSavedGame	= false,
					SavedGame		= false;

			/* Checks if there is any saved game */
			if(FILE *f = fopen(CSIG_SAVED_GAME,"r"))
			{
				SavedGame = true;
				fclose(f);

				// Message Box to decide if the saved game is loaded or not
				// The buttons language is the system defaults
				int msgBox = MessageBox(NULL,
										TEXT("A saved game has been detected. Would you like to load this?"),
										TEXT("Load Game"),
										MB_ICONASTERISK | MB_OKCANCEL );

				if(msgBox == IDOK) // The saved game will be loaded
					loadSavedGame = true;
			}

			/* To load or not to load. That is the question */
			if(loadSavedGame){
				// Load saved game
				SIGLB_AddLog(SIGLB_GetText(TR_GAME_READ_SAVED_GAME));
				LoadGame(CSIG_SAVED_GAME, Directories[CG_LEVEL_DIR]);
				LevelReaded	= true;
			}

			SIGLB_AddLog(LogMessages[CSIG_SHOOTS]);
			ShootsManager->Init();

			//Delete saved game. Do not ask again if you want to play the saved game
			if (SavedGame)
			{
				 if( remove( CSIG_SAVED_GAME ) != 0 )
					 SIGLB_AddLog(LogMessages[CSIG_DELETE_SAVED_GAME]);
				 else
					 SIGLB_AddLog(LogMessages[CSIG_SAVED_GAME_DELETED]);
			}
		}
	}

	if(NextRenderMode != RenderMode)
		ChangeRenderMode(NextRenderMode);

	SIGLB_AddLog(LogMessages[CSIG_ENV_VBLES]);

	// GAME OPTION -> SYNCHRO VERTICALE (VSYNC) SI DISPONIBLE
	typedef BOOL (WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int interval);
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	wglSwapIntervalEXT = ( BOOL (WINAPI *)(int)) wglGetProcAddress( "wglSwapIntervalEXT" );
	Error = glGetError();
	if (wglSwapIntervalEXT)
		if (Application.Window.Vsync)
		{
			wglSwapIntervalEXT(1);
			Application.Window.VsyncActive = true;
		}
		else
		{
			wglSwapIntervalEXT(0);
			Application.Window.VsyncActive = false;
		}
	Error = glGetError();
	// MATHS -> Randomize function INITIALISATION
	//srand((unsigned)timeGetTime());
	srand(1234);

	//RT-Desk engine initializatión. RT-Desk is a Discrete Decoupled Simulation and message management Tool
	AssignRTDESK();

	if(LevelReaded)
		if (DiscreteSimulation)
			LaunchInitialEvents();

	//Space Invaders Keyboard input initialization. Do the nest instructions in the given order.
	SIKeyboard.HRTimerManager = new cHRTimerManager;
	SIKeyboard.HRTimerManager->SetSamplingFrequency(TimerManager.GetSamplingFrequency());

	///Exists two ways to initialize the keyboard, the old way is to read of program the keys, periodes, actions. With this way are used the two
	///following methods.
	///The new way is to read of the html with the parser of the UGK, in this case we use the method LoadKeyboard
	SIKeyboard.SetActionKeys	(CSIK_ActionKeys, CSIK_MAX_KBD_ACTION);
	SIKeyboard.SetDelay			(CSIK_ActionsDelay);

	//Key map load from file. TO DO: verify it works, test and debug
	//LoadKeyboard();

	return TRUE;
}

/// Called at the end of the game when cleaning all the game data
void CSIGame::Deinitialize (void)
{
	ScoresManager.SetHiscore(Score);

	#ifdef UGKSND_FMOD375 
			SoundsManager.MusicFadingOut();
	#elif UGKSND_SDL			
		// Stop the music
		SoundsManager.StopMusic();
		SoundsManager.Close();			
	#endif

	ResetObjects();

	LevelReaded = false;
	Level = 0;
	StopAfterOneLevel = false;

	//write to file the Time.fps average
	FILE *f;
	f=fopen("log_avgfps.txt","w+");
	char buffer[CGUI_MAX_CONSOLE_LENGTH];
	if (f){
		fwrite("*** Execution with FSM ****\n",sizeof(char),32,f);
		strcpy(buffer,UGKS_EMPTY_STRING);
		//sprintf(buffer,"Average Time.fps: %2.2f\n",Time.fpsavr);
		fwrite(buffer,sizeof(char),strlen(buffer),f);
	}
	fclose(f);
}

void CSIGame::ResetObjects(void)
{

	Navy->ShootsFrequency = Level;

	ShootsManager->ShootsAct.clear();

	TimerManager.DestroyAllTimers();

	SceneGraph.Init();

	for(unsigned int i=0; i<CBN_MAX_BONUSES; i++)
		BonusManager->Bonus[i].Init();
	
	EndByTime  = false;
	EndByFrame = false;

	// Load HiScore
	//Game->HiScore = ScoresManager.GetHiscore();

	#ifdef CHAR_USE_QUADTREE
	QTRoot->Clear();
	#endif
}

UGKS_String SaveCharacter(CCharacter *C)
{
	UGKS_String cName,
		mesh,
		character;
	char aux[64];

	float health, explosion;
	float posX, posY, posZ;
	float speedX, speedY, speedZ;
	float sizeX, sizeY, sizeZ;
	int lives;

	switch (C->Type) {

	case CHARS_PLAYER:
		cName = CHARS_Token2Lexeme(CHARS_PLAYER);
		lives = Players[CurrentPlayer]->Lives;
		C = Players[CurrentPlayer];
		break;

	case CHARS_SUPPLYSHIP:
		cName = CHARS_Token2Lexeme(CHARS_SUPPLYSHIP);
		break;

	case CHARS_SHIP:
		cName = CHARS_Token2Lexeme(CHARS_SHIP);
		break;
	default:
		break;
	}

	health = C->Health;
	explosion = C->Hit_duration;
	mesh = C->GetMeshName();
	
	posX = C->Position.v[XDIM];
	posY = C->Position.v[YDIM];
	posZ = C->Position.v[ZDIM];

	speedX = C->Speed.v[XDIM];
	speedY = C->Speed.v[YDIM];
	speedZ = C->Speed.v[ZDIM];

	sizeX = C->AABB[CHAR_BBSIZE][XDIM].Value;
	sizeY = C->AABB[CHAR_BBSIZE][YDIM].Value;
	sizeZ = C->AABB[CHAR_BBSIZE][ZDIM].Value;
	
	character = "\t\t<CHARACTER>\n";
	character += "\t\t\t<NAME>" + cName + "</NAME>\n";
	sprintf(aux, "%f", health);
	character += "\t\t\t<HEALTH>" + string(aux) + "</HEALTH>\n";
	// Position
	character += "\t\t\t<POSITION>\n";
	sprintf(aux, "%f", posX);
	character += "\t\t\t\t<X>" + string(aux) + "</X>\n";
	sprintf(aux, "%f", posY);
	character += "\t\t\t\t<Y>" + string(aux) + "</Y>\n";
	if (posZ > -1000)	// To avoid non-defined Z positions
	{
		sprintf(aux, "%f", posZ);
		character += "\t\t\t\t<Z>" + string(aux) + "</Z>\n";
	}
	else
		character += "\t\t\t\t<Z>0.0</Z>\n";

	character += "\t\t\t</POSITION>\n";

	sprintf(aux, "%f", explosion);
	character += "\t\t\t<EXPLOSION>" + string(aux) + "</EXPLOSION>\n";
	character += "\t\t\t<MESH>" + mesh + "</MESH>\n";
	character += "\t\t\t<SPEED>\n";
	sprintf(aux, "%f", speedX);
	character += "\t\t\t\t<X>" + string(aux) + "</X>\n";
	sprintf(aux, "%f", speedY);
	character += "\t\t\t\t<Y>" + string(aux) + "</Y>\n";
	sprintf(aux, "%f", speedZ);
	character += "\t\t\t\t<Z>" + string(aux) + "</Z>\n";
	character += "\t\t\t</SPEED>\n", character;

	// Only PLAYER
	if (CHARS_PLAYER == C->Type)
	{
		_itoa(lives, aux, 10);
		character += "\t\t\t<LIVES>" + string(aux) + "</LIVES>\n";
	}

	// Only SHIP
	if (CHARS_SHIP == C->Type)
	{
		//Ship = (CShip*)Navy->Ship[i];

		//_itoa(Ship->SubType, aux, 10);
		character += "\t\t\t<SUBTYPE>" + string(aux) + "</SUBTYPE>\n";;
		character += "\t\t\t<SIZE>\n";
		sprintf(aux, "%f", sizeX);
		character += "\t\t\t\t<X>" + string(aux) + "</X>\n";
		sprintf(aux, "%f", sizeY);
		character += "\t\t\t\t<Y>" + string(aux) + "</Y>\n";
		sprintf(aux, "%f", sizeZ);
		character += "\t\t\t\t<Z>" + string(aux) + "</Z>\n";
		character += "\t\t\t</SIZE>\n", character;
	}

	character += "\t\t</CHARACTER>\n";

	return character;
}


void CSIGame::Save(UGKS_String  fileName)			//Saves the game to fileName - Rodrigo Aldecoa
{
	ofstream f;
	UGKS_String info;
	char	aux[64];

	CCharacter *C;
	
	if (!fileName.empty())
		f.open(fileName, ofstream::out);

	if (f.is_open())	//The file is opened
	{

		// HEADER
		info = "<HTML>\n\t<HEAD>\n\t\t<TITLE>" + GetTitle() + "</TITLE>\n";
		info += "\t\t<VERSION>" + GetVersion() + "</VERSION>\n";
		_itoa(Level, aux, 10);
		info += "\t\t<TYPE>Level</TYPE>\n\t\t<LEVEL>" + string(aux) + "</LEVEL>\n";
		_itoa(Score, aux, 10);
		info += "\t\t<SCORE>" + string(aux) + "</SCORE>\n";
		info += "\t</HEAD>\n";

		f << info;

		// starts BODY
		info = "\t<BODY>\n";
		f << info;

		// SAVE PLAYERS
		for (unsigned int i = 0; i<Players.size(); i++) {
			if (Players[i]->Alive()) {
				info = SaveCharacter(Players[i]); // If the player is alive, save its parameters
				f << info; // Print player to file
			}
		}

		/// SAVE NAVY
		for (unsigned int i = 0; i < SceneGraph.Size(); i++)
		{
			C = SceneGraph.Object(i);
			switch (C->Type)
			{
			case CHARS_SHIP:
				if (BouncingMode) {
					float num = (30 + rand() % (170))*0.01f;	//0.3 <-> 2.0;
					C->Speed.v[XDIM] *= num;
					C->Speed.v[YDIM] *= num;
					C->Speed.v[XDIM] *= -1;
					C->Speed.v[YDIM] *= -1;
				}
			case CHARS_SUPPLYSHIP:
			case CHARS_CIRCLESHIP:
				C->FitMeshIntoBoundingBox();
#ifdef CHAR_USE_QUADTREE
				C->UpdateCollisionDetection();
#endif
				if (C->Alive()) {
					info = SaveCharacter(C); // If the Ship is alive, save its parameters
					f << info; // Print SupplyShip to file
				}
				break;
				//Save Scene graph
			case CHARS_BUNKER:	// SAVE BUNKERS
				if (C->Alive())
				{
					info = SaveCharacter(C); // If the Bunker is alive, save its parameters
					f << info;  // Print Bunker to file
				}
			}//Switch
		}//For

		/// file END
		info = "\t</BODY>\n";
		info += "</HTML>\n";
		f << info;

		f.close();
	}
}

void CSIGame::Display()
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Makes the buffers dark
	
	// Showing all the log strings
	FontsManager.SetFontSize(8);								// taille du texte des logs
	if (Application.Window.Width >= UGKCGC_SVGA_HOR_RES)								// logs affichés si résolution >= 800x600
		for(int i=0; i<Log.LogIndex; i++)							// pour toutes les entrées de log
		{
			FontsManager.SetFontSize ();
			if (Application.Window.Width >= UGKCGC_XGA_HOR_RES) 

					FontsManager.DefaultFontColor(); 
			else	FontsManager.SetFontColor(0.8f);
			//fontDrawString (10, Heigth -30 -(FontsManager.GetFontSize()+2)*i, "%s",  LogStrings[i]);
			FontsManager.DrawFont(10, Application.Window.Height -30 -(FontsManager.GetFontSize()+2)*i, "%s",  Log.LogStrings[i]);

/*					fontColor (1.0f, 1.0f, 1.0f); 
			else	fontColor (0.8f, 0.8f, 0.8f);
			fontDrawString (10, Heigth -30 -(FontsManager.GetFontSize+2)*i, "%s",  LogStrings[i]);
			*/
		}

	// Showing the image "loading..."
	Application.Window.Refresh();
}

CSIG_GAME_STATE		Semaphore = CSIG_INITIAL;

void CSIGame::RunMainLoop()
{
	HANDLE  hThread;
	DWORD   dwThreadId;

	unsigned int				i;
	CK_ACTIVE_ACTION::iterator	it;

	switch (AI->GetState())
	{
		case CSIG_INITIAL:
			Application.Window.CreateRenderingContext();

			//Creates a thread in order to speed up the initialization of the program
			hThread = CreateThread(
				NULL,               // default security attributes
				0,                  // use default stack size  
				FadingInAnimation,	// thread function name
				NULL,				// argument to thread function 
				0,                  // use default creation flags 
				&dwThreadId);		// returns the thread identifier 

			ReshapeGL();
			OutputDebugString(L"Space Invaders [End loading]");
			if (AAx4 == Application.Window.AntialiasingMode || AAx4S == Application.Window.AntialiasingMode)
				glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

			Navy->AI_Go2Play();

			//AI->outEvent("STARTING", NULL, this);	//CSIG_NameTransition[CSIG_STARTING]
			//OutEvent(CSIG_STARTING);//v Controlado 0->1
			AI->SetState(CSIG_FADING_IN);
			break;
		case CSIG_FADING_IN:
			if(!LevelReaded){
				SIGLB_AddLog(LogMessages[CSIG_READING_NEXT_LEVEL]);
				LoadLevel(Level);
				
				//Assign Sounds to all objects
				AssignSounds2Objects();
				Navy->SetSounds2Ships();	//Including all ships

				SIGLB_AddLog(LogMessages[CSIG_LOAD_SIM_KERNEL]);

				RTDESK_Engine->EndSimulation();
				if (DiscreteSimulation)
					LaunchInitialEvents();
				AssignRTDESK2Navy();
				LevelReaded	= true;
			}	
			ChangeRenderMode(NextRenderMode);
			Loading();
			//OutEvent(CSIG_FADING_OUT); //v Controlado 2->3
			AI->SetState(CSIG_FADING_OUT2PLAY);

			#ifdef CHW_USE_KINECT
			KinectManager.Init();
			#elif  CHW_USE_WIIMOTE
			WiimotesManager.Init();
			#endif
			break;
		case CSIG_FADING_OUT2PLAY:
			GUI.FadingOutLoading();			// Screen Fading Out
			break;
		case CSIG_PLAYING:
			if (DiscreteSimulation){
				TimerManager.GetTimer(SIGLBT_RTDESK_TIMING)->EndCounting();
				if (NextSimulation <= TimerManager.GetTimer(SIGLBT_RTDESK_TIMING)->GetLastPeriod()){
					#ifdef DEF_IDL_TIME_DISC
					TimerManager.EndAccCounting(SIGLBT_IDLE_TIMING);
					#endif
			
					NextSimulation=RTDESK_Engine->Simulate();

					TimerManager.GetTimer(SIGLBT_RTDESK_TIMING)->InitCounting();
					#ifdef DEF_IDL_TIME_DISC
					TimerManager.GetTimer(SIGLBT_IDLE_TIMING)->InitCounting();
					#endif					
				}
			}
			else{
				#ifdef DEF_IDL_TIME_CONT
				TimerManager.EndAccCounting(SIGLBT_IDLE_TIMING);
				#endif

				//Make input
				SIKeyboard.Update();

				NextSimulation = RTDESK_Engine->Simulate();

				#ifdef XM_UPD_TIME_CONT
				TimerManager.GetTimer(SIGLBT_UPDATE_TIMING)->InitCounting();
				#endif
				
				if (Timer[CSIG_UPD_PERIOD].IsSounding())
				{
					Timer[CSIG_UPD_PERIOD].ResetAlarm();
					Update();
				}
				
				#ifdef XM_UPD_TIME_CONT
				TimerManager.EndAccCounting(SIGLBT_UPDATE_TIMING);
				#endif
				
				//Render Phase
				if (Timer[CSIG_RND_PERIOD].IsSounding())
				{
					TimerManager.GetTimer(SIGLBT_RENDER_TIMING)->InitCounting();

					#ifdef XM_CONTINUOUS_WITH_SIMULATE_TIME
						Timer[CSIG_RND_PERIOD].ResetAlarm();
					#endif

					Render();

#ifdef XM_RND_TIME_CONT
					TimerManager.EndAccCounting(SIGLBT_RENDER_TIMING);
#endif
				}

				#ifdef DEF_IDL_TIME_CONT
				TimerManager.GetTimer(SIGLBT_IDLE_TIMING)->InitCounting();
				#endif
			}
			break;
		case CSIG_SAVING:
			Save(CSIG_SAVED_GAME);
			OutEvent(CSIG_SAVED);		//v Controlado  5->4   ///<Return to playing state
			break;
		case CSIG_PASSING_2_3D:
			#ifdef DEF_IDL_TIME_CONT
			TimerManager.EndAccCounting(SIGLBT_IDLE_TIMING);
			#endif

			if (CSIG_PASSING_2_3D != Semaphore)
			{
				Semaphore = CSIG_PASSING_2_3D;
				for (i = 0; i < Players.size(); i++)
					Players[i]->Pass2D3D();
				GUI.Passing2D23D = true;
				Semaphore = CSIG_INITIAL;	//Reset the semaphore...
			}
			ReshapeGL();
			//Render Phase
			if(Timer[CSIG_RND_PERIOD].IsSounding())
			{

				#ifndef XM_CONTINUOUS_WITH_SIMULATE_TIME
				lastTimeRnd = nowTime;
				#endif
				Timer[CSIG_RND_PERIOD].ResetAlarm();
				Render();
			}
			#ifdef DEF_IDL_TIME_CONT
			TimerManager.GetTimer(SIGLBT_IDLE_TIMING)->InitCounting();
			#endif

			break;
		case CSIG_INTERMISSION:
			//StartCameraMov();
			OutEvent(CD_INTERMISS_FINISHED);	//v Controlado  7->8 
			break;
		case CSIG_LOST_LIFE:
			if (--Players[CurrentPlayer]->Lives <= 0) {
				OutEvent(CSIG_FADING2LOST); //v Controlado 8->13
			}
			else
			{
				//ACHTUNG: Do something here before restarting to play
				OutEvent(CSIG_PLAY);  //v Controlado 8->4
			}
			break;
		case CSIG_WON:		
			PromoteLevel();
			break;
		case CSIG_LOST_GAME:
			Level = -1;
			PromoteLevel();
			OutEvent(CSIG_LOADING);		//v Controlado 10->2
			break;
		case CSIG_FADING_OUT2WON:
			SoundsManager.Play(CGS_APPLAUSE_SND, UGKSND_MAX_VOLUME);
			SoundsManager.SetPosition(CGS_APPLAUSE_SND,NULL);
			StartCameraMov();
			GUI.FadingInWon();
			OutEvent(CSIG_FADED);			//v Controlado 11->9
			break;
		case CSIG_FADING_OUT2END:
			///ACHTUNG: Aquí debería incluirse el código para eliminar a TODOS los objetos del sistema, no sólo a las SS
			///Esto debería además ser incluido como parte de la acción del estado LOST del juego
			
			//StartCameraMov();
			Deinitialize();
			AI->SetState(CSIG_END); // OutEvent(CSIG_2END);			//v Controlado 12->16
			break;
		case CSIG_FADING_OUT2LOST:
			OutEvent(CSIG_FADED);			//v Controlado 13->10
			break;
		case CSIG_PAUSED:
			if(!Paused)
				OutEvent(CSIG_CONTINUE);	//v Controlado 14->4
			break;
		case CSIG_WAIT_KEY:			
			GUI.DisplayPressKey();
			//OutEvent(CGS_KEYPRESSED);  //v Controlado 15->4
			//Activate time counters
			Timer[CSIG_UPD_PERIOD].Activate();
			Timer[CSIG_RND_PERIOD].Activate();
			AI->SetState(CSIG_PLAYING);

			//Set simulation up to date for starting just in the next loop
			RTDESK_Engine->SynchSim2RealTime(0.0);

			#ifdef CHW_USE_KINECT
				if (KinectManager.waitKinect() == true)
					OutEvent(CGS_KEYPRESSED);// start to play (key pressed or "wave" detected)
			#endif	
			break;
		case CSIG_END:
			if(!OnlyOneLevel)
				MainProgramLooping = false;
			else
				StopAfterOneLevel = true;
			break;
	}	//switch

	#ifdef CHW_USE_KINECT
	K_SHIP_MOVE dir_kinect = KinectManager.update();	// update hand position
	#elif CHW_USE_WIIMOTE
	#endif

	if (CSIG_PLAYING != AI->GetState() && CSIG_PAUSED != AI->GetState())	// Step function (loading) has its own swapbuffers
		Application.Window.Refresh();				// glFlush and Swap Buffers (Double Buffering)
					
}

void CSIGame::ReceiveMessage(RTDESK_CMsg *pMsg)
{
	unsigned int type;

	switch (pMsg->Type)
	{
	case UMSG_MSG_GAME:
		cMsgGame *auxMsg;
			auxMsg = (cMsgGame*)pMsg;

			switch (auxMsg->SubType)
			{
			case UMSG_RENDER:
				
					Render();		

					if(!EndByTime && !EndByFrame){
						#ifdef DEF_RTD_TIME
						TimerManager.GetTimer(SIGLBT_RTDSKMM_TIMING].InitCounting();
						#endif

						SendSelfMsg(GenericMsg[UMSG_RENDER], DefaultRndPeriod[CHARS_GAME]);

						#ifdef DEF_RTD_TIME
						TimerManager.EndAccCounting(SIGLBT_RTDSKMM_TIMING);
						#endif
					}
				
				break;
			case UMSG_UPDATE:
				
					DiscreteUpdate();		

					if(!EndByTime && !EndByFrame){
						#ifdef DEF_RTD_TIME
						TimerManager.GetTimer(SIGLBT_RTDSKMM_TIMING].InitCounting();
						#endif

						SendSelfMsg(GenericMsg[UMSG_UPDATE], DefaultUpdPeriod[CHARS_GAME]);

						#ifdef DEF_RTD_TIME
						TimerManager.EndAccCounting(SIGLBT_RTDSKMM_TIMING);
						#endif
					}
				
				break;
			default:
				break;
			}
		break;
	case UMSG_MSG_BASIC_TYPE:
		//Typically when a character has died, it sends this basic msg in order to notify it is out of play
		type = ((CCharacter*)pMsg->Sender)->Type;
		switch (type)
		{
		case CHARS_BONUS:
			//Bonus has collided to the player and inform about it to the game
			switch (((CBonus*)pMsg->Sender)->SubType)
			{
			case CBN_BONUS_3D:
				BonusManager->NextBonus	 = CBN_BONUS_LASER;
				Scene.AngleStart.v[XDIM] = fmod(Scene.Angle.v[XDIM], 360);
				Scene.AngleStart.v[YDIM] = fmod(Scene.Angle.v[YDIM], 360);
				OutEvent(CSIG_CHANGING_2_3D);
				break;
			case CBN_BONUS_LASER:
				BonusManager->NextBonus = CBN_BONUS_WEAPON;
				if (!Players[CurrentPlayer]->Laser[CPL_LEFT_LASER].Alive() || !Players[CurrentPlayer]->Laser[CPL_RIGHT_LASER].Alive())
					for (unsigned int i = 0; i < CPL_MAX_LASERS; i++)
						Players[CurrentPlayer]->ActivateLaser((CPL_PLAYER_LASER)i);
				break;
			case CBN_BONUS_WEAPON:
				if (Players[CurrentPlayer]->ShootType == CSH_PLAYER3D)
				{
					Players[CurrentPlayer]->ShootType = CSH_PLAYER3D_CHEVRON;
					for (unsigned int i = 0; i < CPL_MAX_LASERS;i++)
						((CLaser*)Players[CurrentPlayer]->Laser)[i].ShootType = CSH_AUX_LASER_CHEVRON;

				}
				break;
			case CBM_BONUS_HEAL:
			case CBM_BONUS_SHELL:
			default:;
			}
			break;
		case CHARS_PLAYER:
			//The player has lost its life
			OutEvent(CSIG_LIFE_LOST);	//v Controlado 4->7
			break;
		case CHARS_LASER:
			//One of the player's lasers has lost its life
			BonusManager->NextBonus = CBN_BONUS_LASER;
			break;
		case CHARS_SHIP:
			Score += (CS_MAXTYPE - type) * 10;
			if (Score > HiScore) HiScore = Score;
			break;
		case CHARS_CIRCLESHIP:
			Score += (int)ceil((float)rand() / 100);
			if (Score > HiScore) HiScore = Score;
			break;
		case CHARS_SUPPLYSHIP:
			Score += (CSS_MAXTYPE - Type) * 10;
			if (Score > HiScore) HiScore = Score;
			break;
		default:;
		}
		break;
	default:
		break;
	}
}

/**
* @brief Starts all initial events to allow the game to move
*
*/

void CSIGame::AssignRTDESK()
{
	RTDESK_TIME UpdT, RndT;
	RTDESK_CMsgDispatcher *MD;

	MD = RTDESK_Engine->GetMsgDispatcher();

	//Assign msg dispatcher to both the SI Game singleton and Navy one
	SetMsgDispatcher(MD);
	Navy->SetMsgDispatcher(MD);

	UpdT = TimerManager.GetTimer(SIGLBT_UPDATE_TIMING)->ms2Ticks(DefaultUpdPeriod[CHARS_PLAYER]);
	RndT = TimerManager.GetTimer(SIGLBT_UPDATE_TIMING)->ms2Ticks(DefaultRndPeriod[CHARS_PLAYER]);
	//Players
	for (unsigned int i = 0; i<Players.size(); i++) {
		Players[i]->Timer[CPL_UPD_PERIOD].SetAlarm(UpdT);
		Players[i]->Timer[CPL_RND_PERIOD].SetAlarm(RndT);
		Players[i]->SetMsgDispatcher(MD);
	}

	//ShootsManager
	ShootsManager->TimeRTdeskMsg = TimerManager.GetTimer(SIGLBT_UPDATE_TIMING)->ms2Ticks(DefaultUpdPeriod[CHARS_SHOOT_MNGR]);
	ShootsManager->SetMsgDispatcher(MD);

	//BonusManager
	BonusManager->TimeRTdeskMsg = TimerManager.GetTimer(SIGLBT_UPDATE_TIMING)->ms2Ticks(DefaultUpdPeriod[CHARS_BONUS_MNGR]);
	BonusManager->SetMsgDispatcher(MD);

	//Set auto timers
	Timer[CSIG_UPD_PERIOD].SetAlarm(UpdT);
	Timer[CSIG_RND_PERIOD].SetAlarm(RndT);
}

/**
* @brief Assign the message dispatcher to all the ships in the alien navy
*
*/
void CSIGame::AssignRTDESK2Navy()
{
	RTDESK_TIME UpdT, RndT;
	RTDESK_CMsgDispatcher *MD = GetMsgDispatcher();

	//Each game object wich is going to send events messages must have asigned one RTDESK_Engine Message Dispatcher.
	//Assignment of times to forward messages.
	CCharacter*	Ship;

	//Assign msg dispatcher to both the SI Game singleton and Navy one
	SetMsgDispatcher(MD);

	for (unsigned int i = 0; i < SceneGraph.Size(); i++)
	{
		Ship = SceneGraph.Object(i);
		switch (Ship->Type)
		{
		case CHARS_SHIP:
			UpdT = TimerManager.GetTimer(SIGLBT_UPDATE_TIMING)->ms2Ticks(DefaultUpdPeriod[CHARS_SHIP]);
			RndT = TimerManager.GetTimer(SIGLBT_UPDATE_TIMING)->ms2Ticks(DefaultRndPeriod[CHARS_SHIP]);
			break;
		case CHARS_SUPPLYSHIP:
			UpdT = TimerManager.GetTimer(SIGLBT_UPDATE_TIMING)->ms2Ticks(DefaultUpdPeriod[CHARS_SUPPLYSHIP]);
			RndT = TimerManager.GetTimer(SIGLBT_UPDATE_TIMING)->ms2Ticks(DefaultRndPeriod[CHARS_SUPPLYSHIP]);
			break;
		case CHARS_CIRCLESHIP:
			UpdT = TimerManager.GetTimer(SIGLBT_UPDATE_TIMING)->ms2Ticks(DefaultUpdPeriod[CHARS_CIRCLESHIP]);
			RndT = TimerManager.GetTimer(SIGLBT_UPDATE_TIMING)->ms2Ticks(DefaultRndPeriod[CHARS_CIRCLESHIP]);
			break;
		}

		switch (Ship->Type)
		{
		case CHARS_SHIP:
		case CHARS_SUPPLYSHIP:
		case CHARS_CIRCLESHIP:
			Ship->UpdateSF(TimerManager.GetSF());
			Ship->Timer[CS_UPD_PERIOD].SetAlarm(UpdT);
			Ship->Timer[CS_RND_PERIOD].SetAlarm(RndT);
			Ship->SetMsgDispatcher(MD);
		}
	}
}

/**
* @brief Starts all initial events to allow the game to move
*
*/

void CSIGame::LaunchInitialEvents()
{
	//Each game object wich is going to send events messages must have asigned one RTDESK_Engine Message Dispatcher.
	//Assignment of times to forward messages.
	CCharacter *Ship;

	//****************************************************************************************************//
	//Launching First Event Messages
	//Update Render and General Game Update
	SendSelfMsg(GenericMsg[UMSG_RENDER], TimerManager.GetTimer(SIGLBT_UPDATE_TIMING)->ms2Ticks(DefaultRndPeriod[CHARS_GAME]));
	SendSelfMsg(GenericMsg[UMSG_UPDATE], TimerManager.GetTimer(SIGLBT_UPDATE_TIMING)->ms2Ticks(DefaultUpdPeriod[CHARS_GAME]));

	for (unsigned int i = 0; i < SceneGraph.Size(); i++)
	{
		Ship = SceneGraph.Object(i);
		switch (Ship->Type)
		{
		case CHARS_SHIP:
		case CHARS_SUPPLYSHIP:
		case CHARS_CIRCLESHIP:
			msg = GetMsgToFill(UMSG_MSG_BASIC_TYPE);
			msg->Type = UMSG_MSG_BASIC_TYPE;
		}

		switch (Ship->Type)
		{
		case CHARS_SHIP:
			SendMsg(msg, Ship, Ship->Timer[CS_UPD_PERIOD].GetAlarmPeriodms());
			break;
		case CHARS_SUPPLYSHIP:
			Ship->MoveTo((float)i, 4.0f, 0.05f);
			SendMsg(msg, Ship, Ship->Timer[CSS_UPD_PERIOD].GetAlarmPeriodms());
			break;
		case CHARS_CIRCLESHIP:
			SendMsg(msg, Ship, Ship->Timer[CCS_UPD_PERIOD].GetAlarmPeriodms());
			break;
		}
	}

	//Update Player
	for(unsigned int i=0;i<Players.size();i++){	
		msg = GetMsgToFill(UMSG_MSG_BASIC_TYPE);
		msg->Type = UMSG_MSG_BASIC_TYPE;
		SendMsg(msg, Players[i], Players[i]->Timer[CPL_UPD_PERIOD].GetAlarmPeriodms());
	}
	
	//Execution in Game DiscreteUpdate
	//Update ShootsManager
	/*msg = GetMsgToFill(UMSG_MSG_BASIC_TYPE);
	msg->Type = UMSG_MSG_BASIC_TYPE;
	SendMsg(msg,&ShootsManager,TicksToUpdBonusMngr);*/

	//Update BonusManager
	/*msg = GetMsgToFill(UMSG_MSG_BASIC_TYPE);
	msg->Type = UMSG_MSG_BASIC_TYPE;
	SendMsg(msg,&BonusManager,TicksToUpdShootsMngr);*/
}

/**
@fn void CSIGame::InitTimers()
Initialize all the references to external objects for all the potential objects or objects managers in the game
*/
void CSIGame::InitObjectReferences()
{
	unsigned int i;

	//Init own objects references directory
	Directory.resize(CHARS_MAX_REFERENCES);
	Directory[CHARS_GAME_REF] = this;
	Directory[CHARS_TEXTURES_MNGR_REF] = (CCharacter *)&TexturesManager;

	//Assign references to the Game singleton to any object that requieres it
	//Bonuses and their manager
	BonusManager->SetGameRef(this);

	//Update Player
	for (i = 0; i < Players.size(); i++)
		Players[i]->SetGameRef(this);

	//Assign the Game singleton to all the navy
	Navy->SetGameRef(this);
}

/**
	@fn void CSIGame::InitTimers()
	Initialize all the timers for all the potential objects or objects managers in the game
*/
void CSIGame::InitTimers()
{
	CCharacter		*C;
	unsigned int	i;
	cHRTimer		*T;
	HRTM_SF			*SF;

	//Timers initialization
	if (HRTM_NO_TIMER_CREATED != TimerManager.CreateTimers(SIGLBT_TIMER_AMOUNT)) TimerManager.SetTimersName(TimersName, 0, SIGLBT_TIMER_AMOUNT);

	TimerManager.GetTimer(SIGLBT_APP_TIMING)->InitCounting();		//Start global application timer
	printf("Timers Frecuency = %lld\n", TimerManager.GetTimer(SIGLBT_APP_TIMING)->GetSamplingFreq());

	SF = TimerManager.GetSF();

	//Initialize own timers
	Timer[CSIG_UPD_PERIOD].SetSF(SF);
	Timer[CSIG_RND_PERIOD].SetSF(SF);

	//Initialize all the other timers used in the game
	Timer[CSIG_UPD_PERIOD].SetAlarm(SIGLBD_MIN_UPDATETIME_OBJECTS);
	Timer[CSIG_RND_PERIOD].SetAlarm(SIGLBD_MAX_RENDERTIME);
	
	Timer[CSIG_UPD_PERIOD].Activate();
	Timer[CSIG_RND_PERIOD].Activate();

	//Assign Timer Manager for the AniTimer of Texture Animation 

	//Assign Timer Manager for every Bonus Timer and BonusManager
	BonusManager->SetLocalTimers();
	BonusManager->Timer[CBN_UPD_PERIOD].SetAlarm(SIGLBD_MIN_UPDATETIME_OBJECTS);
	BonusManager->Timer[CBN_RND_PERIOD].SetAlarm(SIGLBD_MAX_RENDERTIME);

	BonusManager->Timer[CBN_UPD_PERIOD].Activate();
	BonusManager->Timer[CBN_RND_PERIOD].Activate();

	//Assign Timer Manager for BunkersManager
	ShootsManager->SetLocalTimers();

	ShootsManager->Timer[CSH_UPD_PERIOD].SetAlarm(SIGLBD_MIN_UPDATETIME_OBJECTS);
	ShootsManager->Timer[CSH_UPD_PERIOD].Activate();

	//Assign Timer Manager for the Timer of the navy
	Navy->Timer.resize	(CN_MAX_TIMERS);
	Navy->ChangeSF		(TimerManager.GetSF());

	//Assign Timer Manager for the Timer of the player and its objects
	for (i = 0; i < Players.size(); i++)
	{
		Players[i]->SetLocalTimers();
		Players[i]->SetRenderPeriod(DefaultRndPeriod[CHARS_PLAYER]);
		Players[i]->SetUpdatePeriod(DefaultUpdPeriod[CHARS_PLAYER]);
	}

	//Assign Render periods to any character that requires it
	SIKeyboard.RenderPeriod				= DefaultRndPeriod[CHARS_GAME];
	
	GUI.RenderTimer.SetSF(TimerManager.GetSF());
	GUI.RenderTimer.SetAlarm(DefaultRndPeriod[CHARS_GAME]);
	GUI.RenderTimer.Activate();

	//Assign Render Period for the whole navy: CircleShip, SupplyShip, Ships,...
	//Navy->SetRenderPeriod(DefaultRndPeriod[CHARS_NAVY]);

	//Assign Update period to all Supply Ships
	for (i = 0; i < SceneGraph.Size(); i++)
	{
		C = SceneGraph.Object(i);
		if (CHARS_SUPPLYSHIP == C->Type)
		{
			T = &(C->Timer[CSS_UPD_PERIOD]);
			T->SetSF(TimerManager.GetSF());
			T->SetAlarm(T->ms2Ticks(DefaultUpdPeriod[CHARS_SUPPLYSHIP]));
			T->Activate();
		}
	}
}


void CSIGame::Loading ()
{
	static bool LoadingTexturesLogged = false;
	unsigned int i;

	CCharacter *Ship;

	#ifdef CHAR_USE_QUADTREE	
		QTRoot->Clear();
	#endif

	// Loading 3D objects
	SIGLB_AddLog(LogMessages[CSIG_LOAD_MESHES]);
	try
	{
		Display();

		//Link every character in the game to its corresponding mesh
		SIGLB_AddLog(LogMessages[CSIG_MESHES2CHARS]);

		for(i=0;i<Players.size();i++){
			Players[i]->Mesh = MeshesManager.GetMesh(Players[i]->IndMesh);
			Players[i]->Laser[CPL_LEFT_LASER].Mesh = MeshesManager.GetMesh(Players[i]->Laser[CPL_LEFT_LASER].IndMesh);
			Players[i]->Laser[CPL_RIGHT_LASER].Mesh = MeshesManager.GetMesh(Players[i]->Laser[CPL_RIGHT_LASER].IndMesh);
			Players[i]->FitMeshIntoBoundingBox();
		}
	
		//Navy->SetMeshes(&MeshesManager);

		for (i = 0; i < SceneGraph.Size(); i++)
		{
			Ship = SceneGraph.Object(i);
			switch (Ship->Type)
			{
			case CHARS_SHIP:
				Ship->FitMeshIntoBoundingBox();
#ifdef CHAR_USE_QUADTREE
				Ship->UpdateCollisionDetection();
#endif
				if (BouncingMode) {
					float num = (30 + rand() % (170))*0.01f;	//0.3 <-> 2.0;
					Ship->Speed.v[XDIM] *= num;
					Ship->Speed.v[YDIM] *= num;
					Ship->Speed.v[XDIM] *= -1;
					Ship->Speed.v[YDIM] *= -1;
				}
				break;
			case CHARS_SUPPLYSHIP:
			case CHARS_CIRCLESHIP:
				Ship->FitMeshIntoBoundingBox();
			}
		}
		
		Background.Mesh = MeshesManager.GetMesh(Background.IndMesh);
		if (NULL != Background.Mesh)
			Background.Mesh->modelo.Materials->tex.texture[0] = TexturesManager.Textures[Background.IndTexture3D]->Texture.gl_id;
	}
	catch(...)
	{
		ERR(L"Exception: There was a problem during loading of the 3D objects.");
	}
}

// REPLACE CAMERA
void CSIGame::ReshapeGL(void)									// Reshape The Window When It's Moved Or Resized
{
	Application.Window.Reshape(45.0f);	///Reshape main Window
	Camera->Reshape(20.0f);	///Reshape the camera

	if (CHAR_2D != RenderMode)
	{
		Scene.Angle.v[YDIM] = -40;
		if (CPL_NO_PLAYER != CurrentPlayer)
			Players[CurrentPlayer]->Position.v[YDIM] = CPL_PLAYER_Y_3D;
	}
	else 
	{
		Scene.Angle.v[YDIM] = 0;
		if (CPL_NO_PLAYER != CurrentPlayer)
			Players[CurrentPlayer]->Position.v[YDIM] = CPL_PLAYER_Y_2D;
	}
#ifdef CHAR_USE_AABB
	if (CPL_NO_PLAYER != CurrentPlayer)
		Players[CurrentPlayer]->UpdateAABB();
#endif
	Scene.Angle.v[XDIM] = 0;

	//Set simulation up to date for starting just in the next loop
	if(DiscreteSimulation)
		RTDESK_Engine->SynchSim2RealTime(0.0);
}

void CSIGame::Snapshot( UGKS_String filename)
{
	int windowWidth=Application.Window.Width;
	int windowHeight=Application.Window.Height;

	byte* bmpBuffer = (byte*)malloc(windowWidth*windowHeight*3);
	if (0 == bmpBuffer)
	return;

	glReadPixels((GLint)0, (GLint)0,
	(GLint)windowWidth-1, (GLint)windowHeight-1,
	GL_BGR, GL_UNSIGNED_BYTE, bmpBuffer);

	FILE *filePtr = fopen(UGKS_string2CString(filename), "wb");
	if (!filePtr)
	return;

	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;

	bitmapFileHeader.bfType = 0x4D42; //"BM"
	bitmapFileHeader.bfSize = windowWidth*windowHeight*3;
	bitmapFileHeader.bfReserved1 = 0;
	bitmapFileHeader.bfReserved2 = 0;
	bitmapFileHeader.bfOffBits =
	sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biWidth = windowWidth-1;
	bitmapInfoHeader.biHeight = windowHeight-1;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = 24;
	bitmapInfoHeader.biCompression = BI_RGB;
	bitmapInfoHeader.biSizeImage = 0;
	bitmapInfoHeader.biXPelsPerMeter = 0; // ?
	bitmapInfoHeader.biYPelsPerMeter = 0; // ?
	bitmapInfoHeader.biClrUsed = 0;
	bitmapInfoHeader.biClrImportant = 0;

	fwrite(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	fwrite(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	fwrite(bmpBuffer, windowWidth*windowHeight*3, 1, filePtr);
	fclose(filePtr);

	free(bmpBuffer);
}