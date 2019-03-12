#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define WIN_SIZE 800
#define TEX_SIZE 800
#define NMax 50

#define R_COEFF 10
#define G_COEFF 10
#define B_COEFF 10

struct complex {
	float re;
	float im;
};

float sqr_modulus(struct complex const z)
{
	return (z.re * z.re + z.im * z.im);
}

struct complex comp_sum(struct complex const z1, struct complex const z2)
{
	struct complex s = {.re = z1.re + z2.re,
											.im = z1.im + z2.im};
	return s;
}

struct complex comp_sqr(struct complex const z)
{
	struct complex s = {.re = z.re * z.re - z.im * z.im,
											.im = 2 * z.re * z.im};
	return s;
}

struct complex z_nplus1(struct complex const z_n, struct complex const c)
{
	return comp_sum(comp_sqr(z_n), c);
}

struct result {
	float mod;
	int iter;
};

struct result check(struct complex const c)
{
	int n = 0;
	struct complex z_n = {.re = 0, .im = 0};
	float mod = 0;
	while ((n < NMax) && (mod <= 4))
	{
		z_n = z_nplus1(z_n, c);
		mod = sqr_modulus(z_n);
		n++;
	}
	struct result res = {.mod = mod, .iter = n};
	return res;
}

void draw_mandelbrot(SDL_Renderer * pRenderer,
										float x1, float y1,
										float x2, float y2)
{
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 0);
	SDL_RenderClear(pRenderer);
	SDL_RenderPresent(pRenderer);

// MAIN PROGRAM
float rcoeff = (x2 - x1) / WIN_SIZE;
float icoeff = (y2 - y1) / WIN_SIZE;
float roffset = x1;
float ioffset = y1;

	// float rcoeff = 2.7 / WIN_SIZE;
	// float icoeff = 2.4 / WIN_SIZE;
	// float roffset = -2.1;
	// float ioffset = -1.2;
	for (int x = 0; x < WIN_SIZE; x++)
	{
		for (int y = 0; y < WIN_SIZE; y++)
		{
			float real = ((float)x) * rcoeff + roffset;
			float img = ((float)y) * icoeff + ioffset;
			struct complex c = {.re = real, .im = img};
			struct result res = check(c);
			float mod = res.mod;
			int iter = res.iter;
			// printf("%f\n", mod);
			if (mod > 4)
			{
				// linear with iter
				int R = ((int)iter)*R_COEFF;
				int G = ((int)iter)*G_COEFF;
				int B = ((int)iter)*B_COEFF;
				// linear with mod
				// int R = ((int)mod)*R_COEFF;
				// int G = ((int)mod)*G_COEFF;
				// int B = ((int)mod)*B_COEFF;
				SDL_SetRenderDrawColor(pRenderer, R % 255, G % 255, B % 255, 255);
			}
			else
			{
				SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
			}
			SDL_RenderDrawPoint(pRenderer, x, y);
		}
	}
	printf("DONE\n");
	SDL_RenderPresent(pRenderer);
}

int main() {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* pWindow = NULL;
	pWindow = SDL_CreateWindow("Mandelbrot",
														SDL_WINDOWPOS_UNDEFINED,
													  SDL_WINDOWPOS_UNDEFINED,
							   						WIN_SIZE,
							   						WIN_SIZE,
							   						SDL_WINDOW_SHOWN);
	if (pWindow == NULL)
	{
		fprintf(stderr, "%s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Renderer * pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);
	if (pRenderer == NULL)
	{
		fprintf(stderr, "%s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	float x1 = -2.1;
	float y1 = -1.2;
	float x2 = 0.6;
	float y2 = 1.2;
	draw_mandelbrot(pRenderer, x1, y1, x2, y2);

// WAIT TILL QUIT
	SDL_Event event;
	int quit = 0;
	while (!quit)
	{
		SDL_WaitEvent(&event);
		switch (event.type)
		{
			int mouse_x = 0;
			int mouse_y = 0;
			case SDL_MOUSEBUTTONDOWN:
				SDL_GetMouseState(&mouse_x, &mouse_y);
				float rcoeff = (x2 - x1) / WIN_SIZE;
				float icoeff = (y2 - y1) / WIN_SIZE;
				float roffset = x1;
				float ioffset = y1;
				float real = ((float)mouse_x) * rcoeff + roffset;
				float img = ((float)mouse_y) * icoeff + ioffset;
				printf("OLD : %f %f %f %f\n", x1, y1, x2, y2);
				x1 = real - (x2 - x1) / 4;
				y1 = img - (y2 - y1) / 4;
				x2 = real + (x2 - x1) / 4;
				y2 = img + (y2 - y1) / 4;
				printf("NEW : %f %f %f %f\n", x1, y1, x2, y2);
				draw_mandelbrot(pRenderer, x1, y1, x2, y2);
				break;
			case SDL_QUIT:
				quit = 1;
				break;
		}
	}

	SDL_DestroyRenderer(pRenderer);
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
	return EXIT_SUCCESS;
}
