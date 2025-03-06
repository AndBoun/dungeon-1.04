//
// Created by Andrew Boun on 3/4/25.
//

#include <stdio.h>
#include <stdlib.h>

#include <character.h>
#include <dungeon.h>
#include <dijkstra.h>

int move_player(Dungeon *d, int x, int y){

    // Check if the move is invalid (same position, rock, or non-zero hardness)
    if ((x == d->pc.x && y == d->pc.y) || 
        (d->grid[y][x].type == ROCK) || 
        (d->grid[y][x].hardness > 0)) {
        printf("Player made an invalid move to (%d, %d)\n", x, y);
        return 0;
    }

    // Check if the new cell is occupied, and kill the occupant
    if (d->grid[y][x].type != FLOOR &&
        d->grid[y][x].type != CORRIDOR &&
        d->grid[y][x].type != UP_STAIRS &&
        d->grid[y][x].type != DOWN_STAIRS
    ){
        printf("Player killed a monster: %c\n", d->grid[y][x].type);
        kill_monster(d, x, y);
    }

    printf("Player moved from (%d, %d) to (%d, %d)\n", d->pc.x, d->pc.y, x, y);

    d->grid[d->pc.y][d->pc.x].type = d->pc.curr_cell; // return the cell to its original type
    d->pc.curr_cell = d->grid[y][x].type; // update the current cell type
    d->grid[y][x].type = PLAYER; // Move the player on the grid array

    d->pc.x = x;
    d->pc.y = y;

    create_non_tunneling_map(d, d->pc.x, d->pc.y);
    create_tunneling_map(d, d->pc.x, d->pc.y);

    return 0;
}

// Duplicated code, remove later once random player movement isn't needed
// Randomly move a monster, used for erratic and unintelligent monsters
static Point get_next_random_move(Dungeon *d, int x, int y, int tunneling)
{
    Point p;
    Point valid_moves[8]; // Store valid adjacent coordinates
    int valid_count = 0;

    // Check all 8 adjacent cells
    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            // Skip the cell itself (0,0)
            if (dx == 0 && dy == 0)
                continue;

            int new_x = x + dx;
            int new_y = y + dy;

            // If valid, add to our list of valid moves
            if (new_x >= 0 && new_x < DUNGEON_WIDTH && new_y >= 0 && new_y < DUNGEON_HEIGHT &&
                d->grid[new_y][new_x].hardness == 0)
            {
                valid_moves[valid_count].x = new_x;
                valid_moves[valid_count].y = new_y;
                valid_count++;
            }
        }
    }

    // If we have valid moves, randomly select one
    if (valid_count > 0)
    {
        int choice = rand() % valid_count;
        p = valid_moves[choice];
    }
    else
    {
        // No valid moves, stay in place
        p.x = x;
        p.y = y;
    }

    return p;
}

int move_player_randomly(Dungeon *d){
    Point p = get_next_random_move(d, d->pc.x, d->pc.y, 0);
    int x = p.x, y = p.y;

    move_player(d, x, y);

    return 0;
}