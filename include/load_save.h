//
// Created by Andrew Boun on 2/11/25.
//

#ifndef LOAD_SAVE_H
#define LOAD_SAVE_H

#ifdef __linux__
#include <endian.h>
#endif

#ifdef __APPLE__
#include <machine/endian.h>
#include <libkern/OSByteOrder.h>
#define be16toh(x) OSSwapBigToHostInt16(x)
#define htobe16(x) OSSwapHostToBigInt16(x)
#define be32toh(x) OSSwapBigToHostInt32(x)
#define htobe32(x) OSSwapHostToBigInt32(x)
#define be64toh(x) OSSwapBigToHostInt64(x)
#define htobe64(x) OSSwapHostToBigInt64(x)
#define be8toh(x) (x)
#define htobe8(x) (x)
#endif

#define EXPECTED_MARKER "RLG327-S2025"

typedef struct {
    FILE *f;
    char *home;
    char *dungeon_file;
    int dungeon_file_length;
} LoadSave;

int init_load_save(LoadSave *ls, const char *mode);
int load(Dungeon *d);
int save(Dungeon *d);


#endif