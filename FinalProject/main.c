/**
 * main.c
 *
 * The main file.
 *
 * FIXME: THE FUNCTIONS SHOULD BE MOVED ELSEWHERE IF WE HAVE TIME
 *
 * @date November 19, 2025
 * @author Thiago Bedal
 * @author Joseph Vesterby
**/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define MAX_TABLES 5
#define TABLE_RADIUS 6
// Center of table to center of table in each axis tolerance (ie object can be max 50 cm in the x and 50 cm in y giving a true distance between of 70.7 cm)
#define TABLE_TOLERANCE 50

// (M_PI / 180)
#define DEG_TO_RAD 0.01745329251

// Creates a gridPoint struct
// TODO: This could get moved to floats to further reduce compounding error
typedef struct gridPointData {
    int16_t x;
    int16_t y;
} gridPoint;

// Creates a gridPoint struct with degree tracking
// TODO: This could get moved to floats to further reduce compounding error
typedef struct gridPointFullData {
    gridPoint position;
    uint16_t degrees;
} gridPointFull;

// TODO: Comment me!
void wrapAroundDegrees(int16_t* degrees);
// TODO: Comment me!
void updateCyBotPosition(gridPointFull* currentPostionFull, uint16_t traveledDistance, int16_t traveledDegrees);
// TODO: Comment me!
gridPoint findTableLocation(gridPointFull* currentPostionFull, uint16_t tableDistance, int16_t tableDegrees);
// TODO: Comment me!
uint8_t isVisitedTable(gridPoint tablePositions[MAX_TABLES], gridPoint tableLocation);
// TODO: Comment me!
gridPoint calculateWall(gridPointFull firstWall, gridPointFull secondWall);


// TO BE PUT IN POSITION.C
// Calculates the position of all the corners
void findCorners(gridPointFull* firstWall, gridPointFull* secondWall, gridPointFull* firstWallParralel, gridPoint corners[4]);


// ALREADY IN OLD MAIN: Returns a 1 if given value is within +/- tolerance of target, 0 if not
uint8_t isWithinTolerance(uint8_t value, uint8_t target, uint8_t tolerance);


uint8_t main(void) {

    // Initializing to all 0,0 because we know no tables in kitchen
    gridPoint tablePositions[MAX_TABLES] = {0};

    gridPointFull currentPositionFull = {{0, 0}, 0};

    // Testing code
    tablePositions[0] = findTableLocation(&currentPositionFull, 200, 45);
    printf("Current Position: %d, %d\t\tCurrent Degrees: %d\n", currentPositionFull.position.x, currentPositionFull.position.y, currentPositionFull.degrees);

    printf("Table Position: %d, %d\n", tablePositions[0].x, tablePositions[0].y);

    printf("Have we been here? %d\n", isVisitedTable(tablePositions, findTableLocation(&currentPositionFull, 200, 45)));


    gridPointFull firstWall = {{141, 141}, 45};
    gridPointFull secondWall = {{177, 389}, 135};

    gridPoint corner = calculateWall(firstWall, secondWall);
    printf("Corner Position: %d, %d\n", corner.x, corner.y);

    int16_t cornerToFirstDegree = round((1 / DEG_TO_RAD) * atan2(firstWall.position.y - corner.y, firstWall.position.x -  corner.x));
    wrapAroundDegrees(&cornerToFirstDegree);
    printf("Corner to First Angle: %d\n", cornerToFirstDegree);


    gridPointFull parralelToFirstWall = {{-71, 354}, 225};


    gridPoint corners[4];
    findCorners(&firstWall, &secondWall, &parralelToFirstWall, corners);


    printf("(%d, %d), (%d, %d), (%d, %d), (%d, %d)", corners[0].x, corners[0].y, corners[1].x, corners[1].y, corners[2].x, corners[2].y, corners[3].x, corners[3].y);

    return 0;
}


// ALREADY IN OLD MAIN:
uint8_t isWithinTolerance(uint8_t value, uint8_t target, uint8_t tolerance) {
    return abs(value - target) < tolerance;
}


void wrapAroundDegrees(int16_t* degrees){
    if      (*degrees >= 360) { *degrees -= 360; }
    else if (*degrees < 0)    { *degrees += 360; }
}

void updateCyBotPosition(gridPointFull* currentPositionFull, uint16_t traveledDistance, int16_t traveledDegrees) {

    // Updating degrees
    (*currentPositionFull).degrees += traveledDegrees;

    // Degree wrap around logic
    wrapAroundDegrees(&((*currentPositionFull).degrees));


    // Updating currentPosition
    (*currentPositionFull).position.x += round(traveledDistance * cos(((*currentPositionFull).degrees) * DEG_TO_RAD));
    (*currentPositionFull).position.y += round(traveledDistance * sin(((*currentPositionFull).degrees) * DEG_TO_RAD));

}

// TO BE PUT IN POSITION.C
// Calculates the position of all the corners
void findCorners(gridPointFull* firstWall, gridPointFull* secondWall, gridPointFull* firstWallParralel, gridPoint corners[4]) {

    corners[0] = calculateWall(*firstWall, *secondWall);
    corners[1] = calculateWall(*firstWallParralel, *secondWall);

    uint8_t parralelDist400 = sqrt(pow(corners[0].x - corners[1].x, 2) + pow(corners[0].y - corners[1].y, 2)) < 350;

    gridPointFull tempPoint = {corners[0], (*secondWall).degrees};
    corners[2] = findTableLocation(&tempPoint, 300 + parralelDist400 * 100 - TABLE_RADIUS, 180);

    tempPoint.position = corners[1];
    corners[3] = findTableLocation(&tempPoint, 300 + parralelDist400 * 100 - TABLE_RADIUS, 180);
}


// Pretends the bot moved to where the table is to get the coordinates of the table
gridPoint findTableLocation(gridPointFull* currentPositionFull, uint16_t tableDistance, int16_t tableDegrees) {

    // Where the bot is
    gridPointFull currentPositionStorage = *currentPositionFull;

    // Finds the coords of the table's center
    updateCyBotPosition(&currentPositionStorage, tableDistance + TABLE_RADIUS, tableDegrees);

    return currentPositionStorage.position;
}


uint8_t isVisitedTable(gridPoint tablePositions[MAX_TABLES], gridPoint tableLocation) {

    uint8_t isVisitedTable = 0;

    uint8_t tableCursor;

    // Goes through each of the tables assuming any 0, 0s are not real
    for (tableCursor = 0; !(tablePositions[tableCursor].x == 0 && tablePositions[tableCursor].y == 0) && tableCursor < MAX_TABLES; tableCursor++) {

        if (isWithinTolerance(tablePositions[tableCursor].x, tableLocation.x, TABLE_TOLERANCE) && isWithinTolerance(tablePositions[tableCursor].y, tableLocation.y, TABLE_TOLERANCE)) {
            isVisitedTable = 1;
            break;
        }
    }

    return isVisitedTable;
}

// Assumes that firstWall and secondWall had a FL and FR sensed border aka bot was perpendicular to the wall
// AND Assumes that first Wall and secondWall are perpendicular
gridPoint calculateWall(gridPointFull firstWall, gridPointFull secondWall){

    gridPoint corner;

    // Saving on tan and cot calculations by doing them once and carrying throughout

    float tanDegree = tan(firstWall.degrees * DEG_TO_RAD);
    // Tertiary because 1 / 0 is undefined but 1 / (basically 0) isn't
    float cotDegree = 1 / (tanDegree == 0 ? 0.00000000000001 : tanDegree);

    // Calculates where a corner must be using Desmos math (trust if this doesn't kill the bot it works)
    corner.x = round((cotDegree * firstWall.position.x + tanDegree * secondWall.position.x - firstWall.position.y + secondWall.position.y)/(cotDegree + tanDegree));
    corner.y = round(cotDegree * (corner.x - firstWall.position.x) + firstWall.position.y);

    return corner;
}






































