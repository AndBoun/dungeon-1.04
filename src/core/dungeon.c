//
// Created by Andrew Boun on 2/11/25.
//

#include <dungeon.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

// Function to initialize a dungeon instance
void init_dungeon(Dungeon *d) {
    d->rooms = NULL;
    d->num_rooms = 0;

    d->num_up_stairs = 0;
    d->num_down_stairs = 0;

    d->up_stairs = NULL;
    d->down_stairs = NULL;

    // initialize PC later
    d->monsters = NULL;
}

bool generate_random_dungeon(Dungeon *d){
    int i, j;
    int num_rooms;

    bool success = false;

    // Generate Rooms
    do {
        // Initialize grid with ROCK
        for (i = 0; i < DUNGEON_HEIGHT; i++) {
            for (j = 0; j < DUNGEON_WIDTH; j++) {
                d->grid[i][j].type = ROCK;
                if (i == 0 || i == DUNGEON_HEIGHT - 1 || j == 0 || j == DUNGEON_WIDTH - 1) {
                    d->grid[i][j].hardness = MAX_HARDNESS;
                } else{
                    d->grid[i][j].hardness = rand() % (MAX_HARDNESS - 1 - MIN_HARDNESS) + 1;
                }
            }
        }

        // Assume a successful room generation
        success = true;

        // Generate a random number of rooms to generate
        num_rooms = MIN_ROOMS + rand() % (MAX_ROOMS - MIN_ROOMS + 1);
        d->num_rooms = num_rooms;
        // printf("Number of rooms: %d\n", num_rooms);
        d->rooms = malloc(num_rooms * sizeof(Room));

        // Generate rooms
        for (i = 0; i < num_rooms; i++) {

            // If attempt limit is reached, reset grid and try again
            if (!generate_random_room(d, i)) {
                success = false;
                break;
            }
        }

        // If rooms were generated successfully, break out of loop
        if (success) break;

    } while (1);

    // Generate Corridors
    for (i = 0; i < num_rooms - 1; i++){
        generate_corridor(
            d,
            d->rooms[i].center_x, 
            d->rooms[i].center_y, 
            d->rooms[i + 1].center_x, 
            d->rooms[i + 1].center_y
        );
    }

    // Generate Stairs
    d->num_up_stairs = MIN_UP_STAIRS + rand() % (MAX_UP_STAIRS - MIN_UP_STAIRS + 1);
    d->up_stairs = malloc(d->num_up_stairs * sizeof(Stair));
    for (i = 0; i < d->num_up_stairs; i++){
        generate_random_stair(d, UP_STAIRS, i);
    }

    d->num_down_stairs = MIN_DOWN_STAIRS + rand() % (MAX_DOWN_STAIRS - MIN_DOWN_STAIRS + 1);
    d->down_stairs = malloc(d->num_down_stairs * sizeof(Stair));
    for (i = 0; i < d->num_down_stairs; i++){
        generate_random_stair(d, DOWN_STAIRS, i);
    }

    place_random_player(d);

    return true;
}

// randomly places the player in the dungeon
int place_random_player(Dungeon *d){
    int x, y;
    do {
        x = rand() % PLACABLE_WIDTH + 1;
        y = rand() % PLACABLE_HEIGHT + 1;
    } while (d->grid[y][x].type == CORRIDOR || d->grid[y][x].type == ROCK);

    init_player(&d->pc, x, y);
    d->grid[y][x].type = PLAYER;

    return 1;
}

void destroy_dungeon(Dungeon *d){
    free(d->rooms);
    free(d->up_stairs);
    free(d->down_stairs);
    return;
}