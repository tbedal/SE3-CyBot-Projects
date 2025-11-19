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
#include <math.h>

#define MAX_TABLES 5
#define TABLE_RADIUS 6
// Center of table to center of table in each axis tolerance
#define TABLE_TOLERANCE 50

#define M_PI 3.141592654

// Creates a gridPoint struct
// TODO: This could get moved to floats to further reduce compounding error
typedef struct gridPointData {
    int16_t x;
    int16_t y;
} gridPoint;

// TODO: Comment me!
void updateCyBotPosition(gridPoint* currentPostion, int16_t* currentDegree, uint8_t traveledDistance, int16_t traveledDegrees);
// TODO: Comment me!
gridPoint findTableLocation(gridPoint* currentPosition, int16_t* currentDegrees, uint8_t tableDistance, int16_t tableDegrees);
// TODO: Comment me!
uint8_t isVisitedTable(gridPoint tablePositions[MAX_TABLES], gridPoint tableLocation);




// ALREADY IN OLD MAIN: Returns a 1 if given value is within +/- tolerance of target, 0 if not
uint8_t isWithinTolerance(uint8_t value, uint8_t target, uint8_t tolerance);


uint8_t main(void) {

    // Initializing to all 0,0 because we know no tables in kitchen
    gridPoint tablePositions[MAX_TABLES] = {0};

    gridPoint currentPosition = {0, 0};
    int16_t currentDegrees = 0;

    tablePositions[0] = findTableLocation(&currentPosition, &currentDegrees, 200, 45);
    printf("Current Position: %d, %d\t\tCurrent Degrees: %d\n", currentPosition.x, currentPosition.y, currentDegrees);

    printf("Current Position: %d, %d\n", tablePositions[0].x, tablePositions[0].y);

    printf("Have we been here? %d", isVisitedTable(tablePositions, findTableLocation(&currentPosition, &currentDegrees, 200, 45)));

    return 0;
}


// ALREADY IN OLD MAIN:
uint8_t isWithinTolerance(uint8_t value, uint8_t target, uint8_t tolerance) {
    return abs(value - target) < tolerance;
}




void updateCyBotPosition(gridPoint* currentPosition, int16_t* currentDegrees, uint8_t traveledDistance, int16_t traveledDegrees) {

    // Updating currentDegrees
    *currentDegrees += traveledDegrees;

    // Wrap around logic
    if      (*currentDegrees >= 360) { *currentDegrees -= 360; }
    else if (*currentDegrees < 0) {  *currentDegrees += 360;  }


    // Updating currentPosition
    (*currentPosition).x += round(traveledDistance * cos((*currentDegrees) * (M_PI / 180)));
    (*currentPosition).y += round(traveledDistance * sin((*currentDegrees) * (M_PI / 180)));

}


// Kinda GHETTO but works by pretending the table is where we moved the robot,
// then sets the bot location back to where it actually is
gridPoint findTableLocation(gridPoint* currentPosition, int16_t* currentDegrees, uint8_t tableDistance, int16_t tableDegrees) {

    gridPoint tablePoint;

    // Remembering where the bot actually is
    gridPoint currentPositionStorage = *currentPosition;
    int16_t currentDegreesStorage = *currentDegrees;

    // Finds the coords of the table's center
    updateCyBotPosition(currentPosition, currentDegrees, tableDistance + TABLE_RADIUS, tableDegrees);

    // Setting the table position
    tablePoint = *currentPosition;

    // Sets bot position back to where it actually is
    *currentPosition = currentPositionStorage;
    *currentDegrees = currentDegreesStorage;

    return tablePoint;
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









































