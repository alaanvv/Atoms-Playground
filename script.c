#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

// ---

#define SCREEN_SIZE 1000
#define SCALE 1
#define CANVAS_SIZE (SCREEN_SIZE / SCALE)

#define WALL_HIT_KEEP 0.5
#define ENERGY_KEEP 0.99999999
#define RAY 180
#define NUM 200
#define MAX 5
#define MIN -5

typedef enum { Red, Green, Blue, Black } Color;

typedef float ColorRule[3][3];

typedef struct {
  int x;
  int y;
  float vx;
  float vy;
  Color color;
} Atom;

// ---

SDL_Window* win;
SDL_Renderer* ren;

ColorRule rules;
Atom atoms[NUM * 3];
int atomsLen = 0;

// ---

void createAtoms(Color c);
void randomizeRules();
void initializeSDL();
void color(Color c);
void applyRules();
void render();
int rRule();
int rPos();

// ---

int main() {
  initializeSDL();

  srand(time(0));
  randomizeRules();

  createAtoms(Red);
  createAtoms(Green);
  createAtoms(Blue);

  SDL_Event e;
  int running = 1;
  while (running) {
    while(SDL_PollEvent(&e)) {
      if (e.type == SDL_KEYDOWN) { randomizeRules(); }
      else if (e.type == SDL_QUIT) { running = 0; }
    }

    applyRules();
    render();

    SDL_Delay(1);
  }

  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}

void initializeSDL() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(SCREEN_SIZE, SCREEN_SIZE, 0, &win, &ren);
  SDL_RenderSetScale(ren, SCALE, SCALE);

  color(Black);
  SDL_RenderClear(ren);

  SDL_RenderPresent(ren);
}

void color(Color c) {
  switch (c) {
    case Black:
      SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
      break;
    case Red:
      SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
      break;
    case Green:
      SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
      break;
    case Blue:
      SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
      break;
  }
}

inline int rRule() { return random() % (MAX - MIN) + MIN; }

void randomizeRules() {
  rules[Red][Red]     = rRule();
  rules[Red][Green]   = rRule();
  rules[Red][Blue]    = rRule();
  rules[Green][Red]   = rRule();
  rules[Green][Green] = rRule();
  rules[Green][Blue]  = rRule();
  rules[Blue][Red]    = rRule();
  rules[Blue][Green]  = rRule();
  rules[Blue][Blue]   = rRule();
}

inline int rPos() { return round(random() % CANVAS_SIZE); }

void createAtoms(Color c) {
  for (int i = 0; i < NUM; i++) {
    atoms[atomsLen].x = rPos();
    atoms[atomsLen].y = rPos();
    atoms[atomsLen].vx = 0;
    atoms[atomsLen].vy = 0;
    atoms[atomsLen].color = c;
    atomsLen++;
  }
}

void applyRules() {
  for (int a = 0; a < NUM * 3; a++) {
    for (int b = 0; b < NUM * 3; b++) {
      if (a == b) { continue; }

      int dx = atoms[a].x - atoms[b].x;
      int dy = atoms[a].y - atoms[b].y;
      int d = sqrt(dx * dx + dy * dy);

      if (d == 0 || d > RAY) continue;

      float F = rules[atoms[a].color][atoms[b].color] / 1000 / d;
      atoms[a].vx += F * dx;
      atoms[a].vy += F * dy;
    }

    atoms[a].vx *= ENERGY_KEEP;
    atoms[a].vy *= ENERGY_KEEP;

    atoms[a].x = atoms[a].x + atoms[a].vx;
    atoms[a].y = atoms[a].y + atoms[a].vy;

    if (atoms[a].x < 0) {
      atoms[a].x = -atoms[a].x;
      atoms[a].vx *= -WALL_HIT_KEEP;
    }
    else if (atoms[a].x > CANVAS_SIZE) {
      atoms[a].x = CANVAS_SIZE - (atoms[a].x - CANVAS_SIZE);
      atoms[a].vx *= -WALL_HIT_KEEP;
    }
    if (atoms[a].y < 0) {
      atoms[a].y = -atoms[a].y;
      atoms[a].vy *= -WALL_HIT_KEEP;
    }
    else if (atoms[a].y > CANVAS_SIZE) {
      atoms[a].y = CANVAS_SIZE - (atoms[a].y - CANVAS_SIZE);
      atoms[a].vy *= -WALL_HIT_KEEP;
    }
  }
}

void render() { 
  color(Black);
  SDL_RenderClear(ren);

  for (int i = 0; i < NUM * 3; i++) {
    color(atoms[i].color);
    SDL_RenderDrawPoint(ren, atoms[i].x, atoms[i].y);
  }

  SDL_RenderPresent(ren);
}
