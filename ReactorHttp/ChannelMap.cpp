#include "ChannelMap.h"

ChannelMap* ChannelMapInit(int size)
{
    ChannelMap* map =(struct ChannelMap*)malloc(sizeof(struct ChannelMap));
    map->size = size;
    //map->list = new Channel * [size];
    map->list = (struct Channel**)malloc(sizeof(struct Channel*)*size);
    return map;
}

void ChannelMapClear(struct ChannelMap* map)
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

bool makeMapRoom(struct ChannelMap* map, int newSize, int unitSize)
{
    if (map->size < newSize) {
        int curSize = map->size;
        while (curSize < newSize) {
            curSize *= 2;
        }
        struct Channel** temp = (Channel**)realloc(map->list, curSize * unitSize);
        if (temp == NULL) {
            return false;
        }
        map->list = temp;
        memset(map->list[map->size], 0, (curSize - map->size) * unitSize);
        map->size = curSize;
    }

    return true;
}
