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

int debug_null(Dungeon *d) {
    if (d->grid[20][49].type == '\0') {
        return 1;
    }
    return 0;
}

int print_d_grid(Dungeon *d){
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

int print_l_grid(Dungeon *d){
    for (int y = DUNGEON_HEIGHT - 1; y < DUNGEON_HEIGHT; y++){
        for (int x = 0; x < DUNGEON_WIDTH; x++){
            printf("x: %d, y: %d, char: %d \n", x, y, d->grid[y][x].type);
        }
        printf("\n");
    }
    return 0;
}

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


    initialize_monsters(&d);
    // print_grid(&d);
    // print_d_grid(&d);

    start_gameplay(&d);

    // int c = 0;
    // while (d.pc.alive && d.num_monsters_alive > 0 && c < 2000) {
    //     usleep(250000);
    //     move_monster(&d.monsters[c % d.num_monsters], &d);
    //     if (print_d_grid(&d) == -1) {
    //         printf("Invalid grid\n");
    //         break;
    //     }
    //     c++;
    //     printf("%d\n", d.num_monsters_alive);
    // }
    // print_d_grid(&d);

    // print_l_grid(&d);

    // print_grid(&d);
    // printf("\nDungeon Grid Cell Types:\n");
    // for (int y = 0; y < DUNGEON_HEIGHT; y++) {
    //     for (int x = 0; x < DUNGEON_WIDTH; x++) {
    //         if (d.grid[y][x].type == ROCK){
    //             printf("X ");
    //             continue;
    //         }
    //         printf("%c ", d.grid[y][x].type);
    //     }
    //     printf("\n");
    // }

    // d.monsters = malloc(d.num_monsters * sizeof(Monster));
    // d.num_monsters_alive = d.num_monsters;
    // int x = 38, y = 13;
    // place_monster(&d, x, y, 0);
    // d.monsters[0].intelligent = 1;
    // d.monsters[0].tunneling = 1;
    // d.monsters[0].telepathy = 1;
    // d.monsters[0].erratic = 1;
    // d.monsters[0].symbol = '0';
    // d.grid[y][x].type = d.monsters[0].symbol;

    // place_monster(&d, 35, 6, 1);
    // d.monsters[1].intelligent = 10;
    // d.monsters[1].tunneling = 0;
    // d.monsters[1].telepathy = 0;
    // d.monsters[1].erratic = 0;
    // d.monsters[1].symbol = '1';
    // d.grid[6][35].type = d.monsters[1].symbol;

    // place_monster(&d, 36, 5, 2);
    // d.monsters[2].intelligent = 0;
    // d.monsters[2].tunneling = 0;
    // d.monsters[2].telepathy = 0;
    // d.monsters[2].erratic = 0;
    // d.monsters[2].symbol = '2';
    // d.grid[5][36].type = d.monsters[2].symbol;

    
    // print_grid(&d);
    
    // for (int i = 0; i < 100; i++){
    //     if (d.pc.alive == 0 || d.num_monsters_alive == 0) {
    //         if (d.pc.alive == 0) printf("Player is dead.\n");
    //         if (d.num_monsters_alive == 0) printf("All monsters are dead.\n");
    //         break;
    //     }
        
    //     usleep(400000);
    //     move_monster(&d.monsters[0], &d);
    //     print_grid(&d);
    //     printf("%d\n", d.num_monsters_alive);


    //     usleep(400000);
    //     move_monster(&d.monsters[1], &d);
    //     print_grid(&d);
    //     printf("%d\n", d.num_monsters_alive);


    //     usleep(400000);
    //     move_monster(&d.monsters[2], &d);
    //     print_grid(&d);

    //     printf("%d\n", d.num_monsters_alive);

    //     if (i == 8){
    //         move_player(&d, 60, 12);
    //         create_non_tunneling_map(&d, d.pc.x, d.pc.y);
    //         create_tunneling_map(&d, d.pc.x, d.pc.y);
    //     }
    // }

    destroy_dungeon(&d);

    return 0;
}