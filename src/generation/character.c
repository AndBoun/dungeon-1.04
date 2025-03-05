//
// Created by Andrew Boun on 3/3/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <character.h>
#include <dungeon.h>

int init_player(Player *p, int x, int y, char curr_cell) {
    p->x = x, p->y = y;
    p->speed = 10;
    p->alive = 1;
    p->curr_cell = curr_cell;

    return 0;
}

int init_monster(Monster *m, int x, int y, int ID, char curr_cell) {
    m->intelligent = rand() % 2, m->telepathy = rand() % 2;
    m->tunneling = rand() % 2, m->erratic = rand() % 2;

    m->speed = (rand() % 16) + 5;

    m->x = x, m->y = y;
    m->pc_x = -1, m->pc_y = -1;

    int traits = 
        m->intelligent +
        (m->telepathy << 1) +
        (m->tunneling << 2) +
        (m->erratic << 3);

    if (traits < 10) {
        m->symbol = '0' + traits; // 0-9
    } else {
        m->symbol = 'A' + (traits - 10); // A-F
    }


    m->alive = 1;
    m->ID = ID;
    m->curr_cell = curr_cell;

    return 0;
}

static Point get_random_coordinates(Dungeon *d){
    int x, y;
    do {
        x = rand() % PLACABLE_WIDTH + 1;
        y = rand() % PLACABLE_HEIGHT + 1;
    } while (d->grid[y][x].type == CORRIDOR || d->grid[y][x].type == ROCK);

    Point p;
    p.x = x, p.y = y;
    return p;
}

int place_monster(Dungeon *d, int x, int y, int idx){
    Monster m;
    init_monster(&m, x, y, idx, d->grid[y][x].type);
    d->monsters[idx] = m;
    d->grid[y][x].type = d->monsters[idx].symbol;

    return 1;
}

int place_monster_randomly(Dungeon *d, int idx){
    Point p = get_random_coordinates(d);
    int x = p.x, y = p.y;

    place_monster(d, x, y, idx);
    
    return 1;
}

int place_player_randomly(Dungeon *d){
    Point p = get_random_coordinates(d);
    int x = p.x, y = p.y;

    init_player(&d->pc, x, y, d->grid[y][x].type);
    d->grid[y][x].type = PLAYER;

    return 1;
}

int get_monster_ID(Dungeon *d, int x, int y){
    int ID = -1;
    for (int i = 0; i < d->num_monsters; i++){
        if (d->monsters[i].x == x && d->monsters[i].y == y){
            ID = i;
            break;
        }
    }
    return ID;
}