#ifndef ASSIGNMENT_DEFS_H
#define ASSIGNMENT_DEFS_H

#include <stdbool.h>
#include <string.h>

#define NUM_ROWS 10
#define NUM_COLS 7

#define MAXLEN 256


// Rows of the spreadsheet.
// NOTE: enums are 0-based, so the constant 'ROW_1' has the numerical value 0.
typedef enum {
    ROW_1,
    ROW_2,
    ROW_3,
    ROW_4,
    ROW_5,
    ROW_6,
    ROW_7,
    ROW_8,
    ROW_9,
    ROW_10,
} ROW;

typedef enum {
    COL_A,
    COL_B,
    COL_C,
    COL_D,
    COL_E,
    COL_F,
    COL_G,
} COL;

typedef enum {
    NONE,
    STR,
    NUM,
    EQN,
} CELL_TYPE;

//Create data structure to model contents
struct Model {
    CELL_TYPE type;
    double modelNumber;
    char *modelText;
} Model;

#endif //ASSIGNMENT_DEFS_H
