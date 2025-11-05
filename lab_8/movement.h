/**
 * movement.h
 *
 * Contains helpful functions to navigate the CyBot around obstacles.
 * 
 * @date October 29, 2025
 * @author Thiago Bedal
 * @author Joseph Vesterby
**/

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include <math.h>
#include "open_interface.h"

#define BOT_MAX_SPEED 500
#define BOT_CRUISE_SPEED 200
#define BOT_CRAWL_SPEED 50
#define BOT_TURN_SPEED 50

// Returns 1 if robot has been bumped, 0 if not
int bot_isBumped(oi_t *sensor);

// Sets cybot wheels to drive forward at given velocity
void bot_drive(int velocity);

// Move the CyBot backward specified amount of centimeters
double bot_driveDistance(oi_t *sensor, int velocity, double distanceCM);

// Move the CyBot forward specified amount of centimeters
double bot_driveDistancePrecise(oi_t *sensor, int velocity, double distanceCM);

// Moves the CyBot forward a distance moving it around obstacles
void bot_driveObstacles(oi_t *sensor, double distanceCM);

// Move the CyBot in a square pattern for TA demo
void bot_driveSquare(oi_t* sensor);

// Sets wheels to tank turn cybot to left for +velocity, right for -velocity 
void bot_turn(int velocity);

// Tank turn the CyBot counter-clockwise by degrees
void bot_turnDegrees(oi_t *sensor, int velocity, double degrees);

// Stops the cybot
void bot_stopWheels(void);

#endif /* MOVEMENT_H_ */
