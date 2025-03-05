//
// Created by Andrew Boun on 2/11/25.
//

#include <dungeon.h>
#include <priority_queue.h>

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

// Initialize a dungeon instance
void init_dungeon(Dungeon *d) {
    d->rooms = NULL;
    d->num_rooms = 0;

    d->num_up_stairs = 0;
    d->num_down_stairs = 0;

    d->up_stairs = NULL;
    d->down_stairs = NULL;

    // initialize later
    d->monsters = NULL;
    d->num_monsters = DEFAULT_NUM_MONSTERS; //default number of monsters
    d->num_monsters_alive = DEFAULT_NUM_MONSTERS;
}

// Generate a random dungeon and create a player
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

    place_player_randomly(d);

    return true;
}

void destroy_dungeon(Dungeon *d){
    free(d->rooms);
    free(d->up_stairs);
    free(d->down_stairs);
    free(d->monsters);
    return;
}


void start_gameplay(Dungeon *d){
    int num_entities = d->num_monsters + 1;

    PriorityQueue *pq = pq_create(num_entities, num_entities, NULL, NULL);

    // 0 = player, i + 1 = monster i
    pq_insert(pq, 0, NULL, 0);
    for (int i = 0; i < d->num_monsters; i++){
        pq_insert(pq, i + 1, NULL, 0);
    }

    pq_extract_min(pq);

    while (d->pc.alive && d ->num_monsters_alive > 0){
        usleep(250000);

        int entity_id = pq_get_min_key(pq);
        int current_time = pq_get_priority(pq, entity_id);
        int next_time;

        // Check if the entity is alive, if not, skip
        if (d->monsters[entity_id - 1].alive == 0) {
            pq_extract_min(pq);
            continue;
        }
        
        if (entity_id == 0) { // Player's turn
            // No player movement yet, does nothing
            move_player(d, d->pc.x, d->pc.y);
            next_time = current_time + (1000 / d->pc.speed);
        } else {
            move_monster(&d->monsters[entity_id - 1], d);
            next_time = current_time + (1000 / d->monsters[entity_id - 1].speed);
        }
        print_grid(d);
        
        // Reschedule entity's next turn
        pq_extract_min(pq);
        pq_insert(pq, entity_id, NULL, next_time);
    }

    if (d->pc.alive == 0) printf("Player is dead.\n");
    if (d->num_monsters_alive == 0) printf("All monsters are dead.\n");

    pq_destroy(pq);
}