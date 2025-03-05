//
// Created by Andrew Boun on 3/4/25.
//

#include <stdio.h>
#include <stdlib.h>

#include <character.h>
#include <dungeon.h>

int move_player(Dungeon *d, int x, int y){
    if (d->grid[y][x].type == ROCK) return 0;
    if (d->grid[y][x].hardness > 0) return 0;

    // Check if the new cell is occupied, and kill the occupant
    if (d->grid[y][x].type != FLOOR &&
        d->grid[y][x].type != CORRIDOR &&
        d->grid[y][x].type != UP_STAIRS &&
        d->grid[y][x].type != DOWN_STAIRS
    ){
        kill_monster(d, x, y);
    }


    d->grid[d->pc.y][d->pc.x].type = d->pc.curr_cell; // return the cell to its original type
    d->pc.curr_cell = d->grid[y][x].type; // update the current cell type
    d->grid[y][x].type = PLAYER; // Move the player on the grid array

    d->pc.x = x;
    d->pc.y = y;

    return 0;
}