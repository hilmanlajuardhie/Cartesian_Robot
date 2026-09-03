#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For usleep() to simulate motor timing
#include <string.h>

#define MAX_ROWS 10
#define MAX_COLS 20
#define STEPS_PER_UNIT 10 // Simulated steps per grid index jump

// Initial gantry position
int current_row = 0;
int current_col = 0;

void print_board(void) {
    // Clear screen escape sequence or clean break lines
    printf("\n==================================================\n");
    printf("        CARTESIAN TABLE STATE (10 x 20)           \n");
    printf("==================================================\n    ");
    
    for (int c = 0; c < MAX_COLS; c++) {
        printf("%2d ", c);
    }
    printf("\n");

    for (int r = 0; r < MAX_ROWS; r++) {
        printf("%2d |", r);
        for (int c = 0; c < MAX_COLS; c++) {
            if (r == current_row && c == current_col) {
                printf(" H ");
            } else {
                printf(" . ");
            }
        }
        printf("|\n");
    }
    printf("==================================================\n");
    printf("Current Coordinates -> Row: %d | Col: %d\n", current_row, current_col);
    printf("==================================================\n");
}

void simulate_motor(const char* axis_name, int total_steps, int forward) {
    if(strcmp(axis_name, "Vertical") == 0){
        printf("[SIMULATION] Driving %s axis [%s]\n", 
               axis_name, forward ? "RIGHT" : "LEFT");
    }else{
        printf("[SIMULATION] Driving %s axis [%s]\n", 
               axis_name, forward ? "DOWN" : "UP");
    }
    
    // Simulate step execution with a progress ticker
    int chunk = total_steps / 10;
    if (chunk == 0) chunk = 1;
    
    for (int i = 0; i <= total_steps; i += chunk) {
        int current_step = (i > total_steps) ? total_steps : i;
        printf("   -> Step progress: %d / %d steps\r", current_step, total_steps);
        fflush(stdout);
        usleep(500000); // 0.5s delay per chunk
    }
    printf("   -> Step progress: %d / %d steps [COMPLETED]    \n\n", total_steps, total_steps);
    print_board();
}

void move_to_coordinate(int target_row, int target_col) {
    if (target_row < 0 || target_row >= MAX_ROWS || target_col < 0 || target_col >= MAX_COLS) {
        printf("\n[ERROR] Target out of bounds! Valid Range: Rows [0-9], Cols [0-19].\n");
        return;
    }

    int row_diff = target_row - current_row;
    int col_diff = target_col - current_col;

    // 1. Move Y-Axis (Vertical - Dual Synchronized Motors)
    if (row_diff != 0) {
        int forward = (row_diff > 0) ? 1 : 0;
        int total_steps = abs(row_diff) * STEPS_PER_UNIT;
        simulate_motor("Vertical", total_steps, forward);
        current_row = target_row;
    }

    // 2. Move X-Axis (Horizontal Motor)
    if (col_diff != 0) {
        int forward = (col_diff > 0) ? 1 : 0;
        int total_steps = abs(col_diff) * STEPS_PER_UNIT;
        simulate_motor("Horizontal", total_steps, forward);
        current_col = target_col;
    }

    if (row_diff == 0 && col_diff == 0) {
        printf("[INFO] Already at target position.\n");
    }

    print_board();
}

int main(void) {
    print_board();

    int target_r, target_c;
    while (1) {
        printf("\nRow (0-9) | Column (0-19) | Exit (-1)\n");
        printf("\nSyntax: [Row] [Space] [Column]");
        printf("\nEnter Coordinate: ");
        if (scanf("%d", &target_r) != 1) break;
        if (target_r == -1) break;
        if (scanf("%d", &target_c) != 1) break;
        printf("\n");
        
        move_to_coordinate(target_r, target_c);
    }

    printf("\nSimulator closed.\n");
    return 0;
}