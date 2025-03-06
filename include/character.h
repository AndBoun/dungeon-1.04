//
// Created by Andrew Boun on 3/3/25.
//

#ifndef CHARACTER_H
#define CHARACTER_H

#define PLAYER_ID 0

typedef struct {
    int intelligent, telepathy, tunneling, erratic;
    int x, y; // characters location
    int pc_x, pc_y; // knowledge of the player's location, negative if unknown
    int speed;
    char symbol; // symbol of the monster
    int alive;
    int ID;
    char curr_cell; // type of cell monster is on
} Monster;

typedef struct {
    int x, y;       // player's location
    int speed;
    int alive;
    char curr_cell; // type of cell player is on
} Player;

int init_player(Player *p, int x, int y, char curr_cell);
int init_monster(Monster *m, int x, int y, int ID, char curr_cell);


#endif