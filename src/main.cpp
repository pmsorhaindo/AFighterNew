/////////////////////////////////////////////////////////////////////////////
// MD3 Model Loader originally Copyright (c) 2007 David Henry
// last modification: may. 7, 2007
// Licenced under the MIT license.
// Heavily modified and improved upon by David Lee and Paul-Michael Sorhaindo 2010
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
// AFighter!! Like Street Fighter...but better (As endorsed by Ziggy)
// Main.cpp of AFighter Project.
//
//===========================================================================

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdarg>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include <Vectormath/cpp/Vectormath_aos.h>
#include <time.h>

#include "Md3Player.h"
#include "TextureManager.h"
#include "Rules.h"

using std::vector;
using std::cout;
using std::cerr;
using std::endl;

struct boundingSphere
{
	Vectormath::Aos::Vector3 origin;
	float radius;
};

// Keyboard
struct keyboard_input_t
{
	unsigned char keymap[256];
	int special[256];
	int modifiers;
} keyboard;

// Timer
struct glut_timer_t
{
	double current_time;
	double last_time;

} timer;

// Camera
struct Vector_3d
{
	float x, y, z;

} rot, eye;

// Globals...   :(

// The Players
Md3Player *player = NULL;
Md3Player *player2= NULL;
// The Player Positions
Vectormath::Aos::Vector3 position = Vectormath::Aos::Vector3 (-3000.0,0.0,0.0); // vector holding the translate
Vectormath::Aos::Vector3 position2 = Vectormath::Aos::Vector3 (3000.0,0.0,0.0); //amount relative to the origin
//The Positions of the Walls
Vectormath::Aos::Vector3 leftWall = Vectormath::Aos::Vector3 (-4100.0, 0.0, 0.0);
Vectormath::Aos::Vector3 rightWall = Vectormath::Aos::Vector3 (4100.0, 0.0, 0.0);
//A Rules/Physics instance for each Player
Rules * player1rules = new Rules(0.0, -1000, 0.0, 5.0, -1000, 0.0, leftWall, rightWall);
Rules * player2rules = new Rules(0.0, -1000, 0.0, 5.0, -1000, 0.0, leftWall, rightWall);
//A projectile model for each player.
Md3Player *projectile = NULL;
Md3Player *projectile2 = NULL;
//A Projectile position for each player.
Vectormath::Aos::Vector3 projectilePos;
Vectormath::Aos::Vector3 projectilePos2;

//These booleans allow us to turn stuff
bool bTextured = true;
bool bLightGL = true;
bool bAnimated = true;
// Player collision Flag
bool collided = false;
//Player active projectile flags
bool fire = false;
bool fire2 = false;
//Player active crouch flags
bool crouch = false;
bool crouch2 = false;
//Player dead/Game over flags
bool dead = false;
bool dead2 = false;
//Player health points for each
float health = 1000.0;
float health2 = 1000.0;
//A switch to toggle between All text to None to just FPS info
int verbose = 2;
//
int fps = 0;

vector<string> skinList;

// Bounding Spheres for player's an Projectile's for collision detection.
boundingSphere player1Sphere;
boundingSphere player2Sphere;
boundingSphere projectileSphere;
boundingSphere projectileSphere2;
// -------------------------------------------------------------------------
// shutdownApp
//
// Application termination.
// -------------------------------------------------------------------------

static void
shutdownApp ()
{
	// free memory!
	Texture2DManager::kill ();
	delete player;
	delete player2;
	delete projectile;
	delete projectile2;
	delete player1rules;
	delete player2rules;
}


// -------------------------------------------------------------------------
// init
//
// Application initialization.  Setup keyboard input, timer, camera and OpenGL.
// -------------------------------------------------------------------------

static void
init (const string &playerpath, const string &weaponpath)
{
	//
	// GLEW Initialization
	//

	GLenum err = glewInit ();
	if (GLEW_OK != err)
	{
		cerr << "Error: " << glewGetErrorString (err) << endl;
		shutdownApp ();
	}

	//
	// Application initialization
	//

	// Initialize keyboard
	memset (keyboard.keymap, 0, 256);
	memset (keyboard.special, 0, 256);

	// Initialize timer
	timer.current_time = 0;
	timer.last_time = 0;

	// Initialize camera input
	rot.x = 0.0f;   eye.x = 0.0f;
	rot.y = 0.0f;   eye.y = 0.0f;
	rot.z = 0.0f;   eye.z = 8.0f;

	// Get base dir for player if a *.md3 file is given
	string playerdir (playerpath);
	if (playerdir.find (".md3") == playerdir.length () - 4)
	#ifdef _WIN32
    playerdir.assign (playerdir, 0, playerdir.find_last_of ('\\'));
	#else
	playerdir.assign (playerdir, 0, playerdir.find_last_of ('/'));
	#endif

	// Initialize MD3 player
	try
	{
		// Load MD3 player
		player = new Md3Player (playerdir);
		player->setScale (0.05f);
		player->setAnimation (kTorsoStand);
		player->setAnimation (kLegsIdle);

		projectile = new Md3Player ("models\\dalek");
		projectile->setScale (0.01f);
		projectile->setAnimation (kTorsoStand);
		projectile->setAnimation (kLegsIdle);

		// Load 2nd MD3 player
		projectile2 = new Md3Player ("models\\dalek");
		projectile2->setScale (0.01f);
		projectile2->setAnimation (kTorsoStand);
		projectile2->setAnimation (kLegsIdle);

		player2 = new Md3Player (playerdir);
		player2->setScale (0.05f);
		player2->setAnimation (kTorsoStand);
		player2->setAnimation (kLegsIdle);
		string colour = "blue";
		player2->setSkin(colour);
	}

	// Error catching awesomeness
	catch (std::runtime_error &err)
	{
		cerr << "Error: failed to load stuff!" << endl;
		cerr << "Reason: " << err.what () << endl;
		exit (-1);
    }

  //
  // Initialize OpenGL
  //

  glClearColor (0.5f, 0.5f, 5.5f, 1.0f);
  glShadeModel (GL_SMOOTH);

  glEnable (GL_DEPTH_TEST);
  glEnable (GL_TEXTURE_2D);
  glEnable (GL_CULL_FACE);
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);

  glCullFace (GL_BACK);

}

// -------------------------------------------------------------------------
// reshape
//
// OpenGL window resizing.  Update the viewport and the projection matrix.
// -------------------------------------------------------------------------

static void
reshape (int w, int h)
{
	if (h == 0)
		h = 1;

	glViewport (0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (45.0, w / static_cast<GLfloat>(h), 0.1, 1000.0);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	glutPostRedisplay ();
}

// -------------------------------------------------------------------------
// updateTimer
//
// Update the timer.
// -------------------------------------------------------------------------

static void
updateTimer (struct glut_timer_t *t)
{
	t->last_time = t->current_time;
	t->current_time = glutGet (GLUT_ELAPSED_TIME) * 0.001f;
}

// -------------------------------------------------------------------------
// begin2D
//
// Enter into 2D mode.
// -------------------------------------------------------------------------

static void
begin2D ()
{
	int width = glutGet (GLUT_WINDOW_WIDTH);
	int height = glutGet (GLUT_WINDOW_HEIGHT);

	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();

	glLoadIdentity ();
	glOrtho (0, width, 0, height, -1.0f, 1.0f);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}

// -------------------------------------------------------------------------
// end2D
//
// Return from 2D mode.
// -------------------------------------------------------------------------

static void
end2D ()
{
	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();

	glMatrixMode (GL_MODELVIEW);
}


// -------------------------------------------------------------------------
// glPrintf
//
// printf-like function for OpenGL.
// -------------------------------------------------------------------------

static int
glPrintf (const char *format, ...)
{
	char buffer[1024];
	std::va_list arg;
	int ret;

	// Format the text
	va_start (arg, format);
    ret = std::vsnprintf (buffer, sizeof (buffer), format, arg);
    va_end (arg);

    // Print it
    for (unsigned int i = 0; i < strlen (buffer); ++i)
    glutBitmapCharacter (GLUT_BITMAP_HELVETICA_12, buffer[i]);

    return ret;
}


// -------------------------------------------------------------------------
// gameLogic
//
// Perform application logic.
// -------------------------------------------------------------------------

static void
gameLogic ()
{
	// Calculate frame per seconds
	static double current_time = 0;
	static double last_time = 0;
	static int n = 0;

	n++;
	current_time = timer.current_time;

	if ((current_time - last_time) >= 1.0)
    {
		fps = n;
		n = 0;
		last_time = current_time;
    }

	// Animate player
	if (bAnimated)
	{
		double dt = timer.current_time - timer.last_time;
		player->animate (dt);
		player2->animate(dt);
		projectile->animate(dt);
		projectile2->animate(dt);
	}
}

//
bool checkProjectileCollision()
{
	float difference = player1rules->checkDistanceBetweenSpheres(player2Sphere.origin, projectileSphere.origin);
	difference += 1200;
	if (player1rules->checkForCollision(difference, player2Sphere.radius, projectileSphere.radius))
	{
		if(crouch2)
		{
			health2-=5;
		}
		else
		{
			health2-=25;
			//cout << "health 2 : " << health2 << endl;
			player2->setAnimation(static_cast<Md3PlayerAnimType>(0));
		}
		if (health2 <= 0)
		{
			//cout << "dead yet?";
			dead2 =true;
			player2->setAnimation(static_cast<Md3PlayerAnimType>(1));
		}
		return true;
	}
	return false;
}

bool checkProjectileCollision2()
{
	float difference = player2rules->checkDistanceBetweenSpheres(player1Sphere.origin, projectileSphere2.origin);
	difference += 1200; // possible could be "-"
	if (player2rules->checkForCollision(difference, player1Sphere.radius, projectileSphere2.radius))
	{
		if (crouch)
		{
			health-=5;
		}
		else
		{
			health-=25;
			player->setAnimation(static_cast<Md3PlayerAnimType>(0));
		}
		if (health <= 0)
				{
					dead =true;
					player->setAnimation(static_cast<Md3PlayerAnimType>(1));
				}
		return true;
	}
	return false;
}


// -------------------------------------------------------------------------
// draw3D
//
// Render the 3D part of the scene.
// -------------------------------------------------------------------------

static void
draw3D ()
{
	// Clear window
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity ();

	// Perform camera transformations
	glTranslated (-eye.x, -eye.y, -eye.z);
	glRotated (rot.x, 1.0f, 0.0f, 0.0f);
	glRotated (rot.y, 0.0f, 1.0f, 0.0f);
	glRotated (rot.z, 0.0f, 0.0f, 1.0f);

	glEnable (GL_DEPTH_TEST);

	if (bLightGL)
    glEnable (GL_LIGHTING);

	if (bTextured)
	glEnable (GL_TEXTURE_2D);

	//Using the force!!
	// if(player1rules->getForceExerted()[0] > 0 || player1rules->getForceExerted()[1] > 0 || player1rules->getForceExerted()[2] > 0)
	// {
	//Vectormath::Aos::Vector3 exert;
	//exert.setX(player1rules->getForceExerted()[0]);
	//exert.setY(player1rules->getForceExerted()[1]);
	//exert.setZ(player1rules->getForceExerted()[2]);
	//player1rules->setForceExerted(exert);

	/*Vectormath::Aos::Vector3 newTranslate;
	newTranslate = player1rules->resultantForce(player1rules->getForceExerted(),player1rules->getForceActingOn());


    position[0]= newTranslate.getX();
    position[1]= newTranslate.getY();
    position[2]= newTranslate.getZ();*/

	//}

	// Moving and drawing objects
	// player 1
	glPushMatrix();

	if(!player1rules->checkForFloor(position))
	{
		position[1] = position[1] - player1rules->getGravity();
	}

	glTranslatef (position[0]*0.001,position[1]*0.001,position[2]*0.001);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	player->draw ();
	glPopMatrix();

	// player 2
	glPushMatrix();
	if(!player2rules->checkForFloor(position2))
	{
		position2[1] = position2[1] - player2rules->getGravity();
	}

	glTranslatef (position2[0]*0.001,position2[1]*0.001,position2[2]*0.001);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);

	player2->draw ();
	glPopMatrix();

	// update bounding spheres
	if (crouch)
	{
		glPushMatrix();
		player1Sphere.origin = position;
		player1Sphere.origin[1] += 100;
		glTranslatef(player1Sphere.origin[0],player1Sphere.origin[1]+1000,player1Sphere.origin[2]);
		//glutWireSphere(player1Sphere.radius*0.0007,25,25); // debug
		glPopMatrix();
	}
	else
	{
		glPushMatrix();
		player1Sphere.origin = position;
		glTranslatef(player1Sphere.origin[0],player1Sphere.origin[1]+1000,player1Sphere.origin[2]);
		//glutWireSphere(player1Sphere.radius*0.0007,25,25); // debug
		glPopMatrix();
	}

	if (crouch2)
	{
		glPushMatrix();
		player2Sphere.origin = position2;
		player2Sphere.origin[1] -= 100;
		glTranslatef(player2Sphere.origin[0],player2Sphere.origin[1]-3,player2Sphere.origin[2]);
		//glutWireSphere(player2Sphere.radius*0.0007,25,25); // debug
		glPopMatrix();
	}
	else
	{
		glPushMatrix();
		player2Sphere.origin = position2;
		glTranslatef(player2Sphere.origin[0],player2Sphere.origin[1]-3,player2Sphere.origin[2]);
		//glutWireSphere(player2Sphere.radius*0.007,25,25); // debug
		glPopMatrix();
	}


	// projectile firing player 1
	if (!fire && !crouch)
	{
		projectilePos[0] = position[0];
		projectileSphere.origin = projectilePos;
	}

	else if (!fire && !crouch2)
	{
		projectilePos[0] = position[0];
		projectileSphere.origin = projectilePos;
	}

	// projectile firing player 2
	if (!fire2 && !crouch2)
	{
		projectilePos2[0] = position2[0];
		projectileSphere2.origin = projectilePos2;
	}
	else if (!fire2 && !crouch)
	{
		projectilePos2[0] = position2[0];
		projectileSphere2.origin = projectilePos2;
	}


	if (fire)
	{
		if (!crouch)
		{
			glPushMatrix();
			projectilePos[0] = projectilePos[0] + 100;
			glTranslatef (projectilePos[0]*0.001, projectilePos[1]*0.001, projectilePos[2]*0.001);
			projectile->draw();
			projectileSphere.origin = projectilePos;
			if (checkProjectileCollision())
			{
				player->setAnimation (kTorsoStand);
				projectilePos[0] = -4300;
				fire = false;
			}
			if (projectilePos[0] >= 4200)
			{
				player->setAnimation (kTorsoStand);
				fire = false;
			}
			glPopMatrix();
		}
		else if (crouch)
		{
			glPushMatrix();
			projectilePos[0] = projectilePos[0] + 100;
			glTranslatef (projectilePos[0]*0.001, projectilePos[1]*0.001, projectilePos[2]*0.001);
			glTranslatef(0.0, -1.0, 0.0);
			projectile->draw();
			projectileSphere.origin = projectilePos;
			if (checkProjectileCollision())
			{
				player->setAnimation (kTorsoStand);
				projectilePos[0] = -4300;
				fire = false;
			}
			if (projectilePos[0] >= 4200)
			{
				player->setAnimation (kTorsoStand);
				fire = false;
			}
			glPopMatrix();
	 	  }
	  }

	if (fire2)
	{
		if (!crouch2)
		{
			glPushMatrix();
			projectilePos2[0] = projectilePos2[0] - 100;
			glTranslatef (projectilePos2[0]*0.001, projectilePos2[1]*0.001, projectilePos2[2]*0.001);
			projectile2->draw();
			projectileSphere2.origin = projectilePos2;
			if (checkProjectileCollision2())
			{
				player2->setAnimation (kTorsoStand);
				projectilePos2[0] = -4200;
				fire2 = false;
			}
			if (projectilePos2[0] <= -4200)
			{
				player2->setAnimation (kTorsoStand);
				fire2 = false;
			}
			glPopMatrix();
		}
		else if (crouch2)
		{
			glPushMatrix();
			projectilePos2[0] = projectilePos2[0] - 100;
			glTranslatef (projectilePos2[0]*0.001, projectilePos2[1]*0.001, projectilePos2[2]*0.001);
			glTranslatef(0.0, -1.0, 0.0);
			projectile2->draw();
			projectileSphere2.origin = projectilePos2;
			if (checkProjectileCollision2())
			{
				player2->setAnimation (kTorsoStand);
				projectilePos2[0] = -4200;
				fire2 = false;
			}
			if (projectilePos2[0] <= -4200)
			{
				player2->setAnimation (kTorsoStand);
				fire2 = false;
			}
			glPopMatrix();
		}
	}
	glDisable (GL_LIGHTING);
	glDisable (GL_TEXTURE_2D);
	glDisable (GL_DEPTH_TEST);
}

// -------------------------------------------------------------------------
// draw2D
//
// Render the 2D part of the scene.
// -------------------------------------------------------------------------

static void
draw2D ()
{
	begin2D ();

	glColor3f (1.0f, 1.0f, 1.0f);
	glRasterPos2i (10, 30);
	char temp1[64];
	sprintf(temp1,"%G",health);

	glPrintf ("Player 1 Health: ");
	glPrintf (temp1);

	glRasterPos2i (500, 30);
	char temp2[64];
	sprintf(temp2,"%G",health2);

	glPrintf ("Player 2 Health: ");
	glPrintf (temp2);

	if (dead)
	{
		glRasterPos2i (250, glutGet (GLUT_WINDOW_HEIGHT) - 250);
		glPrintf ("GAME OVER :: PLAYER 2 WINS");
	}

	if (dead2)
	{
		glRasterPos2i (250, glutGet (GLUT_WINDOW_HEIGHT) - 250);
		glPrintf ("GAME OVER :: PLAYER 1 WINS");
	}

	if (verbose > 0)
	{
		glRasterPos2i (10, 10);
		glPrintf ("FPS: %i", fps);
	}

	if (verbose > 1)
	{
		glRasterPos2i (10, glutGet (GLUT_WINDOW_HEIGHT) - 20);
		glPrintf ("AFighter v0.6 (proper Boost Library usag3)");

		glRasterPos2i (10, glutGet (GLUT_WINDOW_HEIGHT) - 35);
		glPrintf ("Yes it really is this awesome");
	}
	end2D ();
}


// -------------------------------------------------------------------------
// display
//
// Render the main scene to the screen.
// -------------------------------------------------------------------------

static void
display ()
{
	gameLogic ();
	draw3D ();
	draw2D ();
	glutSwapBuffers ();
}

//---------------------------------------------------------------------------
//
// This is the awesome Sleep method :D!!!
//
//---------------------------------------------------------------------------
/*
void sleep(unsigned int timeToPause)
{
	clock_t goal= timeToPause + clock();
	while (goal > clock());
}
*/
//---------------------------------------------------------------------------
//
// This method won't return squat.. it'll just do it all....
// ... The Jump Method. v3
//
//---------------------------------------------------------------------------

/*
void jump(Vectormath::Aos::Vector3 &position)
{

	  for (int i =0; i<100;i++)
	  {

	    float temp = position.getY()+10;
		  position.setY(temp);
		  draw3D();
	  i++;
	  }
}

*/

//========================================================================================
//
// Model to Model Collision Checking
//
//========================================================================================


int collisionChecking()
{
	if (player1rules->checkForCollision((player1rules->checkDistanceBetweenSpheres(player1Sphere.origin, player2Sphere.origin)), player1Sphere.radius, player2Sphere.radius))
	{
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
//keyUp
//
//
//--------------------------------------------------------------------------
static void
keyUp (unsigned char key, int x, int y)
{
	// Update key state
	keyboard.keymap[key] = 1;

	if (!dead)
	{
		if (key == 'w' || key == 'W')
		{
			player->setAnimation(static_cast<Md3PlayerAnimType>(11));
			player->setAnimation(static_cast<Md3PlayerAnimType>(22));
		}
		if (key == 'q' || key == 'Q')
		{
			player->setAnimation(static_cast<Md3PlayerAnimType>(11));
			player->setAnimation(static_cast<Md3PlayerAnimType>(22));
		}
		if (key == 'a' || key == 'A')
		{
			player->setAnimation(static_cast<Md3PlayerAnimType>(11));
			player->setAnimation(static_cast<Md3PlayerAnimType>(22));
		}
		if (key == 'd' || key == 'D')
		{
			player->setAnimation(static_cast<Md3PlayerAnimType>(11));
			player->setAnimation(static_cast<Md3PlayerAnimType>(22));
		}
	}

}

static void
specialKeyUp(int key, int x, int y)
{
	keyboard.special[key] = 1;

	if (!dead2)
	{
		switch(key)
		{
		case GLUT_KEY_UP :
			player2->setAnimation(static_cast<Md3PlayerAnimType>(11));
			player2->setAnimation(static_cast<Md3PlayerAnimType>(22));
			break;
		case GLUT_KEY_PAGE_UP :
			player2->setAnimation(static_cast<Md3PlayerAnimType>(11));
			player2->setAnimation(static_cast<Md3PlayerAnimType>(22));
			break;
		case GLUT_KEY_RIGHT :
			player2->setAnimation(static_cast<Md3PlayerAnimType>(11));
			player2->setAnimation(static_cast<Md3PlayerAnimType>(22));
			break;
		case GLUT_KEY_LEFT :
			player2->setAnimation(static_cast<Md3PlayerAnimType>(11));
			player2->setAnimation(static_cast<Md3PlayerAnimType>(22));
			break;
		}
	}
}

// -------------------------------------------------------------------------
// keyPress
//
// Key press glut callback function.  Called when user press a key.
// -------------------------------------------------------------------------

static void
keyPress (unsigned char key, int x, int y)
{
	// Update key state
	keyboard.keymap[key] = 1;

	// Handle here ponctual actions when
	// a key is pressed (like toggle ligthing)
	// Escape
	if (key == 27)
		exit (0);
	if (!dead)
	{
		if (key == 'w' || key == 'W')
		{
/*	Vectormath::Aos::Vector3 jumpForce;
	jumpForce.setX(player1rules->getForceExerted()[0]+0.0);
	jumpForce.setY(player1rules->getForceExerted()[0]+500.0);
	jumpForce.setZ(player1rules->getForceExerted()[0]+0.0);
	player1rules->setForceExerted(jumpForce);
*/
			player->setAnimation(static_cast<Md3PlayerAnimType>(18));
			crouch = false;
		}

		if (key == 'a' || key == 'A')
		{
			if (!player1rules->checkLeftWallCollision(position))
			{
				if (crouch)
				{
					position[0]-=300.0;
					player->setAnimation(static_cast<Md3PlayerAnimType>(13));
				}
				else
				{
					position[0]-=300.0;
					player->setAnimation(static_cast<Md3PlayerAnimType>(16));
				}
			}
		}
		if (key == 'd' || key == 'D')
		{
			if (!collisionChecking())
			{
				if (crouch)
				{
					position[0]+=300.0;
					player->setAnimation(static_cast<Md3PlayerAnimType>(13));
				}
				else
				{
					position[0]+=300.0;
					player->setAnimation(static_cast<Md3PlayerAnimType>(15));
				}
			}
		}
		if (key == 's' || key == 'S')
		{
			if (crouch)
			{
				player1Sphere.origin[1] -=10;
				player->setAnimation(static_cast<Md3PlayerAnimType>(11));
				player->setAnimation(static_cast<Md3PlayerAnimType>(22));
				crouch = false;
			}
			else
			{
				player1Sphere.origin[1] +=10;
				player->setAnimation(static_cast<Md3PlayerAnimType>(23)); //There is no animtype created it takes an int so we cast an int... like alien wizards!
				crouch = true;
			}
		}

		if (key == 'e' || key == 'E')
		{
			//fireProjectile(position,1);
			//fireProjectile(direction);
			if (crouch)
			{
				projectileSphere.origin[1] -= 1250;
				//cout << "debug" << endl;
			}
			player->setAnimation(static_cast<Md3PlayerAnimType>(8));
			fire = true;
		}

		if (key == 'q' || key == 'Q')
		{
			player->setAnimation(static_cast<Md3PlayerAnimType>(20));
			if(collisionChecking()&&crouch2)
			{
				health2-=50;
				player2->setAnimation(static_cast<Md3PlayerAnimType>(0));
			}
			if (health2 <= 0)
	  		{
				//cout << "dead yet?";
				dead2 =true;
				player2->setAnimation(static_cast<Md3PlayerAnimType>(1));
	  		}
		}

		// Other defaults

		if (key == 'v' || key == 'V')
			verbose = (verbose + 1) % 3;

		if (key == '7' || key == '7')
			glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

		if (key == '6' || key == '6')
			glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

		if (key == '8' || key == '8')
			bTextured = !bTextured;

		if (key == '1' || key == '1')
		{
			delete player;
			player = new Md3Player ("models\\lara");
			player->setScale (0.05f);
			player->setAnimation (kTorsoStand);
			player->setAnimation (kLegsIdle);
		}

		glutPostRedisplay ();
	}

	if (key == '9' || key == '9')
	{
		health = 1000.0;
		health2 = 1000.0;
		position[0]= -3000;
		position2[0]= 3000;
		crouch = false;
		crouch2 = false;
		fire = false;
		fire2 = false;
		dead = false;
		dead2 = false;
		player2->setAnimation(static_cast<Md3PlayerAnimType>(11));
		player2->setAnimation(static_cast<Md3PlayerAnimType>(22));
		player->setAnimation(static_cast<Md3PlayerAnimType>(11));
		player->setAnimation(static_cast<Md3PlayerAnimType>(22));
	}
}

// -------------------------------------------------------------------------
// specialKeyPress
//
// Key press glut callback function.  Called when user press a special key.
// -------------------------------------------------------------------------

static void
specialKeyPress (int key, int x, int y)
{
	keyboard.special[key] = 1;

	if(!dead2)
	{
		switch(key)
		{
		case GLUT_KEY_UP :
			player2->setAnimation(static_cast<Md3PlayerAnimType>(18));
			crouch2 = false;
			break;
		case GLUT_KEY_LEFT :
			if (!collisionChecking())
			{
				if (crouch2)
				{
					position2[0]-=300.0;
					player2->setAnimation(static_cast<Md3PlayerAnimType>(13));
				}
				else
				{
					position2[0]-=300.0;
					player2->setAnimation(static_cast<Md3PlayerAnimType>(15));
				}
			}
			break;
		case GLUT_KEY_RIGHT :
			if (!player2rules->checkRightWallCollision(position2))
			{
				if (crouch2)
				{
					position2[0]+=300.0;
					player2->setAnimation(static_cast<Md3PlayerAnimType>(13));
				}
				else
				{
					position2[0]+=300.0;
					player2->setAnimation(static_cast<Md3PlayerAnimType>(16));
				}
			}
			break;

		case GLUT_KEY_DOWN :
			if (crouch2)
			{
				player2Sphere.origin[1] +=1000;
				player2->setAnimation(static_cast<Md3PlayerAnimType>(11));
				player2->setAnimation(static_cast<Md3PlayerAnimType>(22));
				crouch2 = false;
			}
			else
			{
				player2Sphere.origin[1] -=1000;
				player2->setAnimation(static_cast<Md3PlayerAnimType>(23));
				crouch2 = true;
			}
			break;

		case GLUT_KEY_PAGE_UP :
			player2->setAnimation(static_cast<Md3PlayerAnimType>(20));
			if(collisionChecking()&&crouch)
			{
				health-=50;
				player->setAnimation(static_cast<Md3PlayerAnimType>(0));
			}
			if (health <= 0)
			{
				//cout << "dead yet?";
				dead =true;
				player->setAnimation(static_cast<Md3PlayerAnimType>(1));
			}
			break;

		case GLUT_KEY_PAGE_DOWN :
			if (!dead)
			{
				player2->setAnimation(static_cast<Md3PlayerAnimType>(7));
				//fireProjectile(position,1);
				//fireProjectile(direction);
				if (crouch2)
				{
					projectileSphere2.origin[1] -= 1250;
				}
				player2->setAnimation(static_cast<Md3PlayerAnimType>(8));
				fire2 = true;
			}
			break;
		}
	}
}

// -------------------------------------------------------------------------
// idleVisible
//
// Idle glut callback function.  Continuously called. Perform background
// processing tasks.
// -------------------------------------------------------------------------

static void
idleVisible ()
{
	// Update the timer
	updateTimer (&timer);

	if (bAnimated)
		glutPostRedisplay ();
}

// -------------------------------------------------------------------------
// windowStatus
//
// Window status glut callback function.  Called when the status of
// the window changes.
// -------------------------------------------------------------------------

static void
windowStatus (int state)
{
	// Disable rendering and/or animation when the
	// window is not visible
	if ((state != GLUT_HIDDEN) && (state != GLUT_FULLY_COVERED))
    {
		glutIdleFunc (idleVisible);
    }

	else
    {
		glutIdleFunc (NULL);
    }
}

// -------------------------------------------------------------------------
// main
//
// Application main entry point.
// -------------------------------------------------------------------------

int main (int argc, char *argv[])
{
	// Initialize GLUT
	glutInit (&argc, argv);
	argc=2;
	argv[1] = "models\\harley";
	// Initialize bounding spheres
	player1Sphere.origin = position;
	player1Sphere.radius = 2100;
	player2Sphere.origin = position2;
	player2Sphere.radius = 2100;
	projectilePos = position;
	projectileSphere.origin = position;
	projectileSphere.radius = 150;
	if (argc < 2)
    {
		cerr << "usage: " << argv[0] << " <player path> [<weapon path>]" << endl;
		return -1;
    }

	else
	{
		cout << argc << "and" << argv[0] << endl;
	}

	// create an OpenGL window
	glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize (640, 480);
	glutCreateWindow ("aFighter");

	// Initialize application
	atexit (shutdownApp);
	init (argv[1], (argc > 2) ? argv[2] : string ());
	// Setup glut callback functions
	glutReshapeFunc (reshape);
	glutDisplayFunc (display);
	glutKeyboardFunc (keyPress);
	glutSpecialFunc (specialKeyPress);
	glutWindowStatusFunc (windowStatus);
	glutIdleFunc (idleVisible);
	glutKeyboardUpFunc(keyUp);
	glutSpecialUpFunc (specialKeyUp);

	// Enter the main loop
	glutMainLoop ();

	return 0;
}
