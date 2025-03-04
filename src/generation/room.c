//
// Created by Andrew Boun on 2/11/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <dungeon.h>

/**
 * Determines if a room can be placed at the specified location in the grid.
 * 
 * @param room The room structure containing x,y coordinates, width and height
 * @return 1 if room can be inserted, 0 otherwise
 * 
 * Checks two conditions:
 * 1. If the room dimensions would exceed the placeable grid boundaries
 * 2. If the room area (including 1-tile border) would overlap with existing floor tiles
 */
int can_insert_room(Dungeon *d, Room room){

    // Check if room is within bounds
    // Do not check for '(x + width - 1) >= PLACEABLE' because we need to account for starting at 1
    // (room.width - 1) and (room.height - 1) to account for starting room at that index (x, y) inclusive
    if (room.x + (room.width - 1) > PLACABLE_WIDTH || room.y + (room.height - 1) > PLACABLE_HEIGHT){ 
        return 0;
    }

    // Check area of room and bordering area to see if it is empty
    for (int i = room.x - 1; i < room.x + room.width + 1; i++){
        for (int j = room.y - 1; j < room.y + room.height + 1; j++){
            if (d->grid[j][i].type == FLOOR){
                return 0; // Room overlaps with existing floor tiles
            }
        }
    }

    return 1; // Room can be inserted
}

/**
 * @brief Generates a room in the grid by filling it with floor tiles
 * 
 * Given a Room struct containing the position (x,y) and dimensions (width, height),
 * this function fills the corresponding grid cells with FLOOR characters.
 * The room is generated from its top-left corner (inclusive) to its bottom-right corner.
 * 
 * @param room A Room struct containing:
 *             - x: The x-coordinate of the top-left corner
 *             - y: The y-coordinate of the top-left corner
 *             - width: The width of the room
 *             - height: The height of the room
 * 
 * @pre The room must fit within the grid boundaries
 * @pre can_insert_room must return true for the given room
 * @pre grid must be properly initialized
 */
void generate_room(Dungeon *d, Room room){
    for(int i = room.x; i < room.x + room.width; i++){
        for(int j = room.y; j < room.y + room.height; j++){
            d->grid[j][i].type = FLOOR;
            d->grid[j][i].hardness = MIN_HARDNESS;
        }
    }
}

/**
 * @brief Generates a random room with random dimensions and position
 * 
 * This function attempts to generate a random room within the dungeon grid.
 * It will make multiple attempts to place a room without overlapping existing rooms.
 * If too many attempts are made (MAX_ATTEMPTS), the function fails.
 * 
 * The room dimensions are randomly generated but must meet minimum size requirements
 * (MIN_ROOM_WIDTH and MIN_ROOM_HEIGHT). The room position is randomly placed within
 * the PLACABLE_WIDTH and PLACABLE_HEIGHT bounds.
 * 
 * @param idx Index where the generated room should be stored in the rooms array
 * @return true if room was successfully generated and placed
 * @return false if room could not be placed after maximum attempts
 */
bool generate_random_room(Dungeon *d, int idx){
    Room room;
    int attempts = 0;

    do {
        attempts++;
        if (attempts >= MAX_ATTEMPTS) { // Reset grid if too many attempts
            return false;
        }

        room.x = (rand() % PLACABLE_WIDTH) + 1;
        room.y = (rand() % PLACABLE_HEIGHT) + 1;

        // Subract current coordinate from the maximum width and height
        // to get the maximum available width and height
        int max_width = PLACABLE_WIDTH - room.x;
        int max_height = PLACABLE_HEIGHT - room.y;

        int random_width = (max_width > 0) ? (rand() % max_width) : MIN_ROOM_WIDTH;
        int random_height = (max_height > 0) ? (rand() % max_height) : MIN_ROOM_HEIGHT;
        room.width = (random_width < MIN_ROOM_WIDTH) ? MIN_ROOM_WIDTH : random_width;
        room.height = (random_height < MIN_ROOM_HEIGHT) ? MIN_ROOM_HEIGHT : random_height;

        room.center_x = room.x + (room.width / 2);
        room.center_y = room.y + (room.height / 2);

    } while (!can_insert_room(d, room));

    d->rooms[idx] = room;
    generate_room(d, room);
    return true;
}