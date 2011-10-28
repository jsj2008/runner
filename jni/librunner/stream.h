#pragma once

#include "common.h"

typedef struct stream_t stream_t;

int stream_init(void* data);
void* stream_read_file(const char* fname, long* psize);

int stream_open_reader(stream_t** pstream, const char* fname);
int stream_open_writer(stream_t** pstream, const char* fname);
void stream_close(stream_t* stream);
long stream_size(stream_t* stream);
long stream_seek(stream_t* stream, long offset, int mode);
long stream_read(stream_t* stream, void* buffer, long size);
long stream_write(stream_t* stream, const void* buffer, long size);

