//
// Created by Andrew Boun on 3/4/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <character.h>
#include <dungeon.h>
#include <dijkstra.h>



// Check if the monster has line of sight to the target
// Monster has line of sight if within a 25 cell radius, with no walls in between
static int has_line_of_sight(Dungeon *d, int x, int y){
    int pc_x = d->pc.x, pc_y = d->pc.y;

    // Check if PC is within sight radius
    int sight_radius = 25;
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

// Check if the move is valid for non-tunneling monsters
static int is_valid_move_non_tunnel(Dungeon *d, int x, int y){
    if (x < 0 || x >= DUNGEON_WIDTH || y < 0 || y >= DUNGEON_HEIGHT) return 0;
    return d->grid[y][x].hardness == 0;
}

// Check if the move is valid for tunneling monsters
static int is_valid_move_tunnel(Dungeon *d, int x, int y){
    if (x < 0 || x >= DUNGEON_WIDTH || y < 0 || y >= DUNGEON_HEIGHT) return 0;
    return d->grid[y][x].hardness != MAX_HARDNESS;
}

// Randomly move a monster, used for erratic and unintelligent monsters
static Point random_move(Dungeon *d, int x, int y, int tunneling){
    Point p;
    Point valid_moves[8]; // Store valid adjacent coordinates
    int valid_count = 0;
    int (*is_valid_func)(Dungeon*, int, int) = 
        tunneling ? is_valid_move_tunnel : is_valid_move_non_tunnel;
    
    // Check all 8 adjacent cells
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            // Skip the cell itself (0,0)
            if (dx == 0 && dy == 0) continue;
            
            int new_x = x + dx;
            int new_y = y + dy;
            
            // If valid, add to our list of valid moves
            if (is_valid_func(d, new_x, new_y)) {
                valid_moves[valid_count].x = new_x;
                valid_moves[valid_count].y = new_y;
                valid_count++;
            }
        }
    }
    
    // If we have valid moves, randomly select one
    if (valid_count > 0) {
        int choice = rand() % valid_count;
        p = valid_moves[choice];
    } else {
        // No valid moves, stay in place
        p.x = x;
        p.y = y;
    }
    
    return p;
}

// Get the next move for an intelligent monster
static Point get_next_intelligent_move(Dungeon *d, Monster *m, int tunneling){

    // Recalculate the distance maps if using PC's remembered position
    if (m->pc_x != d->pc.x || m->pc_y != d->pc.y){
        create_non_tunneling_map(d, m->pc_x, m->pc_y);
        create_tunneling_map(d, m->pc_x, m->pc_y);
    }

    int dx[] = {-1, -1, -1,  0, 0, 1, 1, 1};
    int dy[] = {-1,  0,  1, -1, 1,-1, 0, 1};

    Point p;
    p.x = INF;
    p.y = INF;
    int min_dist = INF;
    for (int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            int new_x = m->x + dx[i];
            int new_y = m->y + dy[i];

            // Check if the new position is within bounds
            if (new_x < 0 || new_x >= DUNGEON_WIDTH || new_y < 0 || new_y >= DUNGEON_HEIGHT){
                continue;
            }

            if (tunneling){
                //Account for -1 being an invalid weight
                if (d->tunneling_dist_map[new_y][new_x] < min_dist &&
                    d->tunneling_dist_map[new_y][new_x] != -1){
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
    if (m->pc_x != d->pc.x || m->pc_y != d->pc.y){
        create_non_tunneling_map(d, m->pc_x, m->pc_y);
        create_tunneling_map(d, m->pc_x, m->pc_y);
    }

    return p;
}

// Get the next move for an unintelligent monster
static Point get_next_unintelligent_move(Dungeon *d, Monster *m, int tunneling){
    Point p;
    p.x = m->x, p.y = m->y;

    // If the monster doesn't know the PC's position, move randomly
    if (m->pc_x == -1 || m->pc_y == -1) return random_move(d, m->x, m->y, tunneling);

    // Calculate the direction towards the PC
    int dx = (m->x < d->pc.x) ? 1 : (m->x > d->pc.x) ? -1 : 0;
    int dy = (m->y < d->pc.y) ? 1 : (m->y > d->pc.y) ? -1 : 0;

    // Choose movement validation function based on monster type
    int (*is_valid_move)(Dungeon*, int, int) = tunneling ? 
            is_valid_move_tunnel : is_valid_move_non_tunnel;

    // Try to move one step horizontaly or vertically of the PC
    if (dx != 0 && is_valid_move(d, m->x + dx, m->y)){
        p.x = m->x + dx;
    } else if (dy != 0 && is_valid_move(d, m->x, m->y + dy)){
        p.y = m->y + dy;
    }

    return p;
}

// Move a non-tunneling monster in the dungeon
static int move_non_tunnel(Dungeon *d, Monster *m, int new_x, int new_y){
    if (is_valid_move_non_tunnel(d, new_x, new_y)){
        
        if (d->grid[new_y][new_x].type != FLOOR &&
            d->grid[new_y][new_x].type != CORRIDOR &&
            d->grid[new_y][new_x].type != UP_STAIRS &&
            d->grid[new_y][new_x].type != DOWN_STAIRS){
            if (d->grid[new_y][new_x].type == PLAYER){
                d->pc.alive = 0;
                d->grid[new_y][new_x].type = d->pc.curr_cell;
            } else {
                int ID = get_monster_ID(d, new_x, new_y);
                d->monsters[m->ID].alive = 0;
                d->grid[new_y][new_x].type = d->monsters[ID].curr_cell;
                d->monsters[ID].alive = 0;
                d->num_monsters_alive--;
            }
        }

        // Update the current cell type
        d->grid[m->y][m->x].type = m->curr_cell; // return the cell to its original type
        // print_grid(d);

        m->curr_cell = d->grid[new_y][new_x].type; // update the current cell type
        // print_grid(d);

        // Move the monster on the grid array
        d->grid[new_y][new_x].type = m->symbol;
        // print_grid(d);

        m->x = new_x;
        m->y = new_y;

    }
    return 0;
}

static int move_tunnel(Dungeon *d, Monster *m, int new_x, int new_y){
    if (!is_valid_move_tunnel(d, new_x, new_y)) return 0;

    // Burrow through the wall
    if (d->grid[new_y][new_x].hardness <= 85 && d->grid[new_y][new_x].hardness > 0){
        d->grid[new_y][new_x].hardness = 0;
        d->grid[new_y][new_x].type = CORRIDOR;
        
        // Recalculate the distance maps when dungeon changes
        create_non_tunneling_map(d, d->pc.x, d->pc.y);
        create_tunneling_map(d, d->pc.x, d->pc.y);
    } else {
        d->grid[new_y][new_x].hardness -= 85;
        return 0;
    }

    move_non_tunnel(d, m, new_x, new_y);

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

    // if player position is unknown, and monster is intelligent, don't move
    if (intelligent && !erratic && (m->pc_x == -1 || m->pc_y == -1)) return 0;

    if (erratic && rand() % 2 == 1){ // erratic random movement, 50% chance
        Point p = random_move(d, m->x, m->y, tunneling);
        new_x = p.x;
        new_y = p.y; 
    } else {
        if (intelligent){ // intelligent movement towards PC
            Point p = get_next_intelligent_move(d, m, tunneling);
            new_x = p.x;
            new_y = p.y;
        } else {
            Point p = get_next_unintelligent_move(d, m, tunneling);
            new_x = p.x;
            new_y = p.y;
        }
    }
    
    if (new_x == m->x && new_y == m->y) return 0; // No movement
    printf("Monster %d moved from (%d, %d) to (%d, %d)\n", m->ID, m->x, m->y, new_x, new_y);
    
    if (tunneling){
        move_tunnel(d, m, new_x, new_y);
    } else {
        move_non_tunnel(d, m, new_x, new_y);
    }

    return 0;
}