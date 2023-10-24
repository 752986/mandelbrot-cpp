#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <complex>
#include "colormap.cpp"
using namespace std;

int mandelbrot(complex<double> c, int max_iterations);

const int WIDTH = 1000;
const int HEIGHT = 1000;

int main() {
	al_init();
	al_init_primitives_addon();
	auto display = al_create_display(WIDTH, HEIGHT);

	complex<double> c;
	int num;

	double zoom = WIDTH / 4;

	const int max_iterations = 80;

	al_lock_bitmap(al_get_target_bitmap(), al_get_display_format(display), al_get_display_flags(display));
	for (double x = -2; x < 2; x += 1 / zoom) {
		for (double y = -2; y < 2; y += 1 / zoom) {
			c = complex<double>(x, y);
			num = mandelbrot(c, max_iterations);
			const float* color = colormap::sample_colormap(colormap::MAGMA, (float)num / (float)max_iterations);
			al_put_pixel(x * zoom + (WIDTH / 2), y * zoom + (HEIGHT / 2), al_map_rgb_f(color[0], color[1], color[2]));
		}
	}

	al_unlock_bitmap(al_get_target_bitmap());
	al_flip_display();

	system("pause");
	al_destroy_display(display);
}

int mandelbrot(complex<double> c, int max_iterations) {
	complex<double> z = 0;

	for (int i = 0; i < max_iterations; i++) {
		z = (z * z) + c;
		if (abs(z) > 2) {
			return i;
		} 
	}
	return 0;
}