#include "timestamp.h"
#include "common.h"

void timestamp_set(timestamp_t* timestamp)
{
   gettimeofday(&timestamp->value, NULL);
}

long timestamp_diff(const timestamp_t* timestamp1, const timestamp_t* timestamp2)
{
   return (timestamp1->value.tv_sec - timestamp2->value.tv_sec) * 1000 + (timestamp1->value.tv_usec - timestamp2->value.tv_usec) / 1000;
}

long timestamp_elapsed(const timestamp_t* timestamp)
{
   timestamp_t current;
   timestamp_set(&current);
   return timestamp_diff(&current, timestamp);
}

long timestamp_update(timestamp_t* timestamp)
{
   timestamp_t prev = *timestamp;
   timestamp_set(timestamp);
   return timestamp_diff(timestamp, &prev);
}

