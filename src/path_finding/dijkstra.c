//
// Created by Andrew Boun on 2/25/25.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dijkstra.h>


#define INF 1000000000

// Helper struct to store node data
typedef struct {
    int x;
    int y;
} NodeData;

static int find_all_paths(
    Dungeon *d,
    int dist_map[DUNGEON_HEIGHT][DUNGEON_WIDTH],
    int start_x,
    int start_y,
    int tunneling  // 0 for non-tunneling, 1 for tunneling
);

// Copy function for PriorityQueue
static void* copy_node_data(const void* data) {
    NodeData* new_data = malloc(sizeof(NodeData));
    memcpy(new_data, data, sizeof(NodeData));
    return new_data;
}

// Destroy function for PriorityQueue
static void destroy_node_data(void* data) {
    free(data);
}

// Convert x,y coordinates to unique key for priority queue
static int coord_to_key(int x, int y) {
    return y * DUNGEON_WIDTH + x;
}

// New function to initialize a distance map
static void initialize_distance_map(int dist_map[DUNGEON_HEIGHT][DUNGEON_WIDTH]) {
    for (int i = 0; i < DUNGEON_HEIGHT; i++)
        for (int j = 0; j < DUNGEON_WIDTH; j++)
            dist_map[i][j] = INF;
}

int create_non_tunneling_map(Dungeon *d, int pc_x, int pc_y) {
    // Initialize the distance map using the new function
    initialize_distance_map(d->non_tunneling_dist_map);
    find_all_paths(d, d->non_tunneling_dist_map, pc_x, pc_y, 0);
    return 0;
}

// Adds support for printing unreachable areas for bonus maps
int create_tunneling_map(Dungeon *d, int pc_x, int pc_y) {
    // Initialize the distance map using the new function
    initialize_distance_map(d->tunneling_dist_map);
    find_all_paths(d, d->tunneling_dist_map, pc_x, pc_y, 1);
    return 0;
}

void static bonus_map_support(Dungeon *d){
    for (int i = 0; i < DUNGEON_HEIGHT; i++){
        for (int j = 0; j < DUNGEON_WIDTH; j++){
            if (
                d->non_tunneling_dist_map[i][j] == INF &&
                d->grid[i][j].type != ROCK
            ){
                d->non_tunneling_dist_map[i][j] = -1;
            }
            
        }
    }
}

static int find_all_paths(
    Dungeon *d,
    int dist_map[DUNGEON_HEIGHT][DUNGEON_WIDTH],
    int start_x,
    int start_y,
    int tunneling  // 0 for non-tunneling, 1 for tunneling
) {
    int dx[] = {-1, -1, -1,  0, 0, 1, 1, 1};
    int dy[] = {-1,  0,  1, -1, 1,-1, 0, 1};
    int rows = DUNGEON_HEIGHT, cols = DUNGEON_WIDTH;
    PriorityQueue* pq = pq_create(rows * cols, rows * cols, copy_node_data, destroy_node_data);



    // Initialize start node
    NodeData start_data = {start_x, start_y};
    pq_insert(pq, coord_to_key(start_x, start_y), &start_data, 0);
    dist_map[start_y][start_x] = 0;

    
    while (!pq_is_empty(pq)) {
        NodeData* current = pq_extract_min(pq);
        int curr_x = current->x, curr_y = current->y;
        
        for (int i = 0; i < 8; i++) {
            int next_x = curr_x + dx[i];
            int next_y = curr_y + dy[i];
            
            if (next_x >= 0 && next_x < cols &&
                next_y >= 0 && next_y < rows) {
                
                int curr_hardness = d->grid[curr_y][curr_x].hardness;
                int edge_cost = 0;
                
                // Check if movement is valid based on tunneling
                int neighbor_hardness = d->grid[next_y][next_x].hardness;
                
                if (!tunneling) {
                    if (neighbor_hardness != 0)
                        continue;
                    edge_cost = 1;
                } else {
                    if (neighbor_hardness == MAX_HARDNESS)
                        continue;
                    edge_cost = 1 + (curr_hardness / 85);
                }
                
                int new_dist = dist_map[curr_y][curr_x] + edge_cost;
                if (new_dist < dist_map[next_y][next_x]) {
                    dist_map[next_y][next_x] = new_dist;
                    NodeData next_data = {next_x, next_y};
                    pq_insert(pq, coord_to_key(next_x, next_y), &next_data, new_dist);
                }
            }
        }
        
        free(current);
    }

    bonus_map_support(d);
    
    pq_destroy(pq);
    return 0;
}