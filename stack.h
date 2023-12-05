#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Complete definition of 'type' struct
struct type {
    double value;  // Or other fields as needed
};

// Definition of the stack structure
typedef struct {
    struct type *data;  // Array to store stack elements
    double *sp;         // Stack pointer
    size_t size;        // Number of elements in the stack
    size_t capacity;    // Capacity of the stack
} stack;

// Function to generate a new stack
stack gen_stack() {
    // Allocate memory for the stack data
    struct type *temp;
    temp = malloc(16 * sizeof(double));

    // Initialize and return the stack
    return (stack){temp, (double *) temp, 0, 16};
}

// Function to build the stack by doubling its capacity
void build_stack(stack *stack1) {
    // Allocate memory for the new stack with doubled capacity
    struct type *temp;
    temp = malloc(2 * stack1->capacity * sizeof(double));

    // Check if memory allocation is successful
    if (temp) {
        // Copy existing data to the new stack
        memcpy(temp, stack1->data, 2 * stack1->capacity * sizeof(double));

        // Update stack capacity
        stack1->capacity *= 2;

        // Free the old stack memory
        free(stack1->data);

        // Update stack data and stack pointer
        stack1->data = temp;
        stack1->sp = (double *) (stack1->data + stack1->size);
    } else {
        // Handle memory allocation failure
        exit(EXIT_FAILURE);
    }
}

// Function to push a value onto the stack
void push(stack *stack1, double value) {
    // Check if the stack is full, and build it if needed
    if ((size_t) stack1->sp >= stack1->capacity) {
        build_stack(stack1);
    }

    // Push the value onto the stack
    *(stack1->sp)++ = value;
    ++stack1->size;
}

// Function to pop a value from the stack
double pop(stack *stack1) {
    // Check if the stack is empty
    if (stack1->size == 0) {
        // Handle popping from an empty stack
        exit(EXIT_FAILURE);
    } else {
        // Pop the value from the stack
        --stack1->size;
        --stack1->sp;
    }
    return *(stack1->sp);
}

// Function to delete the stack and free its memory
void delete_stack(stack *stack1) {
    // Free the stack data memory
    free(stack1->data);

    // Reset stack size and capacity
    stack1->size = 0;
    stack1->capacity = 0;
}