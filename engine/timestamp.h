#pragma once

#include <sys/time.h>

typedef struct timestamp_t
{
   struct timeval value;
} timestamp_t;

void timestamp_set(timestamp_t* timestamp);
long timestamp_diff(const timestamp_t* timestamp1, const timestamp_t* timestamp2);
long timestamp_elapsed(const timestamp_t* timestamp);
long timestamp_update(timestamp_t* timestamp);

