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
    init_dungeon(&d);

    int load_flag = 0;
    int save_flag = 0;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--load") == 0) {
            load_flag = 1;
        } else if (strcmp(argv[i], "--save") == 0) {
            save_flag = 1;
        }
    }

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
    print_dist_map(d.non_tunneling_dist_map);
    
    create_tunneling_map(&d, d.pc.x, d.pc.y);
    print_dist_map(d.tunneling_dist_map);


    initialize_monsters(&d);
    print_grid(&d);

    destroy_dungeon(&d);

    return 0;
}