//
// Created by Andrew Boun on 2/3/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dungeon.h>


/**
 * Generates a corridor between two points in the Dungeon grid.
 * The corridor can be horizontal, vertical, or diagonal based on random selection.
 * 
 * @param x1 The starting x-coordinate of the corridor
 * @param y1 The starting y-coordinate of the corridor
 * @param x2 The ending x-coordinate of the corridor
 * @param y2 The ending y-coordinate of the corridor
 * 
 * The function updates the global 'grid' array, converting ROCK cells to CORRIDOR
 * cells along the path. For diagonal movements, it ensures connectivity by adding
 * additional corridor cells when necessary.
 * 
 * Direction values:
 * 0 - horizontal movement
 * 1 - vertical movement
 * 2 - diagonal movement
 */
void generate_corridor(Dungeon *d, int x1, int y1, int x2, int y2){
    int x = x1;
    int y = y1;

    // 0 for horizontal, 1 for vertical, 2 for diagonal
    int direction = rand() % 3;

    while (x != x2 || y != y2){
        if (direction == 0 && x != x2){
            if (x < x2) x++; 
            else x--;

        } else if (direction == 1 && y != y2){
            if (y < y2) y++;
            else y--;

        } else {
            if (x != x2){
                if (x < x2) x++;
                else x--;

                // Ensure diagonal movement is possible by only
                // up and down movements by adding an extra corridor
                if (d->grid[y][x].type == ROCK) {
                    d->grid[y][x].type = CORRIDOR;
                    d->grid[y][x].hardness = MIN_HARDNESS;
                }
                
            }

            if (y < y2){
                y++;
            } else if (y > y2){
                y--;
            }
        }

        if (d->grid[y][x].type == ROCK) {
            d->grid[y][x].type = CORRIDOR;
            d->grid[y][x].hardness = MIN_HARDNESS;
        }
    }
}