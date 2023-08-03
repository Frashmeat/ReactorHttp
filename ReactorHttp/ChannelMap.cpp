#include "ChannelMap.h"

ChannelMap* ChannelMapInit(int size)
{
    ChannelMap* map = new ChannelMap;
    map->size = size;
    map->list = new Channel * [size];
    return map;
}

void ChannelMapClear(ChannelMap* map)
{
    if (map != NULL) {
        for (int i = 0; i < map->size; i++)
        {
            if (map->list[i] != NULL) {
                free(map->list[i]);
            }
        }
        free(map->list);
        map->list = NULL;
    }
    map->size = 0;
}

bool makeMapRoom(ChannelMap* map, int newSize, int unitSize)
{
    if (map->size < newSize) {
        int curSize = map->size;
        while (curSize < newSize) {
            curSize *= 2;
        }
        Channel** temp = (Channel**)realloc(map->list, curSize * unitSize);
        if (temp == NULL) {
            return false;
        }
        map->list = temp;
        memset(map->list[map->size], 0, (curSize - map->size) * unitSize);
        map->size = curSize;
    }

    return true;
}
