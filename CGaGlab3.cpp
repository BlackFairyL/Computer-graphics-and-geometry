#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>	
#include <vector>
#include <math.h>
#include <algorithm>
#include <ctime>
using namespace std;

const double EPS = 1e-5;

void gradient(vector<unsigned char>& pixel, int& h, int& w) {
	for (int i = 0; i < h * w; i++) {
		pixel[i] = (i % w) * 256 / w;
	}
}

unsigned char nearest_color(int color, int& bit, int& pal) {
	double intense = (double)color / (double)pal;
	return round(intense * ((1 << bit) - 1)) / ((1 << bit) - 1) * 255;
}

void no_dithering(vector<unsigned char>& pixel, int& h, int& w, int& bit, int& color) {
	for (int i = 0; i < h * w; i++) {
		pixel[i] = nearest_color(pixel[i], bit, color);
	}
}

int bayer_matrix[8][8] = {
	{0, 48, 12, 60, 3, 51, 15, 63},
	{32, 16, 44, 28, 35, 19, 47, 31},
	{8, 56, 4, 52, 11, 59, 7, 55},
	{40, 24, 36, 20, 43, 27, 39, 23},
	{2, 50, 14, 62, 1, 49, 13, 61},
	{34, 18, 46, 30, 33, 17, 45, 29},
	{10, 58, 6, 54, 9, 57, 5, 53},
	{42, 26, 38, 22, 41, 25, 37, 21},
};
void ordered_dithering(vector<unsigned char>& pixel, int& h, int& w, int& bit, int& color) {
	double bm, now;
	for (int i = 0; i < h * w; i++) {
		now = (double)pixel[i] / (double)color;
		bm = (double)bayer_matrix[(i / w) % 8][(i % w) % 8] / 64. - 0.5;
		now = (now + bm > 1 ? 1 : (now + bm < 0 ? 0 : now + bm));
		pixel[i] = nearest_color(now * color, bit, color);
	}
}

void random_dithering(vector<unsigned char>& pixel, int& h, int& w, int& bit, int& color) {
	double now, bm;
	for (int i = 0; i < h * w; i++) {
		now = (double)pixel[i] / (double)color;
		bm =  (double)rand() / (double)(RAND_MAX - 1) - 0.5;
		now = (now + bm > 1 ? 1 : (now + bm < 0 ? 0 : now + bm));
		pixel[i] = nearest_color(now * color, bit, color);
	}
}

void floyd_s_dithering(vector<unsigned char>& pixel, int& h, int& w, int& bit, int& color) {
	unsigned char new_c;
	double err;
	for (int i = 0; i < h * w; i++) {
		new_c = nearest_color(pixel[i], bit, color);
		err = (pixel[i] - new_c) / 16.;
		pixel[i] = new_c;
		if (i + 1 < h * w && (i + 1) % w != 0)
			pixel[i + 1] = (pixel[i + 1] + err * 7. > 255 ? 255 : (pixel[i + 1] + err * 7. < 0 ? 0 : pixel[i + 1] + err * 7.));
		if (i + w < h * w)
			pixel[i + w] = (pixel[i + w] + err * 5. > 255 ? 255 : (pixel[i + w] + err * 5. < 0 ? 0 : pixel[i + w] + err * 5.));
		if (i + w - 1 < h * w && i % w != 0)
			pixel[i + w - 1] = (pixel[i + w - 1] + err * 3. > 255 ? 255 : (pixel[i + w - 1] + err * 3. < 0 ? 0 : pixel[i + w - 1] + err * 3.));
		if (i + w + 1 < h * w && (i + 1) % w != 0)
			pixel[i + w + 1] = (pixel[i + w + 1] + err > 255 ? 255 : (pixel[i + w + 1] + err < 0 ? 0 : pixel[i + w + 1] + err));
	}
}

void jjn_dithering(vector<unsigned char>& pixel, int& h, int& w, int& bit, int& color) {
	unsigned char new_c;
	double err;
	for (int i = 0; i < h * w; i++) {
		new_c = nearest_color(pixel[i], bit, color);
		err = (pixel[i] - new_c) / 48.;
		pixel[i] = new_c;
		if (i + 1 < h * w && (i + 1) % w != 0)
			pixel[i + 1] = (pixel[i + 1] + err * 7. > 255 ? 255 : (pixel[i + 1] + err * 7. < 0 ? 0 : pixel[i + 1] + err * 7.));
		if (i + 2 < h * w && (i + 2) % w != 0)
			pixel[i + 2] = (pixel[i + 2] + err * 5. > 255 ? 255 : (pixel[i + 2] + err * 5. < 0 ? 0 : pixel[i + 2] + err * 5.));

		if (i + w < h * w)
			pixel[i + w] = (pixel[i + w] + err * 7. > 255 ? 255 : (pixel[i + w] + err * 7. < 0 ? 0 : pixel[i + w] + err * 7.));
		if (i + w - 1 < h * w && i % w != 0)
			pixel[i + w - 1] = (pixel[i + w - 1] + err * 5. > 255 ? 255 : (pixel[i + w - 1] + err * 5. < 0 ? 0 : pixel[i + w - 1] + err * 5.));
		if (i + w - 2 < h * w && i % w > 1)
			pixel[i + w - 2] = (pixel[i + w - 2] + err * 3. > 255 ? 255 : (pixel[i + w - 2] + err * 3. < 0 ? 0 : pixel[i + w - 2] + err * 3.));
		if (i + w + 1 < h * w && (i + 1) % w != 0)
			pixel[i + w + 1] = (pixel[i + w + 1] + err * 5. > 255 ? 255 : (pixel[i + w + 1] + err * 5. < 0 ? 0 : pixel[i + w + 1] + err * 5.));
		if (i + w + 2 < h * w && (i + 2) % w != 0)
			pixel[i + w + 2] = (pixel[i + w + 2] + err * 3. > 255 ? 255 : (pixel[i + w + 2] + err * 3. < 0 ? 0 : pixel[i + w + 2] + err * 3.));

		if (i + 2 * w < h * w)
			pixel[i + 2 * w] = (pixel[i + 2 * w] + err * 5. > 255 ? 255 : (pixel[i + 2 * w] + err * 5. < 0 ? 0 : pixel[i + 2 * w] + err * 5.));
		if (i + 2 * w - 1 < h * w && i % w != 0)
			pixel[i + 2 * w - 1] = (pixel[i + 2 * w - 1] + err * 3. > 255 ? 255 : (pixel[i + 2 * w - 1] + err * 3. < 0 ? 0 : pixel[i + 2 * w - 1] + err * 3.));
		if (i + 2 * w - 2 < h * w && i % w > 1)
			pixel[i + 2 * w - 2] = (pixel[i + 2 * w - 2] + err * 1. > 255 ? 255 : (pixel[i + 2 * w - 2] + err * 1. < 0 ? 0 : pixel[i + 2 * w - 2] + err * 1.));
		if (i + 2 * w + 1 < h * w && (i + 1) % w != 0)
			pixel[i + 2 * w + 1] = (pixel[i + 2 * w + 1] + err * 3. > 255 ? 255 : (pixel[i + 2 * w + 1] + err * 3. < 0 ? 0 : pixel[i + 2 * w + 1] + err * 3.));
		if (i + 2 * w + 2 < h * w && (i + 2) % w != 0)
			pixel[i + 2 * w + 2] = (pixel[i + 2 * w + 2] + err * 1. > 255 ? 255 : (pixel[i + 2 * w + 2] + err * 1. < 0 ? 0 : pixel[i + 2 * w + 2] + err * 1.));
	}
}


void sierra_dithering(vector<unsigned char>& pixel, int& h, int& w, int& bit, int& color) {
	unsigned char new_c;
	double err;
	for (int i = 0; i < h * w; i++) {
		new_c = nearest_color(pixel[i], bit, color);
		err = (pixel[i] - new_c) / 32.;
		pixel[i] = new_c;
		if (i + 1 < h * w && (i + 1) % w != 0)
			pixel[i + 1] = (pixel[i + 1] + err * 5. > 255 ? 255 : (pixel[i + 1] + err * 5. < 0 ? 0 : pixel[i + 1] + err * 5.));
		if (i + 2 < h * w && (i + 2) % w != 0)
			pixel[i + 2] = (pixel[i + 2] + err * 3. > 255 ? 255 : (pixel[i + 2] + err * 3. < 0 ? 0 : pixel[i + 2] + err * 3.));

		if (i + w < h * w)
			pixel[i + w] = (pixel[i + w] + err * 5. > 255 ? 255 : (pixel[i + w] + err * 5. < 0 ? 0 : pixel[i + w] + err * 5.));
		if (i + w - 1 < h * w && i % w != 0)
			pixel[i + w - 1] = (pixel[i + w - 1] + err * 4. > 255 ? 255 : (pixel[i + w - 1] + err * 4. < 0 ? 0 : pixel[i + w - 1] + err * 4.));
		if (i + w - 2 < h * w && i % w > 1)
			pixel[i + w - 2] = (pixel[i + w - 2] + err * 2. > 255 ? 255 : (pixel[i + w - 2] + err * 2. < 0 ? 0 : pixel[i + w - 2] + err * 2.));
		if (i + w + 1 < h * w && (i + 1) % w != 0)
			pixel[i + w + 1] = (pixel[i + w + 1] + err * 4. > 255 ? 255 : (pixel[i + w + 1] + err * 4. < 0 ? 0 : pixel[i + w + 1] + err * 4.));
		if (i + w + 2 < h * w && (i + 2) % w != 0)
			pixel[i + w + 2] = (pixel[i + w + 2] + err * 2. > 255 ? 255 : (pixel[i + w + 2] + err * 2. < 0 ? 0 : pixel[i + w + 2] + err * 2.));

		if (i + 2 * w < h * w)
			pixel[i + 2 * w] = (pixel[i + 2 * w] + err * 3. > 255 ? 255 : (pixel[i + 2 * w] + err * 3. < 0 ? 0 : pixel[i + 2 * w] + err * 3.));
		if (i + 2 * w - 1 < h * w && i % w != 0)
			pixel[i + 2 * w - 1] = (pixel[i + 2 * w - 1] + err * 2. > 255 ? 255 : (pixel[i + 2 * w - 1] + err * 2. < 0 ? 0 : pixel[i + 2 * w - 1] + err * 2.));
		if (i + 2 * w + 1 < h * w && (i + 1) % w != 0)
			pixel[i + 2 * w + 1] = (pixel[i + 2 * w + 1] + err * 2. > 255 ? 255 : (pixel[i + 2 * w + 1] + err * 2. < 0 ? 0 : pixel[i + 2 * w + 1] + err * 2.));
	}
}

void atkinson_dithering(vector<unsigned char>& pixel, int& h, int& w, int& bit, int& color) {
	unsigned char new_c;
	double err;
	for (int i = 0; i < h * w; i++) {
		new_c = nearest_color(pixel[i], bit, color);
		err = (pixel[i] - new_c) / 8.;
		pixel[i] = new_c;
		if (i + 1 < h * w && (i + 1) % w != 0)
			pixel[i + 1] = (pixel[i + 1] + err * 1. > 255 ? 255 : (pixel[i + 1] + err * 1. < 0 ? 0 : pixel[i + 1] + err * 1.));
		if (i + 2 < h * w && (i + 2) % w != 0)
			pixel[i + 2] = (pixel[i + 2] + err * 1. > 255 ? 255 : (pixel[i + 2] + err * 1. < 0 ? 0 : pixel[i + 2] + err * 1.));

		if (i + w < h * w)
			pixel[i + w] = (pixel[i + w] + err * 1. > 255 ? 255 : (pixel[i + w] + err * 1. < 0 ? 0 : pixel[i + w] + err * 1.));
		if (i + w - 1 < h * w && i % w != 0)
			pixel[i + w - 1] = (pixel[i + w - 1] + err * 1. > 255 ? 255 : (pixel[i + w - 1] + err * 1. < 0 ? 0 : pixel[i + w - 1] + err * 1.));
		if (i + w + 1 < h * w && (i + 1) % w != 0)
			pixel[i + w + 1] = (pixel[i + w + 1] + err * 1. > 255 ? 255 : (pixel[i + w + 1] + err * 1. < 0 ? 0 : pixel[i + w + 1] + err * 1.));

		if (i + 2 * w < h * w)
			pixel[i + 2 * w] = (pixel[i + 2 * w] + err * 1. > 255 ? 255 : (pixel[i + 2 * w] + err * 1. < 0 ? 0 : pixel[i + 2 * w] + err * 1.));
	}
}

double halftone_matrix[4][4] = {
		{0.375,  0.75,   0.625,  0.1875},
		{0.6875, 0.9375, 0.8125, 0.4375},
		{0.5625, 0.875,  0.3125, 0.0625},
		{0.25,   0.5,    0.125,  0.0}
};
void halftone(vector<unsigned char>& pixel, int& h, int& w, int& bit, int& color) {
	double bm, now;
	for (int i = 0; i < h * w; i++) {
		now = (double)pixel[i] / (double)color;
		bm = halftone_matrix[(i / w) % 4][(i % w) % 4] - 0.5;
		now = (now + bm > 1 ? 1 : (now + bm < 0 ? 0 : now + bm));
		pixel[i] = nearest_color(now * color, bit, color);
	}
}

int main(int argc, char* argv[]) {
	srand(time(0));
	if (argc != 7) {
		cerr << "Invalid request";
		return 1;
	}
	string name_in = argv[1];
	string name_out = argv[2];
	int grad = atoi(argv[3]),
		diz_type = atoi(argv[4]),
		bits = atoi(argv[5]);
	double gamma = atof(argv[6]) == 0 ? 2 : atof(argv[6]);
	//kgig.exe <имя_входного_файла> <имя_выходного_файла> <градиент> <дизеринг> <битность> <гамма>

	FILE* fin = fopen(name_in.c_str(), "rb");
	if (!fin) {
		cerr << "Invalid input file";
		return 1;
	}

	char p, number;
	int h, w, color;
	int count = fscanf(fin, "%c%c\n%d %d\n%d\n", &p, &number, &w, &h, &color);
	if (count != 5) {
		cerr << "Incorrect file content";
		fclose(fin);
		return 1;
	}

	if (!(p == 'P' && (number == '5' || number == '6')) || h <= 0 || w <= 0 || color > 255 || color <= 0) {
		cerr << "Incorrect data in the file";
		fclose(fin);
		return 1;
	}
	bool flag = 0;
	vector<unsigned char> pixel;
	if (number == '5') {
		pixel.resize(h * w, 0);
	}
	else if (number == '6') {
		cerr << "Incorrect File type";
		fclose(fin);
		return 1;
	}

	int cnt = fread(&pixel[0], sizeof(unsigned char), pixel.size(), fin);
	if (number == '5' && cnt != h * w) {
		cerr << "Incorrect data in the file";
		fclose(fin);
		return 1;
	}

	if (!(0 <= grad && grad <= 1 &&
		0 <= diz_type && diz_type < 8 &&
		1 <= bits && bits <= 8 &&
		0 < gamma)) {
		cerr << "Incorrect arguments in console";
		fclose(fin);
		return 1;
	}

	fclose(fin);

		for (int i = 0; i < h * w; i++) {
		double now = (double)pixel[i] / (double)color;
		if (gamma != 0)
			pixel[i] = pow(now, gamma) * (double)color;
		else
			if (now <= 0.0031308) {
				pixel[i] = (323.0 * now / 25.0) * (double)color;
			}
			else {
				pixel[i] = ((211 * pow(now, 5.0 / 12.0) - 11) / 200.0) * (double)color;
			}
	}

	if (grad)
		gradient(pixel, h, w);
	
	if (diz_type == 0) {
		no_dithering(pixel, h, w, bits, color);
	}
	if (diz_type == 1) {
		ordered_dithering(pixel, h, w, bits, color);
	}
	if (diz_type == 2) {
		random_dithering(pixel, h, w, bits, color);
	}
	if (diz_type == 3) {
		floyd_s_dithering(pixel, h, w, bits, color);
	}
	if (diz_type == 4) {
		jjn_dithering(pixel, h, w, bits, color);
	}
	if (diz_type == 5) {
		sierra_dithering(pixel, h, w, bits, color);
	}
	if (diz_type == 6) {
		atkinson_dithering(pixel, h, w, bits, color);
	}
	if (diz_type == 7) {
		halftone(pixel, h, w, bits, color);
	}

	for (int i = 0; i < h * w; i++) {
		double now = (double)pixel[i] / (double)color;
		if (gamma != 0)
			pixel[i] = pow(now, 1. / gamma) * (double)color;
		else
			if (now <= 0.04045) {
				pixel[i] = (25.0 * now / 323) * (double)color;
			}
			else {
				pixel[i] = (pow((200 * now + 11) / 211.0, 12.0 / 5.0) > 255 ? 255 : pow((200 * now + 11) / 211.0, 12.0 / 5.0)) * (double)color;
			}
	}

	FILE* fout = fopen(name_out.c_str(), "wb");

	if (!fout) {
		cerr << "Invalid output file";
		return 1;
	}

	fprintf(fout, "%c%c\n%d %d\n%d\n", p, number, w, h, color);
	fwrite(&pixel[0], sizeof(unsigned char), pixel.size(), fout);
	fclose(fout);
	return 0;
}
