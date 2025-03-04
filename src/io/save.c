//
// Created by Andrew Boun on 2/10/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dungeon.h>
#include <load_save.h>

static int calculate_size(Dungeon *d){
    int size = 12 + 4 + 4 + 2 + 1680 
                + 2 + (d->num_rooms * 4) 
                + 2 + (d->num_up_stairs * 2) 
                + 2 + (d->num_down_stairs * 2);

    return size;
}

int save(Dungeon *d){

    LoadSave ls;
    init_load_save(&ls, "wb");

    // Write marker
    fwrite(EXPECTED_MARKER, 1, 12, ls.f);

    // Write version
    uint32_t version = 0;
    version = htobe32(version);
    fwrite(&version, sizeof(uint32_t), 1, ls.f);

    // Write size
    uint32_t size = htobe32(calculate_size(d));
    fwrite(&size, 4, 1, ls.f);

    // Write PC
    uint8_t pc_x = (uint8_t)d->pc_x;
    uint8_t pc_y = (uint8_t)d->pc_y;
    fwrite(&pc_x, 1, 1, ls.f);
    fwrite(&pc_y, 1, 1, ls.f);

    // Write hardness
    for (int i = 0; i < DUNGEON_HEIGHT; i++){
        for (int j = 0; j < DUNGEON_WIDTH; j++){
            uint8_t hardness = (uint8_t)d->grid[i][j].hardness;
            fwrite(&hardness, 1, 1, ls.f);
        }
    }

    // Write number of rooms
    uint16_t num_rooms = htobe16((uint16_t)d->num_rooms);
    fwrite(&num_rooms, 2, 1, ls.f);

    // Write rooms
    for (int i = 0; i < d->num_rooms; i++){
        uint8_t x = (uint8_t)d->rooms[i].x;
        uint8_t y = (uint8_t)d->rooms[i].y;
        uint8_t width = (uint8_t)d->rooms[i].width;
        uint8_t height = (uint8_t)d->rooms[i].height;
        fwrite(&x, sizeof(uint8_t), 1, ls.f);
        fwrite(&y, sizeof(uint8_t), 1, ls.f);
        fwrite(&width, sizeof(uint8_t), 1, ls.f);
        fwrite(&height, sizeof(uint8_t), 1, ls.f);
    }

    // Write number of up stairs
    uint16_t num_up = htobe16((uint16_t)d->num_up_stairs);
    fwrite(&num_up, sizeof(uint16_t), 1, ls.f);

    // Write up stairs
    for (int i = 0; i < d->num_up_stairs; i++){
        uint8_t x = (uint8_t)d->up_stairs[i].x;
        uint8_t y = (uint8_t)d->up_stairs[i].y;
        fwrite(&x, sizeof(uint8_t), 1, ls.f);
        fwrite(&y, sizeof(uint8_t), 1, ls.f);
    }

    // Write number of down stairs
    uint16_t num_down = htobe16((uint16_t)d->num_down_stairs);
    fwrite(&num_down, sizeof(uint16_t), 1, ls.f);

    // Write down stairs
    for (int i = 0; i < d->num_down_stairs; i++){
        uint8_t x = (uint8_t)d->down_stairs[i].x;
        uint8_t y = (uint8_t)d->down_stairs[i].y;
        fwrite(&x, sizeof(uint8_t), 1, ls.f);
        fwrite(&y, sizeof(uint8_t), 1, ls.f);
    }


    fclose(ls.f);
    free(ls.dungeon_file);

    return 0;
}