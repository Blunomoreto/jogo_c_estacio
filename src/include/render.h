#ifndef RENDER_H
#define RENDER_H

#include "types.h"

struct Game;

void render_text(float x, float y, const char *text, void *font, float r, float g, float b);
void render_rect(float x, float y, float w, float h, Color c);
void render_circle(Vec2 p, float radius, Color c, int segments);
void render_triangle(Vec2 p, float size, Color c);
void render_diamond(Vec2 p, float size, Color c);
void render_star(Vec2 p, float size, Color c);
void render_pentagon(Vec2 p, float size, Color c);
void render_text_wrapped(float x, float y, const char *text, void *font, float r, float g, float b, float maxW);
void render_ortho(struct Game *g);

#endif
