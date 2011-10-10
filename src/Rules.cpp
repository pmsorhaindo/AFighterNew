/*
 * Rules.cpp
 *
 *  Created on: 12 May 2010
 *  Author: David Lee and Paul-Michael Sorhaindo
 */
#include "Rules.h"
#include <cstdlib>

// Constructor for class Rules, takes 6 floats for the 2 floor vectors, and 2 vectors for the 2 wall vectors
Rules::Rules(float q, float w, float e, float r, float t, float y, Vectormath::Aos::Vector3 leftWall, Vectormath::Aos::Vector3 rightWall)
{

	// Initialize stuff
	setFloorVector1(q,w,e);
	setFloorVector2(r,t,y);
	setLeftWall(leftWall);
	setRightWall(rightWall);
	physics = (struct physicsInstance *) malloc (sizeof(struct physicsInstance));
    physics->gravity[0] = 0.0;
    physics->gravity[1] = 9.81;
    physics->gravity[2] = 0.0;
}

Rules::~Rules()
{
	//Delete stuff
}

// Returns the current force acting against the player, e.g from the other player running into them
Vectormath::Aos::Vector3 Rules::getForceActingOn()
{
	return physics->forceActingOn;
}

// Set the current force being exerted e.g from running
void Rules::setForceExerted(Vectormath::Aos::Vector3 jediForce)
{
	physics->forceExerted = jediForce;
}

// Get the current force being exerted e.g from running

Vectormath::Aos::Vector3 Rules::getForceExerted()
{
	return physics->forceExerted;
}

// Calculates the dot product between 2 vectors
float Rules::dotProduct(Vectormath::Aos::Vector3 position, Vectormath::Aos::Vector3 normal)
{
	float fulhamForTheWin = (position.getX()*normal.getX())+(position.getY()*normal.getY())+(position.getZ()*normal.getZ());
	return fulhamForTheWin;
}

// Returns which player vector is exerting the most force, and thus moving forward
Vectormath::Aos::Vector3 Rules::resultantForce(Vectormath::Aos::Vector3 forceExerted, Vectormath::Aos::Vector3 forceActingOn)
{
	Vectormath::Aos::Vector3 andTheWinnerIs;
	andTheWinnerIs = physics->forceExerted-physics->forceActingOn;
	return andTheWinnerIs;
}

// Calculate Magnitude of the given vector
float Rules::calculateMagnitude(Vectormath::Aos::Vector3 position)
{
	float foobar = sqrt((position.getX()*position.getX())+(position.getY()*position.getY())+(position.getZ()*position.getZ()));
	//Debug: cout << "Magnet Dude!! : " << foobar << endl;
	return foobar;
}

// Calculates the angle of Projection as part of Sphere-Plane collision
float Rules::angleOfAidanSpeak(float foo) // This is the angle of projection discovered in Aidan's lecture notes!
{
	float bar=acos(foo);
	//cout << "This is not foo : " << bar << endl;
	return bar; // we hate doing this!
}

// Set the first floor vector
void Rules::setFloorVector1(float a, float b, float c)
{
	floorVector1[0]=a;
	floorVector1[1]=b;
	floorVector1[2]=c;
}

// Set the second floor vector
void Rules::setFloorVector2(float a, float b, float c)
{
	floorVector2[0]=a;
	floorVector2[1]=b;
	floorVector2[2]=c;
}

// Returns the first floor vector
Vectormath::Aos::Vector3 Rules::getFloorVector1()
{
	return floorVector1;
}

// Returns the second floor vector
Vectormath::Aos::Vector3 Rules::getFloorVector2()
{
	return floorVector2;
}

// Sets the current force of gravity
void Rules::setGravity(float g)
{
	physics->gravity[1] = g;
}

// returns the current force of gravity
float Rules::getGravity()
{
	return physics->gravity[1];
}

// Calculates the normal vector to the floor using the 2 floor vectors
Vectormath::Aos::Vector3 Rules::calculateFloorNormal(Vectormath::Aos::Vector3 floor1, Vectormath::Aos::Vector3 floor2)
{
	Vectormath::Aos::Vector3 normal;
	float mag;
	normal.setX((floor1.getY()*floor2.getZ())-(floor1.getZ()*floor2.getY()));
	normal.setY((floor1.getZ()*floor2.getX())-(floor1.getX()*floor2.getZ()));
	normal.setZ((floor1.getZ()*floor2.getY())-(floor1.getY()*floor2.getX()));

	//normal now contains the cross product, for you maths students out there
	//we now divide by the magnitude of the cross product.
	// for an awesome normal.

	mag = (normal.getX()*normal.getX())+(normal.getY()*normal.getY())+(normal.getZ()*normal.getZ());
	mag= sqrt(mag);

	normal.setX(normal.getX()/mag);
	normal.setY(normal.getY()/mag);
	normal.setZ(normal.getZ()/mag);

	//normal is now looking pretty normal to us!

	//cout << "Vector normalized = " << normal.getX() << normal.getY() << normal.getZ() <<endl;
	return normal;
}

// Checks for Collision with the floor using the current position vector of the player
bool Rules::checkForFloor(Vectormath::Aos::Vector3 position)
{
	/*Vectormath::Aos::Vector3 normal = calculateFloorNormal(getFloorVector1(),getFloorVector2());
	float dp = dotProduct(position,normal);
	//cout << "This is wrong :" << dp << endl;
	float omgBrightonWonOnce = angleOfAidanSpeak(dp);
	float zero = sin(omgBrightonWonOnce);
	float somethingSensible = calculateMagnitude(normal);
	float jokes = somethingSensible/zero;
	cout << zero << endl;
	cout << "seriously this better not be jokes :" << jokes << endl; */

	bool pro = false;
	Vectormath::Aos::Vector3 theFloor = getFloorVector1();
	float distanceFromFloor = theFloor[1]-position[1];
	//cout << "Distance from Floor : " << distanceFromFloor <<endl;

	if (distanceFromFloor<=0)
	{
		pro = false;
	}

	else
	{
	pro = true;
	}
	return pro;
}

// Returns the mass of the player
float Rules::getMass()
{
	return rules->mass;
}

// sets the mass of the player
void Rules::setMass(float mass)
{
	rules->mass = mass;
}

// returns the players accuracy value for the current game
float Rules::getAccuracyForGame()
{
	return rules->accuracyForGame;
}

// Sets the players accuracy for the current game
void Rules::setAccuracyForGame(float accu)
{
	//stuff edit!
}

// returns the players current health
float Rules::getHealth()
{
	return rules->health;
}

// reduces the players current health by the given amount
void Rules::decreaseHealth(float health)
{
	rules->health-=health;
}

// increases the players current health by the given amount
void Rules::increaseHealth(float health)
{
	rules->health+=health;
}

// returns the games won for the current player
int Rules::getGamesWon()
{
	return rules->gamesWon;
}

// increments the games won for the current player
void Rules::incGamesWon()
{
	rules->gamesWon++;
}

//returns the amount of projectiles currently in the air for the current player
int Rules::getAmountOfProjectiles()
{
	return rules->projectiles;
}

// increases the amount of projectiles in the air currently, if there's less than 3
bool Rules::incAmountOfProjectiles()
{
	if(rules->projectiles<3)
	{
		rules->projectiles++;
		return true;
	}
	else
	{
		return false;
	}
}

// decrements the current amount of projectiles in the air
bool Rules::decAmountOfProjectiles()
{
	if(rules->projectiles>0)
	{
		rules->projectiles--;
		return true;
	}
	else
	{
		return false;
	}

}

// returns the distance between the 2 spheres given
float Rules::checkDistanceBetweenSpheres(Vectormath::Aos::Vector3 sphere1, Vectormath::Aos::Vector3 sphere2)
{
	float temp = ((sphere1.getX() - sphere2.getX()) + (sphere1.getY() - sphere2.getY()) + (sphere1.getZ() - sphere2.getZ()));
	temp = (temp * temp);
	temp = sqrt(temp);
	return temp;
}

// checks for collision between the 2 spheres given
bool Rules::checkForCollision(float difference, float radius1, float radius2)
{
	if ( difference < radius1 || difference < radius2)
	{
		return true;
	}
	return false;
}

// sets the left wall vector
void Rules::setLeftWall(Vectormath::Aos::Vector3 leftWall)
{
	wallLeft = leftWall;
}

// sets the right wall vector
void Rules::setRightWall(Vectormath::Aos::Vector3 rightWall)
{
	wallRight = rightWall;
}

// returns the left wall vector
Vectormath::Aos::Vector3 Rules::getLeftWall()
{
	return wallLeft;
}

// returns the right wall vector
Vectormath::Aos::Vector3 Rules::getRightWall()
{
	return wallRight;
}

// checks for collision with the left wall and the player
bool Rules::checkLeftWallCollision(Vectormath::Aos::Vector3 position)
{
	bool pro = false;
	Vectormath::Aos::Vector3 leftWall = getLeftWall();
	float distanceFromWall = position[0]-leftWall[0];

	if (distanceFromWall>=0)
	{
		pro = false;
	}
	else
	{
		pro = true;
	}
	return pro;
}

// checks for collision with the right wall and the player
bool Rules::checkRightWallCollision(Vectormath::Aos::Vector3 position)
{
	bool pro = false;
	Vectormath::Aos::Vector3 rightWall = getRightWall();
	float distanceFromWall = rightWall[0]-position[0];

	if (distanceFromWall>=0)
	{
		pro = false;
	}
	else
	{
		pro = true;
	}
	return pro;
}


