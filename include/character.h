//
// Created by Andrew Boun on 3/3/25.
//

#ifndef CHARACTER_H
#define CHARACTER_H

typedef struct {
    int intelligent, telepathy, tunneling, erratic;
    int x, y; // characters location
    int pc_x, pc_y; // knowledge of the player's location, negative if unknown
    int speed;
    char symbol;
    int alive;
} Monster;

typedef struct {
    int x, y;       // player's location
    int speed;
    int alive;
} Player;

int init_player(Player *p, int x, int y);
int init_monster(Monster *m, int x, int y);

#endif