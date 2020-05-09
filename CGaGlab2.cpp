#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>	
#include <vector>
#include <math.h>
#include <algorithm>
using namespace std;

const double EPS = 1e-5;

void to_black(vector<unsigned char>& pixel) {
	pixel.assign(pixel.size(), 0);
}

void draw_point(vector<unsigned char>& pixel, int& x, int& y, int& mx, int& my, double bright, double normal, double& gamma, char& type) {
	if (type == '5') {
		double newGamma = pow(bright / 255.0, gamma);
		double pixelGamma = pow(pixel[x * my + y] / 255.0, gamma);
		pixel[x * my + y] = pow((1 - normal) * pixelGamma + normal * newGamma, 1 / gamma) * 255;
	}
	else {
		double newGamma = pow(bright / 255.0, gamma);
		double pixelGamma = pow(pixel[x * my * 3 + y * 3] / 255.0, gamma);
		double pixelGamma1 = pow(pixel[x * my * 3 + y * 3 + 1] / 255.0, gamma);
		double pixelGamma2 = pow(pixel[x * my * 3 + y * 3 + 2] / 255.0, gamma);
		pixel[x * my * 3 + y * 3] = pow((1 - normal) * pixelGamma + normal * newGamma, 1 / gamma) * 255;
		pixel[x * my * 3 + y * 3 + 1] = pow((1 - normal) * pixelGamma1 + normal * newGamma, 1 / gamma) * 255;
		pixel[x * my * 3 + y * 3 + 2] = pow((1 - normal) * pixelGamma2 + normal * newGamma, 1 / gamma) * 255;
	}
}

double dist(double& x1, double& y1, double& x2, double& y2) {
	double dx = x1 - x2, dy = y1 - y2;
	return sqrt(dx * dx + dy * dy);
}

double min_dist(double& xs, double& ys, double& xf, double& yf, double findx, double findy, double thick) {
	double to_st = dist(xs, ys, findx, findy), to_fi = dist(xf, yf, findx, findy), len = dist(xs, ys, xf, yf), temp = 0.0;
	double dx = (xf - xs) / len, dy = (yf - ys) / len;
	double xns = xs - dx, yns = ys - dy, xnf = xf + dx, ynf = yf + dy;
	double to_ns = dist(xns, yns, findx, findy), to_nf = dist(xnf, ynf, findx, findy), len_new = dist(xns, yns, xnf, ynf);
	double A = yf - ys, B = xf - xs, C = ys * xf - yf * xs, H;
	H = fabs(A * findx - B * findy + C) / sqrt(A * A + B * B);
	if (to_st * to_st > to_fi * to_fi + len * len || to_fi * to_fi > to_st * to_st + len * len)
		if (to_ns * to_ns > to_nf * to_nf + len_new * len_new || to_nf * to_nf > to_ns * to_ns + len_new * len_new || H > thick + 1.0 - EPS)
			H = (thick + 1) * 2;
		else {
			double procx, procy; // проекция точки на прямую для сглаживания
			double ay = -dx * H, ax = dy * H;
			if (A * (findx + ax) - B * (findy + ay) + C < EPS)
				procx = findx + ax, procy = findy + ay;
			else
				procx = findx - ax, procy = findy - ay;
			H = min(dist(procx, procy, xs, ys), dist(procx, procy, xf, yf)) + thick - EPS;
		}
	return H;
}

double normal(double dist, double& thick) {
	if (dist >= thick + 1.0) // расстояние до точки больше толщины линии и погрешности -> не красим
		return 0.0;
	if (dist <= thick) // точка есть часть прямой, яркость как есть
		return 1.0;
	return 1.0 - dist + thick; // высчитываем погрешность, чем выше погрешность - тем тусклее точка
}

void draw_line(vector<unsigned char>& pixel, double xs, double ys, double xf, double yf, int& mx, int& my, int& bright, double& thick, double& gamma, char& type) {
	if (xs > xf) {
		swap(xs, xf);
		swap(ys, yf);
	}
	double thick_cor = thick * 2 + 2.0 - EPS;
	// direct line
	if (xs == xf || ys == yf)
		for (int x = max(0, int(xs - thick_cor)); x <= min(mx - 1, int(xf + thick_cor)); x++)
			for (int y = max(0, int(ys - thick_cor)); y <= min(my - 1, int(yf + thick_cor)); y++)
				draw_point(pixel, x, y, mx, my, bright, normal(min_dist(xs, ys, xf, yf, x, y, thick), thick), gamma, type);
	else {
		//алгоритм не Ву
		double grad = (yf - ys) / (xf - xs), ynow = ys;
		for (int x = max(0, int(xs - thick_cor)); x <= min(mx - 1, int(xf + thick_cor)); x++) {
			for (int y = max(0, int(ynow - fabs(grad) - thick_cor)); y <= min(my - 1, int(ynow + fabs(grad) + thick_cor)); y++)
				draw_point(pixel, x, y, mx, my, bright, normal(min_dist(xs, ys, xf, yf, x, y, thick), thick), gamma, type);
			ynow += (x >= xs && x <= xf ? grad : 0);
		}
	}
}

int main(int argc, char* argv[]) {

	if (argc != 10 && argc != 9) {
		cerr << "Invalid request";
		return 1;
	}
	string name_in = argv[1];
	string name_out = argv[2];
	int bright = atoi(argv[3]);
	double	thick = (atof(argv[4]) - 1) / 2.0,	// корректируем толщину
		xs = atof(argv[5]),
		ys = atof(argv[6]),
		xf = atof(argv[7]),
		yf = atof(argv[8]),
		gamma = (argc == 10 ? atof(argv[9]) : 2);
	//kgig.exe <имя_входного_файла> <имя_выходного_файла> <яркость_линии> <толщина_линии> <x_начальный> <y_начальный> <x_конечный> <y_конечный> <гамма>
	swap(xs, ys);
	swap(xf, yf);
	FILE* fin = fopen(name_in.c_str(), "rb");
	if (!fin) {
		cerr << "Invalid input file";
		fclose(fin);
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
		pixel.resize(3 * h * w);
		flag = 1;
	}

	int cnt = fread(&pixel[0], sizeof(unsigned char), pixel.size(), fin);
	if (number == '5' && cnt != h * w || number == '6' && cnt != 3 * h * w) {
		cerr << "Incorrect data in the file";
		fclose(fin);
		return 1;
	}

	if (!(0 <= xs && xs < h &&
		0 <= xf && xf < h &&
		0 <= ys && ys < w &&
		0 <= yf && yf < w &&
		0 <= bright && bright <= 255 &&
		-0.5 < thick &&
		0 < gamma)) {
		cerr << "Incorrect arguments in console";
		fclose(fin);
		return 1;
	}

	fclose(fin);
	//to_black(pixel);
	draw_line(pixel, xs, ys, xf, yf, h, w, bright, thick, gamma, number);

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
