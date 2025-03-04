//
// Created by Andrew Boun on 3/3/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <character.h>

int init_player(Player *p, int x, int y) {
    p->x = x, p->y = y;
    p->speed = 10;
    p->alive = 1;

    return 0;
}

int init_monster(Monster *m, int x, int y) {
    m->intelligent = rand() % 2, m->telepathy = rand() % 2;
    m->tunneling = rand() % 2, m->erratic = rand() % 2;

    m->speed = (rand() % 16) + 5;

    m->x = x, m->y = y;
    m->pc_x = -1, m->pc_y = -1;

    m->symbol = 'A' + (rand() % 26);

    m->alive = 1;

    return 0;
}