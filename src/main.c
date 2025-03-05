//
// Created by Andrew Boun on 2/11/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include <dungeon.h>
#include <load_save.h>
#include <dijkstra.h>


int main(int argc, char *argv[]) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec ^ getpid());

    Dungeon d;

    int load_flag = 0;
    int save_flag = 0;
    int num_monsters = DEFAULT_NUM_MONSTERS;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--load") == 0) {
            load_flag = 1;
        } else if (strcmp(argv[i], "--save") == 0) {
            save_flag = 1;
        } else if (strcmp(argv[i], "--nummon") == 0) {
            num_monsters = atoi(argv[++i]);
        }
    }

    init_dungeon(&d);
    d.num_monsters = num_monsters;

    if (load_flag) {
        load(&d);
        print_grid(&d);
    } else {
        if (generate_random_dungeon(&d)) {
            print_grid(&d);
        } else {
            printf("Dungeon generation failed.\n");
            return 1;
        }
    }

    if (save_flag) {
        save(&d);
        printf("Dungeon saved.\n");
    }

    create_non_tunneling_map(&d, d.pc.x, d.pc.y);
    // print_dist_map(d.non_tunneling_dist_map);
    
    create_tunneling_map(&d, d.pc.x, d.pc.y);
    // print_dist_map(d.tunneling_dist_map);


    // initialize_monsters(&d);

    d.monsters = malloc(d.num_monsters * sizeof(Monster));
    d.num_monsters_alive = d.num_monsters;
    int x = 38, y = 13;
    place_monster(&d, x, y, 0);
    d.monsters[0].intelligent = 1;
    d.monsters[0].tunneling = 1;
    d.monsters[0].telepathy = 1;
    d.monsters[0].erratic = 0;
    d.monsters[0].symbol = '0';
    d.grid[y][x].type = d.monsters[0].symbol;

    
    // print_grid(&d);
    

    for (int i = 0; i < 30; i++){
        if (d.pc.alive == 0 || d.num_monsters_alive == 0) {
            if (d.pc.alive == 0) printf("Player is dead.\n");
            if (d.num_monsters_alive == 0) printf("All monsters are dead.\n");
            break;
        }
        
        usleep(400000);

        move_monster(&d.monsters[0], &d);
        print_grid(&d);

        if (i == 8){
            move_player(&d, 60, 12);
            create_non_tunneling_map(&d, d.pc.x, d.pc.y);
            create_tunneling_map(&d, d.pc.x, d.pc.y);
            print_dist_map(d.tunneling_dist_map);
        }
    }

    destroy_dungeon(&d);

    return 0;
}