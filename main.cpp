// TODO:
// - multithreading
//   > extract drawing to function
// - interactivity
//   > ability to close the window
//   > make it easy to specify a center and zoom
// - progressive refinement (shrinkwrap)
// - timing
// - colormap normalization
//   > maybe lerp colormap colors

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <complex>
#include "colormap.cpp"
using namespace std;

int mandelbrot(complex<double> c, int max_iterations);
double smooth_mandelbrot(complex<double> c, int max_iterations);

const int WIDTH = 1000;
const int HEIGHT = 1000;

int main() {
	al_init();
	al_init_primitives_addon();
	auto display = al_create_display(WIDTH, HEIGHT);

	complex<double> c;
	double num;

	double zoom = WIDTH / 4;

	const int max_iterations = 80;

	al_lock_bitmap(al_get_target_bitmap(), al_get_display_format(display), al_get_display_flags(display));
	for (double x = -2; x < 2; x += 1 / zoom) {
		for (double y = -2; y < 2; y += 1 / zoom) {
			c = complex<double>(x, y);
			num = smooth_mandelbrot(c, max_iterations);
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
		// a higher escape distance makes the output smoother
		if (abs(z) > 20) {
			return i;
		} 
	}
	return 0;
}

double smooth_mandelbrot(complex<double> c, int max_iterations) {
	complex<double> z = 0;

	for (int i = 0; i < max_iterations; i++) {
		z = (z * z) + c;
		// a higher escape distance makes the output smoother
		if (abs(z) > 20) {
			// I don't fully understand the smoothing algorithm, but basically:
			// 1. it is based on `abs(z)` because you are asking how far the point
			//    got when it escaped
			// 2. the `log(log(...))` part undoes some nonlinearities in the way that distance
			//    function behaves
			return i - log(log(abs(z))) / log(2);
		} 
	}
	return 0;
}
