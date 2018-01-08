#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#define GLX_GLXEXT_LEGACY //Must be declared so that our local glxext.h is picked up, rather than the system one


//#include <windows.h>
#include "GameConstants.h"
#include "windowOGL.h"
#include "cWNDManager.h"
#include "cMaterial.h"
#include "cLight.h"
#include "cFontMgr.h"
#include "cCamera.h"
#include "cInputMgr.h"
#include "cSoundMgr.h"
#include "cModelLoader.h"
#include "cModel.h"
#include "cPlayer.h"
#include "cEnemy.h"
#include "cBullet.h"
#include "spaceGame.h"

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR cmdLine,
                   int cmdShow)
{

    //Set our window settings
    const int windowWidth = 1024;
    const int windowHeight = 768;
    const int windowBPP = 16;



    //This is our window
	static cWNDManager* pgmWNDMgr = cWNDManager::getInstance();

	// This is the input manager
	static cInputMgr* theInputManager = cInputMgr::getInstance();

	// This is the Font manager
	static cFontMgr* theFontManager = cFontMgr::getInstance();

	// This is the sound manager
	static cSoundMgr* theSoundManager = cSoundMgr::getInstance();
	
	//The example OpenGL code
    windowOGL theOGLWnd;

    //Attach our example to our window
	pgmWNDMgr->attachOGLWnd(&theOGLWnd);

	// Attach the keyboard manager
	pgmWNDMgr->attachInputMgr(theInputManager);


    //Attempt to create the window
	if (!pgmWNDMgr->createWND(windowWidth, windowHeight, windowBPP))
    {
        //If it fails

        MessageBox(NULL, "Unable to create the OpenGL Window", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); //Reset the display and exit
        return 1;
    }

	if (!theOGLWnd.initOGL(windowWidth, windowHeight)) //Initialize our example
    {
        MessageBox(NULL, "Could not initialize the application", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); //Reset the display and exit
        return 1;
    }

	// Create Texture map
	cTexture bossTexture;
	bossTexture.createTexture("Models/tardis.png");
	cTexture spaceShipTexture;
	spaceShipTexture.createTexture("Models/SpaceShip/sh3.jpg");
	cTexture laserTexture;
	laserTexture.createTexture("Models/laser.tga");
	cTexture playerTexture;
	playerTexture.createTexture("Models/fighter_texture.jpg");

	// Create Materials for lights
	cMaterial sunMaterial(lightColour4(0.0f, 0.0f, 0.0f, 1.0f), lightColour4(1.0f, 1.0f, 1.0f, 1.0f), lightColour4(1.0f, 1.0f, 1.0f, 1.0f), lightColour4(0, 0, 0, 1.0f), 5.0f);

	// Create Light
	cLight sunLight(GL_LIGHT0, lightColour4(0, 0, 0, 1), lightColour4(1, 1, 1, 1), lightColour4(1, 1, 1, 1), glm::vec4(0, 0, 20, 1),
		glm::vec3(0.0, 0.0, 1.0), 0.0f, 180.0f, 1.0f, 0.0f, 0.0f);
	cLight lfLight(GL_LIGHT1, lightColour4(0, 0, 0, 1), lightColour4(1, 1, 1, 1), lightColour4(1, 1, 1, 1), glm::vec4(30, 0, 100, 1),
		glm::vec3(0.0, 0.0, 1.0), 0.0f, 180.0f, 1.0f, 0.0f, 0.0f);
	cLight rfLight(GL_LIGHT2, lightColour4(0, 0, 0, 1), lightColour4(1, 1, 1, 1), lightColour4(1, 1, 1, 1), glm::vec4(-30, 0, 100, 1),
		glm::vec3(0.0, 0.0, 1.0), 0.0f, 180.0f, 1.0f, 0.0f, 0.0f);
	cLight cbLight(GL_LIGHT3, lightColour4(0, 0, 0, 1), lightColour4(1, 1, 1, 1), lightColour4(1, 1, 1, 1), glm::vec4(0, 0, -100, 1),
		glm::vec3(0.0, 0.0, 1.0), 0.0f, 180.0f, 1.0f, 0.0f, 0.0f);
	//Define Ambient light for scene
	GLfloat g_Ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, g_Ambient);

	// load game fonts
	// Load Fonts
	LPCSTR gameFonts[3] = { "Fonts/digital-7.ttf", "Fonts/space age.ttf", "Fonts/doctor_who.ttf" };

	theFontManager->addFont("SevenSeg", gameFonts[0], 24);
	theFontManager->addFont("Space", gameFonts[1], 30);
	theFontManager->addFont("DrWho", gameFonts[2], 48);

	// load game sounds
	// Load Sound
	LPCSTR gameSounds[3] = { "Audio/backgroundMusic.wav", "Audio/playerFire.wav", "Audio/enemyHit.wav" };

	theSoundManager->add("Theme", gameSounds[0]);
	theSoundManager->add("Shot", gameSounds[1]);
	theSoundManager->add("Explosion", gameSounds[2]);

	// Create a camera
	cCamera theCamera;
	theCamera.setTheCameraPos(glm::vec3(10.0f, 50.0f, 10.0f));
	theCamera.setTheCameraLookAt(glm::vec3(0.0f, 0.0f, 0.0f));
	theCamera.setTheCameraUpVector(glm::vec3(0.0f, 1.0f, 0.0f));
	theCamera.setTheCameraAspectRatio(windowWidth, windowHeight);
	theCamera.setTheProjectionMatrix(45.0f, theCamera.getTheCameraAspectRatio(), 0.1f, 300.0f);
	theCamera.update();

	//Clear key buffers
	theInputManager->clearBuffers(theInputManager->KEYS_DOWN_BUFFER | theInputManager->KEYS_PRESSED_BUFFER);

	// Model
	cModelLoader playerModel;
	playerModel.loadModel("Models/fighter.obj", playerTexture); // Player model

	cModelLoader spaceShipModel;
	spaceShipModel.loadModel("Models/SpaceShip/Sample_Ship.obj", spaceShipTexture); // Standard enemy model

	cModelLoader bossModel;
	bossModel.loadModel("Models/tardis.obj", bossTexture); // Boss enemy model
	
	cModelLoader bulletModel;
	bulletModel.loadModel("Models/laser.obj", laserTexture); // Bullet model

	// Main enemy loop
	for (int loop = 0; loop < 40; loop++)
	{
		enemies.push_back(new cEnemy);
		enemies[loop]->randomise();
		enemies[loop]->setMdlDimensions(spaceShipModel.getModelDimensions());
		enemies[loop]->setScale(glm::vec3(5, 5, 5));
	}

	// Boss loop
	for (int loop = 0; loop < 5; loop++)
	{
		bossEnemies.push_back(new cEnemy);
		bossEnemies[loop]->randomise();
		bossEnemies[loop]->setMdlDimensions(bossModel.getModelDimensions());
		bossEnemies[loop]->setScale(glm::vec3(1, 1, 1));
	}
	
	// Create the player 
	cPlayer thePlayer;
	thePlayer.initialise(glm::vec3(0, 0, 0), 0.0f, glm::vec3(1, 1, 1), glm::vec3(0, 0, 0), 5.0f, true);
	thePlayer.setMdlDimensions(playerModel.getModelDimensions());
	thePlayer.attachInputMgr(theInputManager);
	thePlayer.attachSoundMgr(theSoundManager);

	float tCount = 0.0f;
	string enemiesLeft;
	string bossEnemiesLeft;

	// Play the theme sound
	theSoundManager->getSnd("Theme")->playAudio(AL_LOOPING);

	// Bullet list
	std::vector<cBullet*> bulletList;
	std::vector<cBullet*>::iterator index;

   //This is the mainloop, we render frames until isRunning returns false
	while (pgmWNDMgr->isWNDRunning())
    {
		pgmWNDMgr->processWNDEvents(); //Process any window events

        //We get the time that passed since the last frame
		float elapsedTime = pgmWNDMgr->getElapsedSeconds();
		
		// Clear buffer and initialise window dimensions
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		theOGLWnd.initOGL(windowWidth,windowHeight);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixf((GLfloat*)&theCamera.getTheViewMatrix());

		sunMaterial.useMaterial();
		sunLight.lightOn();
		lfLight.lightOn();
		rfLight.lightOn();
		cbLight.lightOn();

		for (vector<cEnemy*>::iterator enemyIterator = enemies.begin(); enemyIterator != enemies.end(); ++enemyIterator)
		{
			if ((*enemyIterator)->isActive())
			{
				spaceShipModel.renderMdl((*enemyIterator)->getPosition(), (*enemyIterator)->getRotation(), (*enemyIterator)->getScale());
				(*enemyIterator)->update(elapsedTime);
			}
		}

		for (vector<cEnemy*>::iterator bossIterator = bossEnemies.begin(); bossIterator != bossEnemies.end(); ++bossIterator)
		{
			if ((*bossIterator)->isActive())
			{
				bossModel.renderMdl((*bossIterator)->getPosition(), (*bossIterator)->getRotation(), (*bossIterator)->getScale());
				(*bossIterator)->update(elapsedTime);
			}
		}

		playerModel.renderMdl(thePlayer.getPosition(), thePlayer.getRotation(), thePlayer.getScale());
		thePlayer.update(elapsedTime);
		
		for (vector<cBullet*>::iterator bulletIterartor = bullets.begin(); bulletIterartor != bullets.end(); ++bulletIterartor)
		{
			if ((*bulletIterartor)->isActive())
			{
				bulletModel.renderMdl((*bulletIterartor)->getPosition(), (*bulletIterartor)->getRotation(), (*bulletIterartor)->getScale());
				(*bulletIterartor)->update(elapsedTime);
			}
		}

		enemiesLeft = to_string(enemies.size()); // convert float to string
		bossEnemiesLeft = to_string(bossEnemies.size()); 
		
		glPushMatrix();
		theOGLWnd.setOrtho2D(windowWidth, windowHeight);
		theFontManager->getFont("Space")->printText("Space Shooter", FTPoint(10, 35, 0.0f), colour3f(0.0f,255.0f,0.0f));

		theFontManager->getFont("Space")->printText("Enemies Left: ", FTPoint(622, 35, 0.0f), colour3f(0.0f, 255.0f, 0.0f));
		theFontManager->getFont("Space")->printText(enemiesLeft.c_str(), FTPoint(950, 35, 0.0f), colour3f(255.0f, 255.0f, 0.0f));

		theFontManager->getFont("Space")->printText("Boss Enemies Left: ", FTPoint(500, 55, 0.0f), colour3f(0.0f, 255.0f, 0.0f));
		theFontManager->getFont("Space")->printText(bossEnemiesLeft.c_str(), FTPoint(950, 55, 0.0f), colour3f(255.0f, 255.0f, 0.0f));
		glPopMatrix();

		pgmWNDMgr->swapBuffers();

		tCount += elapsedTime;

		//Clear key buffers
		theInputManager->clearBuffers(theInputManager->KEYS_DOWN_BUFFER | theInputManager->KEYS_PRESSED_BUFFER);

	}

	theOGLWnd.shutdown(); //Free any resources
	pgmWNDMgr->destroyWND(); //Destroy the program window

    return 0; //Return success
}
