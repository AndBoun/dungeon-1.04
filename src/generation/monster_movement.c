//
// Created by Andrew Boun on 3/4/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <character.h>
#include <dungeon.h>
#include <dijkstra.h>

typedef struct {
    int x, y;
} Point;

static int has_line_of_sight(Dungeon *d, int x, int y){
    int pc_x = d->pc.x, pc_y = d->pc.y;

    // Check if PC is within sight radius
    int sight_radius = 20;
    int dx = x - pc_x;
    int dy = y - pc_y;
    if (dx*dx + dy*dy > sight_radius*sight_radius) {
        return 0; // Target is out of sight radius
    }

    // Bresenham's Line Algorithm to check for walls and obstacles
    int abs_dx = abs(dx);
    int abs_dy = abs(dy);

    int sx = (dx > 0) ? 1 : -1;
    int sy = (dy > 0) ? 1 : -1;

    int err = abs_dx - abs_dy;
    int curr_x = pc_x;
    int curr_y = pc_y;

    // Trace the line from player to target
    while (curr_x != x || curr_y != y) {
        int e2 = 2 * err;

        if (e2 > -abs_dy) {
            err -= abs_dy;
            curr_x += sx;
        }

        if (e2 < abs_dx) {
            err += abs_dx;
            curr_y += sy;
        }

        // Check if we hit a wall before reaching the target
        // Skip the starting position (player's position)
        if ((curr_x != pc_x || curr_y != pc_y) && (curr_x != x || curr_y != y)) {
            if (d->grid[curr_y][curr_x].hardness != 0) {
                return 0; // Line of sight blocked by a wall
            }
        }
    }

    return 1; // Line of sight is clear
}

static int find_monster_idx(Dungeon *d, int x, int y, char symbol){
    for (int i = 0; i < d->num_monsters; i++){
        if (d->monsters[i].x == x && d->monsters[i].y == y && d->monsters[i].symbol == symbol){
            return i;
        }
    }
    return -1;
}

// Check if the move is valid for non-tunneling monsters
static int is_valid_move_non_tunnel(Dungeon *d, int x, int y){
    return d->grid[y][x].hardness == 0;
}

// Check if the move is valid for tunneling monsters
static int is_valid_move_tunnel(Dungeon *d, int x, int y){
    return d->grid[y][x].hardness != MAX_HARDNESS;
}

// Get the next move for an intelligent monster
static Point get_next_intelligent_move(Dungeon *d, int x, int y, int pc_x, int pc_y, int tunneling){

    // Recalculate the distance maps if using PC's remembered position
    if (pc_x != d->pc.x || pc_y != d->pc.y){
        create_non_tunneling_map(d, pc_x, pc_y);
        create_tunneling_map(d, pc_x, pc_y);
    }

    int dx[] = {-1, -1, -1,  0, 0, 1, 1, 1};
    int dy[] = {-1,  0,  1, -1, 1,-1, 0, 1};

    Point p;
    p.x = INF;
    p.y = INF;
    int min_dist = INF;
    for (int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            int new_x = x + dx[i];
            int new_y = y + dy[i];

            if (new_x < 0 || new_x >= DUNGEON_WIDTH || new_y < 0 || new_y >= DUNGEON_HEIGHT){
                continue;
            }

            if (tunneling){
                if (d->tunneling_dist_map[new_y][new_x] < min_dist){
                    min_dist = d->tunneling_dist_map[new_y][new_x];
                    p.x = new_x;
                    p.y = new_y;
                }
            } else {
                if (d->non_tunneling_dist_map[new_y][new_x] < min_dist){
                    min_dist = d->non_tunneling_dist_map[new_y][new_x];
                    p.x = new_x;
                    p.y = new_y;
                }
            }
        }
    }

    // Recalculate the distance maps using PC's correct position
    if (pc_x != d->pc.x || pc_y != d->pc.y){
        create_non_tunneling_map(d, pc_x, pc_y);
        create_tunneling_map(d, pc_x, pc_y);
    }

    return p;
}

// Get the next move for an unintelligent monster
static Point get_next_unintelligent_move(Dungeon *d, int x, int y, int tunneling){
    Point p;
    p.x = x + (x < d->pc.x ? 1 : -1);
    p.y = y + (y < d->pc.y ? 1 : -1);

    if (tunneling){
        if (!is_valid_move_tunnel(d, p.x, y)){
            p.y = y;
        }
    } else {
        if (!is_valid_move_non_tunnel(d, x, p.y)){
            p.x = x;
        }
    }
    return p;
}

// Move a non-tunneling monster in the dungeon
static int move_non_tunnel(Dungeon *d, int x, int y, int new_x, int new_y){
    if (is_valid_move_non_tunnel(d, new_x, new_y)){
        d->grid[y][x].type = FLOOR;
        
        if (d->grid[new_y][new_x].type != FLOOR || d->grid[new_y][new_x].type != CORRIDOR){
            if (d->grid[new_y][new_x].type == PLAYER){
                d->pc.alive = 0;
            } else {
                int idx = find_monster_idx(d, new_x, new_y, d->grid[new_y][new_x].type);
                d->monsters[idx].alive = 0;
            }
        }
        d->grid[new_y][new_x].type = d->grid[y][x].type;

    }
    return 0;
}

static int move_tunnel(Dungeon *d, int x, int y, int new_x, int new_y){
    if (!is_valid_move_tunnel(d, new_x, new_y)) return 0;

    // Burrow through the wall
    if (d->grid[new_y][new_x].hardness <= 85) {
        d->grid[new_y][new_x].hardness = 0;
        d->grid[new_y][new_x].type = FLOOR;
    } else {
        d->grid[new_y][new_x].hardness -= 85;
        return 0;
    }

    move_non_tunnel(d, x, y, new_x, new_y);

    return 0;
}




int move_monster(Monster *m, Dungeon *d){
    int new_x, new_y;
    
    int intelligent = m->intelligent, telepathy = m->telepathy, tunneling = m->tunneling, erratic = m->erratic;

    // Update monster's knowledge of PC position
    if (telepathy || has_line_of_sight(d, m->x, m->y)) {
        // Monster can see the PC through telepathy or direct line of sight
        m->pc_x = d->pc.x;
        m->pc_y = d->pc.y;
    } else if (!intelligent) {
        // Non-intelligent monsters forget PC position when out of sight
        m->pc_x = -1;
        m->pc_y = -1;
    }

    // if player position is unknown, do not move
    if (m->pc_x == -1 || m->pc_y == -1) return 0;

    if (erratic && rand() % 2 == 1){ // erratic random movement, 50% chance
        do {
            new_x = m->x + (rand() % 3 - 1);
            new_y = m->y + (rand() % 3 - 1);
        } while (!(tunneling 
            ? is_valid_move_tunnel(d, new_x, new_y) 
            : is_valid_move_non_tunnel(d, new_x, new_y)));  
    } else {
        if (intelligent){ // intelligent movement towards PC
            Point p = get_next_intelligent_move(d, m->x, m->y, m->pc_x, m->pc_y, tunneling);
            new_x = p.x;
            new_y = p.y;
        } else {
            Point p = get_next_unintelligent_move(d, m->x, m->y, tunneling);
            if (p.x == m->x && p.y == m->y) return 0; // no movement
            new_x = p.x;
            new_y = p.y;
        }
    }
    

    if (tunneling){
        move_tunnel(d, m->x, m->y, new_x, new_y);
    } else {
        move_non_tunnel(d, m->x, m->y, new_x, new_y);
    }

    return 0;
}