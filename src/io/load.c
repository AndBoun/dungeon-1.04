//
// Created by Andrew Boun on 2/10/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dungeon.h>
#include <load_save.h>

int init_load_save(LoadSave *ls, const char *mode) {
    ls->home = getenv("HOME");
    ls->dungeon_file_length = strlen(ls->home) + strlen("/.rlg327/dungeon") + 1;
    ls->dungeon_file = malloc(ls->dungeon_file_length * sizeof(*(ls->dungeon_file)));
    strcpy(ls->dungeon_file, ls->home);
    strcat(ls->dungeon_file, "/.rlg327/dungeon");

    ls->f = fopen(ls->dungeon_file, mode);
    if (!ls->f) {
        fprintf(stderr, "Error: Could not open file %s\n", ls->dungeon_file);
        return 1;
    }
    
    return 0;
}

static char* load_marker(LoadSave *ls) {
    char* marker = malloc(13);  // 12 bytes + 1 for null terminator

    // Read exactly 12 bytes
    if (fread(marker, sizeof(char), 12, ls->f) != 12) {
        fprintf(stderr, "Error: Could not read marker from file\n");
        fclose(ls->f);
        free(marker);
        return NULL;
    }

    // Null terminate the string
    marker[12] = '\0';

    return marker;
}

static int load_version(LoadSave *ls) {
    uint32_t version;

    // Read exactly 4 bytes
    if (fread(&version, sizeof(version), 1, ls->f) != 1) {
        fprintf(stderr, "Error: Could not read version from file\n");
        fclose(ls->f);
        return 1;
    }

    version = be32toh(version);

    return version;
}

static int load_size(LoadSave *ls) {
    uint32_t size;

    // Read exactly 4 bytes
    if (fread(&size, sizeof(size), 1, ls->f) != 1) {
        fprintf(stderr, "Error: Could not read size from file\n");
        fclose(ls->f);
        return 1;
    }

    size = be32toh(size);

    return size;
}

static int load_pc(LoadSave *ls, Dungeon *d) {
    u_int8_t pc[2]; // player character

    // Read exactly 2 bytes
    if (fread(pc, sizeof(u_int8_t), 2, ls->f) != 2) {
        fprintf(stderr, "Error: Could not read pc from file\n");
        fclose(ls->f);
        return 1;
    }
    init_player(&d->pc, pc[0], pc[1]);


    return 0;
}

static int load_hardness(LoadSave *ls, Dungeon *d) {
    uint8_t *hardness = malloc(1680); // 1680 bytes for hardness

    // Read exactly 1680 bytes
    if (fread(hardness, sizeof(u_int8_t), 1680, ls->f) != 1680) {
        fprintf(stderr, "Error: Could not read hardness from file\n");
        fclose(ls->f);
        return 1;
    }
    for (int i = 0; i < 1680; i++){
        Cell *cell = &d->grid[i / DUNGEON_WIDTH][i % DUNGEON_WIDTH];
        cell->hardness = hardness[i];
        cell->type = ROCK;
    }

    return 0;
}

static int load_num_rooms(LoadSave *ls, Dungeon *d) {
    uint16_t num_rooms;
    
    if (fread(&num_rooms, sizeof(uint16_t), 1, ls->f) != 1) {
        fprintf(stderr, "Error: Could not read num_rooms from file\n");
        fclose(ls->f);
        return 1;
    }

    num_rooms = be16toh(num_rooms);
    d->num_rooms = (int) num_rooms;
    d->rooms = malloc(d->num_rooms * sizeof(Room));

    return 0;
}

static int load_rooms(LoadSave *ls, Dungeon *d, int r){
    u_int8_t rooms[r * 4]; // 4 bytes per room

    if(fread(rooms, sizeof(u_int8_t), r * 4, ls->f) != r * 4){
        fprintf(stderr, "Error: Could not read rooms from file\n");
        fclose(ls->f);
        return 1;
    }

    for (int i = 0; i < r; i++){
        d->rooms[i].x = (int) rooms[i * 4];
        d->rooms[i].y = (int) rooms[i * 4 + 1];
        d->rooms[i].width = (int) rooms[i * 4 + 2];
        d->rooms[i].height = (int) rooms[i * 4 + 3];
        d->rooms[i].center_x = d->rooms[i].x + d->rooms[i].width / 2;
        d->rooms[i].center_y = d->rooms[i].y + d->rooms[i].height / 2;

        generate_room(d, d->rooms[i]);
    }

    return 0;
}

static int load_num_up_stairs(LoadSave *ls, Dungeon *d) {
    uint16_t num_up_stairs;

    if (fread(&num_up_stairs, sizeof(uint16_t), 1, ls->f) != 1) {
        fprintf(stderr, "Error: Could not read num_up_stairs from file\n");
        fclose(ls->f);
        return 1;
    }

    num_up_stairs = be16toh(num_up_stairs);
    d->num_up_stairs = (int) num_up_stairs;
    d->up_stairs = malloc(d->num_up_stairs * sizeof(Stair));

    return 0;
}

static int load_up_stairs(LoadSave *ls, Dungeon *d, int u) {
    u_int8_t up_stairs[u * 2]; // 2 bytes per up_stairs

    if (fread(up_stairs, sizeof(u_int8_t), u * 2, ls->f) != u * 2) {
        fprintf(stderr, "Error: Could not read up_stairs from file\n");
        fclose(ls->f);
        return 1;
    }

    for (int i = 0; i < u; i++) {
        d->grid[up_stairs[i * 2 + 1]][up_stairs[i * 2]].type = UP_STAIRS;
        d->up_stairs[i].x = up_stairs[i * 2];
        d->up_stairs[i].y = up_stairs[i * 2 + 1];

    }

    return 0;
}

static int load_num_down_stairs(LoadSave *ls, Dungeon *d) {
    uint16_t num_down_stairs;

    if (fread(&num_down_stairs, sizeof(uint16_t), 1, ls->f) != 1) {
        fprintf(stderr, "Error: Could not read num_down_stairs from file\n");
        fclose(ls->f);
        return 1;
    }

    num_down_stairs = be16toh(num_down_stairs);
    d->num_down_stairs = (int) num_down_stairs;
    d->down_stairs = malloc(d->num_down_stairs * sizeof(Stair));

    return 0;
}

static int load_down_stairs(LoadSave *ls, Dungeon *d, int down) {
    u_int8_t down_stairs[down * 2]; // 2 bytes per down_stairs

    if (fread(down_stairs, sizeof(u_int8_t), down * 2, ls->f) != down * 2) {
        fprintf(stderr, "Error: Could not read down_stairs from file\n");
        fclose(ls->f);
        return 1;
    }

    for (int i = 0; i < down; i++) {
        d->grid[down_stairs[i * 2 + 1]][down_stairs[i * 2]].type = DOWN_STAIRS;
        d->down_stairs[i].x = down_stairs[i * 2];
        d->down_stairs[i].y = down_stairs[i * 2 + 1];
    }

    return 0;
}

static int fill_in_corridors(Dungeon *d) {
    for (int i = 0; i < DUNGEON_HEIGHT; i++) {
        for (int j = 0; j < DUNGEON_WIDTH; j++) {
            if (d->grid[i][j].type == ROCK && d->grid[i][j].hardness == 0) {
                d->grid[i][j].type = CORRIDOR;
            }
        }
    }

    return 0;
}

int load(Dungeon *d){
    int r, u, down;

    LoadSave ls;
    init_load_save(&ls, "rb");
    printf("Home: %s\n", ls.home);
    printf("Dungeon file: %s\n", ls.dungeon_file);

    char* marker = load_marker(&ls);
    printf("Marker: %s\n", marker);
    if (strcmp(marker, EXPECTED_MARKER) != 0) {
        fprintf(stderr, "Error: Invalid marker\n");
        free(marker);
        fclose(ls.f);
        free(ls.dungeon_file);
        return 1;
    }
    free(marker);

    // load_version(&ls);
    uint32_t version = load_version(&ls);
    printf("Version: %u\n", version);

    // load_size(&ls);
    uint32_t size = load_size(&ls);
    printf("Size: %u\n", size);

    load_pc(&ls, d); // player character
    printf("PC: x: %u \t y: %u \n", d->pc.x, d->pc.y);

    load_hardness(&ls, d);
    // print_hardness_info(d);

    load_num_rooms(&ls, d);
    r = d->num_rooms;
    printf("Number of rooms: %u\n", d->num_rooms);

    load_rooms(&ls, d, r);
    // print_grid(&d);

    load_num_up_stairs(&ls, d);
    u = d->num_up_stairs;
    printf("Number of up stairs: %u\n", d->num_up_stairs);

    load_up_stairs(&ls, d, u);
    // print_grid(d);

    load_num_down_stairs(&ls, d);
    down = d->num_down_stairs;
    printf("Number of down stairs: %u\n", d->num_down_stairs);

    load_down_stairs(&ls, d, down);
    // print_grid(d);

    fill_in_corridors(d);
    // print_grid(d);

    d->grid[d->pc.y][d->pc.x].type = PLAYER;

    fclose(ls.f);
    free(ls.dungeon_file);
    return 0;
}