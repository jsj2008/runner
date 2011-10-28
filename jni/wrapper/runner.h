#pragma once

struct AAssetManager;

int init(struct AAssetManager* assetManager);
void shutdown();
void resize(int width, int height);
void update();
void scroll(long dt, float dx1, float dy1, float dx2, float dy2);
void pointer_down(int pointerId, float x, float y);
void pointer_up(int pointerId, float x, float y);
void pointer_move(int pointerId, float x, float y);

