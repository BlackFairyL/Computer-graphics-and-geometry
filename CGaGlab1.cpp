﻿#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>	
#include <vector>
using namespace std;

void inversion(vector<unsigned char>& pixel, int &color) {
	for (int i = 0; i < pixel.size(); i++) {
		pixel[i] = (unsigned char)(color - int(pixel[i]));
	}
}

void vertical_display(vector<unsigned char>& pixel, int h, int w, bool flag) {
	if (flag) {
		w = w * 3;
	}
	for (int i = 0; i < h / 2; i++) {
		for (int j = 0; j < w; j++) {
			swap(pixel[i * w + j], pixel[(h * w - w) - i * w + j]);
		}
	}
}

void horizontal_display(vector<unsigned char>& pixel, int h, int w, bool flag) {
	if (flag) {
		w = w * 3;
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w / 2; j += 3) {
				swap(pixel[i * w + j], pixel[i * w + w - j - 1 - 2]);
				swap(pixel[i * w + j + 1], pixel[i * w + w - 1 - j - 1]);
				swap(pixel[i * w + j + 2], pixel[i * w + w - 1 - j]);
			}
		}
	
	}
	else {
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w / 2; j++) {
				swap(pixel[i * w + j], pixel[i * w + w - 1 - j]);
			}
		}
	}
}

void change_pixel(vector<unsigned char>& pixel, int h, int w, bool flag) {
	vector<unsigned char>new_pixel;
	if (!flag) {
		new_pixel.resize(h * w);
		for (int j = 0; j < w; j++) {
			for (int i = 0; i < h; i++) {
				new_pixel[j * h + i] = pixel[i * w + j];
			}
		}
	}
	else {
		w = w * 3;
		int cnt = 0;
		new_pixel.resize(h * w);
		for (int j = 0; j < w; j += 3) {
			for (int i = 0; i < h; i++) {
				new_pixel[cnt] = pixel[i * w + j];
				new_pixel[cnt + 1] = pixel[i * w + j + 1];
				new_pixel[cnt + 2] = pixel[i * w + j + 2];
				cnt += 3;
			}
		}
		w = w / 3;
	}
	pixel = new_pixel;
}

void clockwise_rotation(vector<unsigned char>& pixel, int h, int w, bool flag) {
	change_pixel(pixel, h, w, flag);
	swap(h, w);
	horizontal_display(pixel, h, w, flag);
}

void counterclockwise_rotation(vector<unsigned char>& pixel, int h, int w, bool flag) {
	change_pixel(pixel, h, w, flag);
	swap(h, w);
	vertical_display(pixel, h, w, flag);
}
 
int main(int argc, char *argv[]) {
	
	if (argc != 4) {
		cout << "Invalid request";
		return 0;
	}
	string name_in = argv[1];
	string name_out = argv[2];
	int transform = atoi(argv[3]);

	FILE * fin = fopen(name_in.c_str(), "rb");
	if (!fin) {
		cout << "Invalid input file";
		return 0;
	}
	
	if (!(0 <= transform && transform <= 4)) {
		cout << "Invalid conversion";
		return 0;
	}

	char p, number;
	int h, w, color;
	int count = fscanf(fin, "%c%c\n%d %d\n%d\n", &p, &number, &w, &h, &color);
	if (count != 5) {
		cout << "Incorrect file content";
		return 0;
	}

	if (!(p == 'P' && (number == '5' || number == '6')) || h <= 0 || w <= 0 || color > 255 || color <= 0 ) {
		cout << "Incorrect data in the file";
		return 0;
	}
	bool flag = 0;
	vector<unsigned char> pixel;
	if (number == '5') {
		pixel.resize(h * w, 0);
	}
	else if (number == '6') {
		pixel.resize(3 * h * w);
		flag = 1;
	}

	int cnt = fread(&pixel[0], sizeof(unsigned char), pixel.size(), fin);
	if (number == '5' && cnt != h * w || number == '6' && cnt != 3 * h * w) {
		cout << "Incorrect data in the file";
		return 0;
	}

	fclose(fin);

	if (transform == 0)
		inversion(pixel, color);
	if (transform == 1)
		horizontal_display(pixel, h, w, flag);
	if (transform == 2)
		vertical_display(pixel, h, w, flag);
	if (transform == 3) {
		clockwise_rotation(pixel, h, w, flag);
		swap(h, w);
	}
	if (transform == 4) {
		counterclockwise_rotation(pixel, h, w, flag);
		swap(h, w);
	}
	FILE* fout = fopen(name_out.c_str(), "wb");
	
	if (!fout) {
		cout << "Invalid output file";
		return 0;
	}

	fprintf(fout, "%c%c\n%d %d\n%d\n", p, number, w, h, color);
	fwrite(&pixel[0], sizeof(unsigned char), pixel.size(), fout);
	fclose(fout);

}
