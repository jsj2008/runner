#pragma once

int init(void* iodata);
void shutdown();
void resize(int width, int height);
void activated();
void deactivated();
int restore();
int update();
void pointer_down(int pointerId, float x, float y);
void pointer_up(int pointerId, float x, float y);
void pointer_move(int pointerId, float x, float y);
void key_down(int key);
void key_up(int key);

