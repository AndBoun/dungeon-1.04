//
// Created by Andrew Boun on 2/11/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <dungeon.h>

void print_hardness_info(const Dungeon *d){
    for (int i = 0; i < DUNGEON_HEIGHT; i++){
        for (int j = 0; j < DUNGEON_WIDTH; j++){
            printf("%d ", d->grid[i][j].hardness);
        }
        printf("\n");
    }
}

// Prints grid with a border
void print_grid(Dungeon *d){
    for(int i = 0; i < DUNGEON_WIDTH + 2; i++){
        printf("%c", HORIZONTAL_BORDER);
    }
    printf("\n");

    for (int i = 0; i < DUNGEON_HEIGHT; i++){
        printf("%c", VERTICAL_BORDER);
        for (int j = 0; j < DUNGEON_WIDTH; j++){
            printf("%c", d->grid[i][j].type);
        }
        printf("%c", VERTICAL_BORDER);
        printf("\n");
    }

    for(int i = 0; i < DUNGEON_WIDTH + 2; i++){
        printf("%c", HORIZONTAL_BORDER);
    }
    printf("\n");
}

void print_room_info(const Dungeon *d){
    for (int i = 0; i < d->num_rooms; i++){
        printf("Room %d\n", i + 1);
        printf("x: %d\n", d->rooms[i].x);
        printf("y: %d\n", d->rooms[i].y);
        printf("width: %d\n", d->rooms[i].width);
        printf("height: %d\n", d->rooms[i].height);
        printf("center_x: %d\n", d->rooms[i].center_x);
        printf("center_y: %d\n", d->rooms[i].center_y);
        printf("\n");
    }
}

void print_dist_map(const int dist_map[DUNGEON_HEIGHT][DUNGEON_WIDTH]){
    for(int i = 0; i < DUNGEON_WIDTH + 2; i++){
        printf("%c", HORIZONTAL_BORDER);
    }
    printf("\n");

    for (int i = 0; i < DUNGEON_HEIGHT; i++){
        printf("%c", VERTICAL_BORDER);
        for (int j = 0; j < DUNGEON_WIDTH; j++){
            switch (dist_map[i][j]) {
                case 0:
                    printf("%c", PLAYER);
                    break;
                case INF:
                    printf(" ");
                    break;
                case -1:
                    printf("X");
                    break;
                default:
                    printf("%d", dist_map[i][j] % 10);
                    break;
            }
        }
        printf("%c", VERTICAL_BORDER);
        printf("\n");
    }

    for(int i = 0; i < DUNGEON_WIDTH + 2; i++){
        printf("%c", HORIZONTAL_BORDER);
    }
    printf("\n");
}

// Print spaces out grid, with X as spaces
// For debugging
int print_x_grid(Dungeon *d){
    int r = 0;
    for (int y = 0; y < DUNGEON_HEIGHT; y++){
        for (int x = 0; x < DUNGEON_WIDTH; x++){
            char type = d->grid[y][x].type;

            if (type != ROCK && type != CORRIDOR && type != FLOOR && type != UP_STAIRS && type != DOWN_STAIRS && type != PLAYER &&
                type != '0' && type != '1' && type != '2' && type != '3' && type != '4' && type != '5' &&
                type != '6' && type != '7' && type != '8' && type != '9' && type != 'A' && type != 'B' && type != 'C' &&
                type != 'D' && type != 'E' && type != 'F'){
                printf("x: %d, y: %d, char: %d \n", x, y, type);
                r = -1;
            }


            if (type == ROCK){
                printf("X ");
                continue;
            }
            printf("%c ", type);
        }
        printf("\n");
    }
    return r;
}