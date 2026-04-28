#include "render.h"
#include "game.h"

#include <GL/glut.h>
#include <math.h>

void render_text(float x, float y, const char *text, void *font, float r, float g, float b)
{
    const unsigned char *p = (const unsigned char *)text;
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    while (*p)
    {
        glutBitmapCharacter(font, *p++);
    }
}

void render_rect(float x, float y, float w, float h, Color c)
{
    glColor4f(c.r, c.g, c.b, c.a);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void render_circle(Vec2 p, float radius, Color c, int segments)
{
    int i;
    glColor4f(c.r, c.g, c.b, c.a);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(p.x, p.y);
    for (i = 0; i <= segments; ++i)
    {
        float a = (float)i / (float)segments * 2.0f * (float)M_PI;
        glVertex2f(p.x + cosf(a) * radius, p.y + sinf(a) * radius);
    }
    glEnd();
}

void render_triangle(Vec2 p, float size, Color c)
{
    glColor4f(c.r, c.g, c.b, c.a);
    glBegin(GL_TRIANGLES);
    glVertex2f(p.x, p.y + size);
    glVertex2f(p.x - size * 0.8f, p.y - size * 0.7f);
    glVertex2f(p.x + size * 0.8f, p.y - size * 0.7f);
    glEnd();
}

void render_diamond(Vec2 p, float size, Color c)
{
    glColor4f(c.r, c.g, c.b, c.a);
    glBegin(GL_QUADS);
    glVertex2f(p.x, p.y + size);
    glVertex2f(p.x + size, p.y);
    glVertex2f(p.x, p.y - size);
    glVertex2f(p.x - size, p.y);
    glEnd();
}

void render_star(Vec2 p, float size, Color c)
{
    int i;
    glColor4f(c.r, c.g, c.b, c.a);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(p.x, p.y);
    for (i = 0; i <= 10; ++i)
    {
        float a = (float)i / 10.0f * 2.0f * (float)M_PI - (float)M_PI * 0.5f;
        float r = (i % 2 == 0) ? size : size * 0.45f;
        glVertex2f(p.x + cosf(a) * r, p.y + sinf(a) * r);
    }
    glEnd();
}

void render_pentagon(Vec2 p, float size, Color c)
{
    int i;
    glColor4f(c.r, c.g, c.b, c.a);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(p.x, p.y);
    for (i = 0; i <= 5; ++i)
    {
        float a = (float)i / 5.0f * 2.0f * (float)M_PI - (float)M_PI * 0.5f;
        glVertex2f(p.x + cosf(a) * size, p.y + sinf(a) * size);
    }
    glEnd();
}

void render_text_wrapped(float x, float y, const char *text, void *font, float r, float g, float b, float maxW)
{
    const unsigned char *p = (const unsigned char *)text;
    float curX = x;
    float curY = y;
    char word[64];
    int wordIdx = 0;
    glColor3f(r, g, b);

    while (*p)
    {
        if (*p == ' ' || *p == '\0')
        {
            word[wordIdx] = '\0';
            float wordW = 0;
            for (int i = 0; i < wordIdx; i++)
                wordW += glutBitmapWidth(font, word[i]);

            if (curX + wordW > x + maxW)
            {
                curX = x;
                curY += 15.0f;
            }

            glRasterPos2f(curX, curY);
            for (int i = 0; i < wordIdx; i++)
                glutBitmapCharacter(font, word[i]);
            curX += wordW + glutBitmapWidth(font, ' ');
            wordIdx = 0;
            if (*p == '\0')
                break;
        }
        else
        {
            if (wordIdx < 63)
                word[wordIdx++] = *p;
        }
        p++;
    }
    if (wordIdx > 0)
    {
        word[wordIdx] = '\0';
        glRasterPos2f(curX, curY);
        for (int i = 0; i < wordIdx; i++)
            glutBitmapCharacter(font, word[i]);
    }
}

void render_ortho(struct Game *g)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)g->width, (GLdouble)g->height, 0.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
