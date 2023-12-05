#include "model.h"
#include "interface.h"
#include "stack.h"

#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TEXT_LENGTH 30

// Global variable to store the models
struct Model models[NUM_ROWS][NUM_COLS]; // store up to 100 models

// Initialize the models with default values
void model_init() {
    // TODO: implement this.

    // Global variable and data types here
    // Represent contents of a cell (text or numbers)

    // Set models
    int i, j;

    // Loop through each row and column of the models array
    for (i = 0; i < NUM_ROWS; ++i) {
        for (j = 0; j < NUM_COLS; ++j) {
            // Initialize each cell with default values for an empty model
            models[i][j].type = STR;
            models[i][j].modelNumber = 0;

            // Allocate memory for the model text
            models[i][j].modelText = malloc(MAX_TEXT_LENGTH);

            // Check if memory was allocated successfully before checking for text
            if (models[i][j].modelText == NULL) {
                // Handle allocation failure
                exit(EXIT_FAILURE);
            }

            // Initialize the text to an empty string or some default value
            *(models[i][j].modelText) = '\0';
        }
    }
}

// Check if a string is a valid number
bool is_valid_num(const char *str) {
    // Iterate through each character in the string
    while (*str) {
        // Check if the character is a digit or a dot (for floating-point numbers)
        if (!(isdigit(*str) || *str == '.')) {
            return false;
        }
        ++str;
    }

    // The string is a valid number
    return true;
}

// Skip leading whitespace in a string
char *skip_blank(const char *text) {
    // Iterate through the string until a non-whitespace character is found
    while (*text && isspace(*text)) {
        text++;
    }
    return (char *)text;
}

// Check if a given string is a valid formula
bool check_formula(char *eqn) {
    // Skip leading whitespace in the formula
    eqn = skip_blank(eqn);

    // Check if the formula starts with '='
    if('=' != eqn[0]) {
        return false;
    }

    // Skip the '=' sign
    ++eqn;

    // Iterate through the formula
    for (; *eqn; ++eqn) {
        // Skip any leading whitespace in the formula
        eqn = skip_blank(eqn);

        // Convert lowercase letters to uppercase
        if (isalpha(*eqn)) {
            if (islower(*eqn)) {
                *eqn = (char)toupper(*eqn);
            }
        }

        // Check for valid characters in the formula
        if (isalpha(*eqn) || isdigit(*eqn) || *eqn == '+' || *eqn == '.') {
            continue;
        } else {
            return false;
        }
    }

    // The formula is valid
    return true;
}

// Set the numerical value of a cell
void set_num_val(ROW row, COL col, const char * const text) {
    // Convert the text to a double and set the cell properties
    models[row][col].modelNumber = strtod(text, NULL);
    models[row][col].type = NUM;
    strncpy(models[row][col].modelText, text, MAX_TEXT_LENGTH);
}

// Set the string value of a cell
void set_string_val(ROW row, COL col, const char * const text) {
    // Set the cell properties for a string value
    models[row][col].modelNumber = 0;
    models[row][col].type = STR;
    strncpy(models[row][col].modelText, text, MAX_TEXT_LENGTH);
}

// Check and evaluate a formula, updating the temporary value
bool formula(char *eqn, double *temp) {
    // Initialize the temporary result to 0
    *temp = 0;

    // Create a stack to handle numerical operations
    stack stack_number = gen_stack();

    // Variable to track the number of operands in the formula
    int index = 0;

    // Skip leading '=' or spaces
    if (eqn[0] == '=' || eqn[0] == ' ') {
        ++eqn;
    }

    // Process the formula string
    while (*eqn) {
        if (eqn[0] == '+') {
            ++index;
            ++eqn;
        } else if (isalpha(*eqn)) {
            // Handle cell reference in the formula
            if (!isdigit(eqn[1])) {
                return false;
            }
            COL current_col = (COL)(eqn[0] - 'A');
            ROW current_row = strtol(eqn += 1, &eqn, 10) - 1;

            // Validate cell reference
            if (current_col * current_row > NUM_COLS * NUM_ROWS || current_col * current_row < 0) {
                return false;
            }

            // Push the value of the referenced cell onto the stack
            push(&stack_number, models[current_row][current_col].modelNumber);
        } else if (isdigit(*eqn) || *eqn == '.') {
            // Push numeric values onto the stack
            push(&stack_number, strtod(eqn, &eqn));
        } else {
            // Invalid character in the formula
            delete_stack(&stack_number);
            return false;
        }
    }

    // Check if the number of operands matches the expected count
    if (stack_number.size != index + 1) {
        delete_stack(&stack_number);
        return false;
    }

    // Calculate the final result by popping elements from the stack
    while (stack_number.size > 0) {
        *temp += pop(&stack_number);
    }

    // Cleanup: free the stack
    delete_stack(&stack_number);
    return true;
}

// Set the value of a cell based on a formula
void set_eqn_val(ROW row, COL col, char *text) {
    // Evaluate the formula and store the result in the cell
    if (formula(text, &(models[row][col].modelNumber))) {
        models[row][col].type = EQN;

        // Convert the numerical result to a string
        sprintf(text, "%lg", models[row][col].modelNumber);

        // Update the cell display with the calculated result
        update_cell_display(row, col, text);
    }
}

// Update the value of a cell
void update_cell_value(ROW row, COL col) {
    // Check if the cell contains a formula
    if (models[row][col].type == EQN) {
        // Evaluate the formula and update the cell value
        if (formula(models[row][col].modelText, &(models[row][col].modelNumber))) {
            // Convert the numerical result to a string
            char *numStr = alloca(MAX_TEXT_LENGTH);
            sprintf(numStr, "%lg", models[row][col].modelNumber);

            // Update the cell display with the calculated result
            update_cell_display(row, col, numStr);
        }
    }
}

// Set the value of a cell based on its content
void set_cell_value(ROW row, COL col, char *text) {
    // Copy the input text to the model
    strncpy(models[row][col].modelText, text, MAX_TEXT_LENGTH);

    // Check if the text represents a formula
    if (check_formula(text)) {
        // Set the cell value based on the formula
        set_eqn_val(row, col, text);
    } else if (is_valid_num(text)) {
        // Set the cell value as a numerical value
        set_num_val(row, col, text);
    } else {
        // Set the cell value as a string
        set_string_val(row, col, text);
    }

    // Update the values of all cells in the model
    for (ROW i = ROW_1; i < NUM_ROWS; ++i) {
        for (COL j = COL_A; j < NUM_COLS; ++j) {
            // Skip the current cell to avoid unnecessary updates
            if (!(i == row && j == col)) {
                // Update the value of other cells in the model
                update_cell_value(i, j);
            }
        }
    }

    // Assuming update_cell_display updates the display of the cell
    update_cell_display(row, col, text);

    // Free the memory allocated for the input text
    free(text);
}

// Clear the content of a cell
void clear_cell(ROW row, COL col) {
    // Free the memory allocated for the text in the cell
    free(models[row][col].modelText);

    // Reset the cell to a default state
    models[row][col] = (struct Model){.type = NONE, .modelNumber = 0.0, .modelText = NULL};

    // Update the display to show an empty string in the cleared cell
    update_cell_display(row, col, "");
}

// Clear the entire model
void model_clear() {
    // Iterate over all cells in the model
    for (ROW row = ROW_1; row < NUM_ROWS; row++) {
        for (COL col = COL_A; col < NUM_COLS; col++) {
            // Clear the content of each cell
            clear_cell(row, col);
        }
    }
}

// Get the textual representation of a cell's value
char *get_textual_value(ROW row, COL col) {
    // Get a pointer to the Model at the specified row and column
    struct Model *this = &models[row][col];

    // Allocate memory for the result string
    char *result = malloc(MAX_TEXT_LENGTH);

    // Switch based on the type of the cell
    switch (this->type) {
        // If the type is NUM, STR, or EQN, copy the modelText to the result
        case NUM:
        case STR:
        case EQN:
            strncpy(result, this->modelText, MAX_TEXT_LENGTH);
            break;

            // If the type is NONE, set an empty string in the result
        case NONE:
            *result = '\0';
            break;
    }

    // Return the result string
    return result;
}
