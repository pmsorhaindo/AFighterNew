/*======================================================================
 *
 * Rules Class of AFighter Project
 * Author: David Lee and Paul-Michael Sorhaindo
 *
 *=====================================================================
 */

#ifdef _WIN32
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <iostream>
#include <cmath>
#include <vectormath/cpp/vectormath_aos.h>

using namespace std;


/////////////////////////////////////////////////////////////////////////////
//
// Game rule structs defined
//
/////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------------
// Game Rules
// --------------------------------------------------------------------------

// See Rules.cpp for individual Method Details
class Rules {
public:
	Rules(float q, float w, float e, float r, float t, float y, Vectormath::Aos::Vector3 leftWall, Vectormath::Aos::Vector3 rightWall);
	~Rules();
	void setFloorVector1(float a, float b, float c);
	void setFloorVector2(float a, float b, float c);
	Vectormath::Aos::Vector3 getFloorVector1();
	Vectormath::Aos::Vector3 getFloorVector2();
	void setGravity(float g);
	float getGravity();
	Vectormath::Aos::Vector3 calculateFloorNormal(Vectormath::Aos::Vector3 floor1, Vectormath::Aos::Vector3 floor2);
	bool checkForFloor(Vectormath::Aos::Vector3 position);
	float dotProduct(Vectormath::Aos::Vector3 floor1, Vectormath::Aos::Vector3 floor2);
	float angleOfAidanSpeak(float foo);
	float calculateMagnitude(Vectormath::Aos::Vector3 position);
	void setForceExerted(Vectormath::Aos::Vector3 jediForce);
	Vectormath::Aos::Vector3 getForceExerted();
	Vectormath::Aos::Vector3 resultantForce(Vectormath::Aos::Vector3 forceExerted, Vectormath::Aos::Vector3 forceActingOn);
	Vectormath::Aos::Vector3 getForceActingOn();
	float getMass();
	void setMass(float mass);
	float getAccuracyForGame();
	void setAccuracyForGame(float accu);
	float getHealth();
	void decreaseHealth(float health);
	void increaseHealth(float health);
	int getGamesWon();
	void incGamesWon();
	int getAmountOfProjectiles();
	bool incAmountOfProjectiles();
	bool decAmountOfProjectiles();
	bool checkForCollision(float difference, float radius1, float radius2);
	float checkDistanceBetweenSpheres(Vectormath::Aos::Vector3 sphere1, Vectormath::Aos::Vector3 sphere2);
	void setLeftWall(Vectormath::Aos::Vector3 leftWall);
	void setRightWall(Vectormath::Aos::Vector3 rightWall);
	Vectormath::Aos::Vector3 getLeftWall();
	Vectormath::Aos::Vector3 getRightWall();
	bool checkLeftWallCollision(Vectormath::Aos::Vector3 position);
	bool checkRightWallCollision(Vectormath::Aos::Vector3 position);

private:
	Vectormath::Aos::Vector3 floorVector1;
	Vectormath::Aos::Vector3 floorVector2;
	Vectormath::Aos::Vector3 wallLeft;
	Vectormath::Aos::Vector3 wallRight;

struct rules
{
	int avatar;     // points to an array of file paths to load different avatars
	float mass;       // how gravity affects the player.
	float accuracyForGame;   // Accuracy for the Current game
	float health; // health of fighter game lost when this == 0;
	int gamesWon;// Amount of games the fighter has won for best of 3's/5's/till n number of wins
	int projectiles; // amount of projectiles the fighter has on the screen.
};

struct physicsInstance
{
	Vectormath::Aos::Vector3 gravity;     // have fun editing this!
	Vectormath::Aos::Vector3 forceExerted;       // This is where you're trying to go
	Vectormath::Aos::Vector3 forceActingOn;   // Where people want you to go
	float coEfFriction;       // I hope we get to use this...
};

physicsInstance * physics;
rules * rules;
};

class Fighter
{
public:
	Fighter(int av,float mass,float accuracy,int health,int gamesWon);
	Fighter(int av,float mass);
};


struct player
{
	char username[64];     // User's name
	int gamesPlayed;       // indication of users social life
	int gamesWon;   	   // indication of users skills
	int gamesLost;		  // indication of users failure!
	int gamesDrawn;		   //indication of boring/epic battles
	float percentageWon;  // a useless statistic to show we still know our GCSE Math!
	float accuracy;		  // a slightly more interesting statistic
};

struct fightEnviroment
{
	char background[64];     // file path of the background texture
	int gameMode;   			// array index to select game mode
	float fps;    		  	 // Frames per second
};
