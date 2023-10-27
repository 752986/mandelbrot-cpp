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
#include <ctime>
#include <complex>
#include <vector>
#include <thread>
#include "colormap.cpp"
using namespace std;

int mandelbrot(complex<double> c, int max_iterations);
double smooth_mandelbrot(complex<double> c, int max_iterations);
void draw_mandelbrot(
	vector<vector<double>> &pixels,
	double startX, 
	double startY, 
	double endX, 
	double endY, 
	int screenX, 
	int screenY, 
	int screenWidth
);
void draw_pixels(vector<vector<double>> &pixels, int max_iterations);

const int WIDTH = 1000;
const int HEIGHT = 1000;

int main() {
	al_init();
	al_init_primitives_addon();
	auto display = al_create_display(WIDTH, HEIGHT);

	double zoom = WIDTH / 4;

	vector<vector<double>> pixels(WIDTH, vector<double>(HEIGHT, 0.0));

	auto start_time = clock();

	int num_threads = thread::hardware_concurrency();
	thread threads[num_threads];
	for (int i = 0; i < num_threads; i++) {
		int pixel_width = WIDTH / num_threads;
		double unit_width = 4 / (double)num_threads;
		threads[i] = thread(
			draw_mandelbrot,
			ref(pixels), // `ref()` is *very* important here
			-2 + (unit_width * i), 
			-2, 
			-2 + (unit_width * (i + 1)), 
			2, 
			pixel_width * i, 
			0, 
			pixel_width
		);
		// draw_mandelbrot(
		// 	pixels,
		// 	-2 + (unit_width * i), 
		// 	-2, 
		// 	-2 + (unit_width * (i + 1)), 
		// 	2, 
		// 	pixel_width * i, 
		// 	0, 
		// 	pixel_width
		// );
	}

	al_lock_bitmap(al_get_target_bitmap(), al_get_display_format(display), al_get_display_flags(display));

	for (int i = 0; i < num_threads; i++) {
		threads[i].join();
	}

	draw_pixels(pixels, 80);

	auto run_time = clock() - start_time;
	printf("%d ms\n", run_time);

	al_unlock_bitmap(al_get_target_bitmap());
	al_flip_display();

	system("pause");
	al_destroy_display(display);
}

void draw_mandelbrot(
	vector<vector<double>> &pixels,
	double startX, 
	double startY, 
	double endX, 
	double endY, 
	int screenX, 
	int screenY, 
	int screenWidth // screenHeight is inferred
) {
	complex<double> c;
	double num;

	int max_iterations = 80;

	double scale = screenWidth / (endX - startX);

	for (double x = startX; x < endX; x += 1 / scale) {
		for (double y = startY; y < endY; y += 1 / scale) {
			c = complex<double>(x, y);
			num = smooth_mandelbrot(c, max_iterations);
			int xCoord = x * scale + screenX - (scale * startX);
			int yCoord = y * scale + screenY - (scale * startY);
			pixels[xCoord][yCoord] = num; 
		}
	}
}

void draw_pixels(vector<vector<double>> &pixels, int max_iterations) {
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			double num = pixels[x][y];
			const float* color = colormap::sample_colormap(colormap::MAGMA, num / (float)max_iterations);
			al_put_pixel(x, y, al_map_rgb_f(color[0], color[1], color[2]));
		}
	}
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
