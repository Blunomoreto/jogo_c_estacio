#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* =========================================================================
   1. SIMULATION & PHYSICS CONSTANTS
   =========================================================================
   WORLD_HEIGHT is our vertical 'anchor'. All math is relative to this.
   If the window is resized, we expand the world_width proportionally to
   ensure shapes do not stretch (Aspect Ratio = 1:1).

   G-LIMIT: MAX_LAT_ACCEL defines the structural limit of the airframe.
   If the guidance law commands 20,000 units/s^2, the missile will cap
   at 7888 units/s^2, simulating realistic 'overshoot' in high-G turns.
   ========================================================================= */
#define WORLD_HEIGHT 4096.0f
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define MAX_MISSILES 25
#define MAX_BALLOONS 5
#define FPS 60
#define DT (1.0f / FPS)

#define GRAVITY 1314.4f       // Accel due to gravity (pixels/s^2)
#define MAX_LAT_ACCEL 7888.0f // Max "G" the missile can pull (Overload)

/* =========================================================================
   2. GLOBAL STATE
   ========================================================================= */
int current_win_w = WINDOW_WIDTH;
int current_win_h = WINDOW_HEIGHT;
float world_width = 4096.0f * (640.0f / 480.0f);
float absolute_time = 0.0f;

float turret_x = 0.0f;
const float turret_y = 120.0f;
int player_lives = 5;
bool game_over = false;

/* =========================================================================
   3. STRUCTURES
   ========================================================================= */

typedef struct
{
    float x, y, vx, vy, ax, ay;

    // TARGET VECTOR MATH:
    // base_x/y: The moving center-point of the balloon.
    // dir_x/y: Normalized vector pointing from spawn to player.
    // perp_x/y: Vector 90 deg to dir, used to create the 'weave' offset.
    float base_x, base_y, dir_x, dir_y, perp_x, perp_y;
    float amplitude, omega, time_offset, speed;
    bool active;
} Target;

typedef enum
{
    GUIDANCE_APNG,
    GUIDANCE_PURE_PURSUIT
} GuidanceLaw;

typedef struct
{
    float x, y, vx, vy;
    float actual_lat_accel; // The actual force currently produced by the fins
    float fuel_timer;
    int target_idx;
    bool active, missed;
    float prev_dist, sd_timer;
    GuidanceLaw guidance;
} Missile;

Target targets[MAX_BALLOONS];
Missile missiles[MAX_MISSILES];

/* =========================================================================
   4. ANALYTICAL TARGET GENERATION
   =========================================================================
   Target path is defined as: P(t) = (Origin + Direction * Speed * t) + (Perp * sin(t))
   By using this analytical form, we can provide the APNG guidance law with
   the EXACT instantaneous acceleration of the target, allowing for
   perfect predictive interception.
   ========================================================================= */
void spawn_target(int i)
{
    bool left = (rand() % 2 == 0);
    targets[i].base_x = left ? -800.0f : world_width + 800.0f;
    targets[i].base_y = 2500.0f + (rand() % 2000);

    // Vector from spawn to turret
    float dx = turret_x - targets[i].base_x;
    float dy = turret_y - targets[i].base_y;
    float mag = hypotf(dx, dy);

    // Normalized direction unit-vector
    targets[i].dir_x = dx / mag;
    targets[i].dir_y = dy / mag;

    // Perpendicular unit-vector (Rotation matrix applied: -y, x)
    targets[i].perp_x = -targets[i].dir_y;
    targets[i].perp_y = targets[i].dir_x;

    targets[i].speed = 700.0f + (rand() % 300);
    targets[i].amplitude = 250.0f + (rand() % 150);
    targets[i].omega = 1.0f + (float)(rand() % 15) / 10.0f;
    targets[i].time_offset = (float)(rand() % 100);
    targets[i].active = true;
}

void init_sim()
{
    srand((unsigned int)time(NULL));
    turret_x = world_width / 2.0f;
    for (int i = 0; i < MAX_BALLOONS; i++)
        spawn_target(i);
    for (int i = 0; i < MAX_MISSILES; i++)
        missiles[i].active = false;
}

/* =========================================================================
   5. COORDINATE MAPPING (RESOLUTION INDEPENDENCE)
   =========================================================================
   This logic handles "Dynamic FOV". Instead of stretching the image, we
   calculate a new world_width. The WORLD_HEIGHT is always 4096 units.
   ========================================================================= */
void reshape(int w, int h)
{
    current_win_w = w;
    current_win_h = h;
    glViewport(0, 0, w, h);

    // Adjust horizontal coordinate range based on new aspect ratio
    world_width = WORLD_HEIGHT * ((float)w / (float)h);
    turret_x = world_width / 2.0f;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, world_width, 0.0, WORLD_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
}

/* =========================================================================
   6. CORE UPDATE LOOP (PHYSICS & GUIDANCE)
   ========================================================================= */
void update(int value)
{
    if (game_over)
        return;
    absolute_time += DT;

    // --- A. TARGET PHYSICS (Vector-based weaving homing) ---
    for (int i = 0; i < MAX_BALLOONS; i++)
    {
        if (!targets[i].active)
            continue;
        Target *t = &targets[i];

        // Move the center point toward the turret
        t->base_x += t->dir_x * t->speed * DT;
        t->base_y += t->dir_y * t->speed * DT;

        float time = absolute_time + t->time_offset;
        float weave = t->amplitude * sinf(t->omega * time);

        // Final Position = Base + Perpendicular Offset
        t->x = t->base_x + t->perp_x * weave;
        t->y = t->base_y + t->perp_y * weave;

        // Velocity (First Derivative)
        float d_weave = t->amplitude * t->omega * cosf(t->omega * time);
        t->vx = t->dir_x * t->speed + t->perp_x * d_weave;
        t->vy = t->dir_y * t->speed + t->perp_y * d_weave;

        // Acceleration (Second Derivative) - Critical for APNG 'Augmentation'
        float dd_weave = -t->amplitude * t->omega * t->omega * sinf(t->omega * time);
        t->ax = t->perp_x * dd_weave;
        t->ay = t->perp_y * dd_weave;

        // Check for player integrity loss
        if (hypotf(t->x - turret_x, t->y - turret_y) < 120.0f)
        {
            player_lives--;
            if (player_lives <= 0)
                game_over = true;
            spawn_target(i);
        }
        if (t->y < -1000.0f)
            spawn_target(i);
    }

    // --- B. MISSILE PHYSICS & GUIDANCE ---
    for (int i = 0; i < MAX_MISSILES; i++)
    {
        if (!missiles[i].active)
            continue;
        Missile *m = &missiles[i];

        if (m->missed)
        {
            m->sd_timer -= DT;
            if (m->sd_timer <= 0)
                m->active = false;
            continue;
        }

        float speed = hypotf(m->vx, m->vy);
        float gamma = atan2f(m->vy, m->vx); // Flight Path Angle
        float a_cmd = 0.0f;                 // Command generated by Guidance Law

        if (m->target_idx != -1 && targets[m->target_idx].active)
        {
            Target *t = &targets[m->target_idx];
            float dx = t->x - m->x, dy = t->y - m->y;
            float dist = hypotf(dx, dy), los_angle = atan2f(dy, dx);

            if (m->prev_dist < 9999.0f && dist > m->prev_dist && dist < 600.0f)
            {
                m->missed = true;
                m->sd_timer = 0.2f;
            }
            m->prev_dist = dist;

            if (!m->missed)
            {
                if (m->guidance == GUIDANCE_APNG)
                {
                    /* APNG MATH:
                       Vc (Closing Velocity) = Rate at which distance is shrinking.
                       los_rate (Line of Sight Rate) = Rotational speed of the LOS vector.
                       a_t_perp = Target's actual acceleration perpendicular to LOS. */
                    float rel_vx = t->vx - m->vx, rel_vy = t->vy - m->vy;
                    float Vc = -(dx * rel_vx + dy * rel_vy) / dist;
                    float los_rate = (dx * rel_vy - dy * rel_vx) / (dist * dist);
                    float a_t_perp = -t->ax * sinf(los_angle) + t->ay * cosf(los_angle);

                    // APNG Command: N * Vc * los_rate + (N/2) * a_t_perp
                    a_cmd = 3.5f * Vc * los_rate + (3.5f / 2.0f) * a_t_perp;
                }
                else
                {
                    /* PURE PURSUIT MATH:
                       Directly attempts to align Velocity Vector with Target Position.
                       Highly inefficient for crossing targets (tail-chase). */
                    float heading_err = los_angle - gamma;
                    while (heading_err > M_PI)
                        heading_err -= 2 * M_PI;
                    while (heading_err < -M_PI)
                        heading_err += 2 * M_PI;
                    a_cmd = 5.0f * speed * heading_err;
                }
            }
        }
        else
            m->target_idx = -1;

        // Apply Gravity Compensation
        a_cmd += GRAVITY * cosf(gamma);

        // Apply G-Limit (Saturation)
        if (a_cmd > MAX_LAT_ACCEL)
            a_cmd = MAX_LAT_ACCEL;
        if (a_cmd < -MAX_LAT_ACCEL)
            a_cmd = -MAX_LAT_ACCEL;

        // AUTOPILOT LAG (First-Order Filter):
        // Missile cannot produce commanded Gs instantly.
        // 0.15s is the time constant for the fin-actuators to respond.
        m->actual_lat_accel += (a_cmd - m->actual_lat_accel) * (DT / 0.15f);

        // Thrust vs Drag
        float a_thrust = (m->fuel_timer > 0) ? 4800.0f : (-0.0125f * speed * speed);
        if (m->fuel_timer > 0)
            m->fuel_timer -= DT;

        // Final Kinematic Integration
        m->vx += (a_thrust * cosf(gamma) - m->actual_lat_accel * sinf(gamma)) * DT;
        m->vy += (a_thrust * sinf(gamma) + m->actual_lat_accel * cosf(gamma) - GRAVITY) * DT;
        m->x += m->vx * DT;
        m->y += m->vy * DT;

        if (m->x < -1000 || m->x > world_width + 1000 || m->y < -1000 || m->y > WORLD_HEIGHT + 1000)
            m->active = false;

        if (m->target_idx != -1 && !m->missed)
        {
            if (hypotf(targets[m->target_idx].x - m->x, targets[m->target_idx].y - m->y) < 100.0f)
            {
                m->active = targets[m->target_idx].active = false;
                spawn_target(m->target_idx);
            }
        }
    }
    glutPostRedisplay();
    if (!game_over)
        glutTimerFunc(1000 / FPS, update, 0);
}

/* =========================================================================
   7. INPUT (DIRECTIONAL LAUNCH)
   ========================================================================= */
void mouse(int button, int state, int mouse_x, int mouse_y)
{
    if (state != GLUT_DOWN || game_over)
        return;

    // Convert screen pixels to current internal coordinate system
    float world_x = ((float)mouse_x / current_win_w) * world_width;
    float world_y = ((float)(current_win_h - mouse_y) / current_win_h) * WORLD_HEIGHT;

    int best_target = -1;
    float min_dist = 10000.0f;
    for (int i = 0; i < MAX_BALLOONS; i++)
    {
        if (!targets[i].active)
            continue;
        float d = hypotf(targets[i].x - world_x, targets[i].y - world_y);
        if (d < min_dist && d < 350.0f)
        {
            min_dist = d;
            best_target = i;
        }
    }

    if (best_target != -1)
    {
        for (int i = 0; i < MAX_MISSILES; i++)
        {
            if (!missiles[i].active)
            {
                /* DIRECTIONAL LAUNCH VECTOR MATH:
                   1. Subtract Turret Pos from Click Pos to get Displacement Vector.
                   2. Hypotf provides the magnitude (distance).
                   3. (dx/mag, dy/mag) provides a UNIT VECTOR (length of 1.0).
                   4. Multiplying unit vector by speed scales the velocity correctly. */
                float dx = world_x - turret_x;
                float dy = world_y - turret_y;
                float mag = hypotf(dx, dy);
                if (mag < 1.0f)
                    mag = 1.0f;

                float launch_speed = 1600.0f;
                missiles[i].vx = (dx / mag) * launch_speed;
                missiles[i].vy = (dy / mag) * launch_speed;

                missiles[i].x = turret_x;
                missiles[i].y = turret_y;
                missiles[i].actual_lat_accel = 0.0f;
                missiles[i].fuel_timer = 2.0f;
                missiles[i].target_idx = best_target;
                missiles[i].missed = false;
                missiles[i].prev_dist = 9999.0f;
                missiles[i].guidance = (button == GLUT_LEFT_BUTTON) ? GUIDANCE_APNG : GUIDANCE_PURE_PURSUIT;
                missiles[i].active = true;
                break;
            }
        }
    }
}

/* =========================================================================
   8. RENDER & MAIN
   ========================================================================= */
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    if (game_over)
    {
        glColor3f(1.0f, 0.0f, 0.0f);
        char *go = "OVERWHELMED - GAME OVER!";
        glRasterPos2f(turret_x - 400, WORLD_HEIGHT / 2.0f);
        for (int i = 0; go[i] != '\0'; i++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, go[i]);
    }
    else
    {
        // Player Turret
        glColor3f(0.0f, 1.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(turret_x - 80, turret_y - 80);
        glVertex2f(turret_x + 80, turret_y - 80);
        glVertex2f(turret_x + 80, turret_y + 80);
        glVertex2f(turret_x - 80, turret_y + 80);
        glEnd();

        // Targets
        glColor3f(0.0f, 0.5f, 1.0f);
        for (int i = 0; i < MAX_BALLOONS; i++)
            if (targets[i].active)
            {
                glBegin(GL_POLYGON);
                for (int n = 0; n < 36; n++)
                {
                    float th = 2.0f * M_PI * n / 36.0f;
                    glVertex2f(targets[i].x + 74.0f * cosf(th), targets[i].y + 74.0f * sinf(th));
                }
                glEnd();
            }

        // Missiles
        for (int i = 0; i < MAX_MISSILES; i++)
            if (missiles[i].active)
            {
                if (missiles[i].missed)
                    glColor3f(1.0f, 0.0f, 0.0f);
                else if (missiles[i].guidance == GUIDANCE_APNG)
                    glColor3f(1.0f, 0.5f, 0.0f);
                else
                    glColor3f(0.8f, 0.0f, 1.0f);
                float h = atan2f(missiles[i].vy, missiles[i].vx);
                glPushMatrix();
                glTranslatef(missiles[i].x, missiles[i].y, 0.0f);
                glRotatef((h * 180.0f / M_PI) - 90.0f, 0, 0, 1);
                glBegin(GL_TRIANGLES);
                glVertex2f(-20, -20);
                glVertex2f(20, -20);
                glVertex2f(0, 60);
                glEnd();
                glPopMatrix();
            }

        char ui[64];
        sprintf(ui, "Integrity: %d", player_lives);
        glColor3f(1, 1, 1);
        glRasterPos2f(world_width * 0.05f, WORLD_HEIGHT * 0.9f);
        for (int i = 0; ui[i] != '\0'; i++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ui[i]);
    }
    glutSwapBuffers();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("APNG vs Pure Pursuit: Final Simulation");
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    init_sim();
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    return 0;
}