#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>	
#include <vector>
#include <math.h>
#include <algorithm>

using namespace std;

class pix {
public:
	unsigned char f, s, t;

};

class image {
public:
	char p, number;
	int h, w, color;
	vector<pix> pixel;

	image() {}
	image(const char _p, const char _number, const int _h, const int _w, const int _color) {
		p = _p;
		number = _number;
		h = _h;
		w = _w;
		color = _color;
	}
	int scan(FILE* &fin) {
		return fscanf(fin, "%c%c\n%d %d\n%d\n", &p, &number, &w, &h, &color);
	}
	void print_header(FILE* &fout) {
		fprintf(fout, "%c%c\n%d %d\n%d\n", p, number, w, h, color);
	}
	void print_pixel(FILE*& fout) {
		fwrite(&pixel[0], sizeof(pix), pixel.size(), fout);
	}
};

bool check_wrong(FILE* &fin, image &im, char number, int &count) {
	if (count != 5) {
		cerr << "Incorrect file content";
		fclose(fin);
		return 1;
	}

	if (!(im.p == 'P' && im.number == number) || im.h <= 0 || im.w <= 0 || im.color > 255 || im.color <= 0) {
		cerr << "Incorrect data in the file";
		fclose(fin);
		return 1;
	}
	return 0;
}

//void from_RGB(image& im) {
//}
//
//void to_RGB(image& im) {
//}

void from_CMY (image& im) {
	for (int i = 0; i < im.pixel.size(); i++) {
		im.pixel[i].f = 255 - im.pixel[i].f;
		im.pixel[i].s = 255 - im.pixel[i].s;
		im.pixel[i].t = 255 - im.pixel[i].t;
	}
}

void to_CMY(image& im) {
	for (int i = 0; i < im.pixel.size(); i++) {
		im.pixel[i].f = 255 - im.pixel[i].f;
		im.pixel[i].s = 255 - im.pixel[i].s;
		im.pixel[i].t = 255 - im.pixel[i].t;
	}
}

void from_HS_RGB(double& h, double& c, double& x, double& r, double& g, double& b) {
	if (h < 60) {
		r = c;
		g = x;
	}
	else if (h < 120) {
		r = x;
		g = c;
	}
	else if (h < 180) {
		g = c;
		b = x;
	}
	else if (h < 240) {
		g = x;
		b = c;
	}
	else if (h < 300) {
		r = x;
		b = c;
	}
	else if (h < 360) {
		r = c;
		b = x;
	}
}

void from_HSL(image& im) {
	for (int i = 0; i < im.pixel.size(); i++) {
		double h = im.pixel[i].f / 255. * 360, s = im.pixel[i].s * 1. / 255, l = im.pixel[i].t * 1. / 255;
		double c = (1 - fabs(2 * l - 1)) * s;
		double x = c * (1 - fabs(((h / 60) - (int)(h / 60)) + (int)(h / 60) % 2 - 1));
		double m = l - c / 2;
		double r = 0, g = 0, b = 0;
		from_HS_RGB(h, c, x, r, g, b);
		im.pixel[i].f = round((r + m) * 255);
		im.pixel[i].s = round((g + m) * 255);
		im.pixel[i].t = round((b + m) * 255);
	}

}

void to_HSL(image& im) {
	for (int i = 0; i < im.pixel.size(); i++) {
		double r = im.pixel[i].f * 1. / 255., g = im.pixel[i].s * 1. / 255., b = im.pixel[i].t * 1. / 255.;
		double cmax = max({ r, g, b }), cmin = min({ r, g, b });
		double d = cmax - cmin;

		double h = 0;
		if (d == 0)
			h = 0;
		else if (cmax == r) {
			double k = (g - b) / d;
			k = k > 0 ? k : 360 + k;
			h = 60 * (k - int(k) + int(k) % 6);
		}
		else if (cmax == g) {
			h = 60 * ((b - r) / d + 2);
		}
		else if (cmax == b) {
			h = 60 * ((r - g) / d + 4);
		}
		
		double l = (cmax + cmin) / 2.;
		double s = 0;

		s = l != 0 ? d / (1 - fabs(2 * l - 1)) : 0;

		im.pixel[i].f = round(h / 360. * 255.);
		im.pixel[i].s = round(s * 255);
		im.pixel[i].t = round(l * 255);
	}
}

void from_HSV(image& im) {
	for (int i = 0; i < im.pixel.size(); i++) {
		double h = im.pixel[i].f / 255. * 360, s = im.pixel[i].s * 1. / 100, v = im.pixel[i].t * 1. / 100;
		double c = s * v;
		double x = c * (1 - fabs(((h / 60) - (int)(h / 60)) + (int)(h / 60) % 2 - 1));
		double m = v - c;
		double r = 0, g = 0, b = 0;
		from_HS_RGB(h, c, x, r, g, b);
		im.pixel[i].f = round((r + m) * 255);
		im.pixel[i].s = round((g + m) * 255);
		im.pixel[i].t = round((b + m) * 255);
	}

}

void to_HSV(image& im) {
	for (int i = 0; i < im.pixel.size(); i++) {
		double r = im.pixel[i].f * 1. / 255, g = im.pixel[i].s * 1. / 255, b = im.pixel[i].t * 1. / 255;
		double cmax = max({ r, g, b }), cmin = min({ r, g, b });
		double d = cmax - cmin;

		double h = 0;
		if (d == 0)
			h = 0;
		else if (cmax == r) {
			double k = (g - b) / d;
			k = k > 0 ? k : 360 + k;
			h = 60 * (k - int(k) + int(k) % 6);
		}
		else if (cmax == g) {
			h = 60 * ((b - r) / d + 2);
		}
		else if (cmax == b) {
			h = 60 * ((r - g) / d + 4);
		}

		double l = (cmax + cmin) / 2;
		double s = 0;

		s = cmax != 0 ? d / (cmax) : 0;

		im.pixel[i].f = round(h / 360. * 255.);
		im.pixel[i].s = round(s * 100);
		im.pixel[i].t = round(cmax * 100);
	}
}

void to_ycbcr_form(image& im, int &i, double &Kr, double &Kg, double &Kb) {
	double r = im.pixel[i].f * 1. / 255, g = im.pixel[i].s * 1. / 255, b = im.pixel[i].t * 1. / 255;
	double y = (Kr * r + Kg * g + Kb * b);
	double cb = (1. / 2. * (b - y) / (1. - Kb));
	double cr = (1. / 2. * (r - y) / (1. - Kr));
	y = y * 255 > 255 ? 255 : (y * 255 < 0 ? 0 : y * 255);
	//cout << cb << " " << cr << endl;
	im.pixel[i].f = round(y);
	im.pixel[i].s = round((cb + 0.5) * 255);
	im.pixel[i].t = round((cr + 0.5) * 255);
}

void from_ycbcr_form(image& im, int &i, double& Kr, double& Kg, double& Kb) {
	double y = im.pixel[i].f * 1.  / 255, cb = (im.pixel[i].s * 1. / 255) - 0.5, cr = (im.pixel[i].t * 1. / 255) - 0.5;
	double r = y + (2. - 2 * Kr) * cr;
	double g = y - (Kb / Kg) * (2. - 2 * Kb) * cb - (Kr / Kg) * (2. - 2 * Kr) * cr;
	double b = y + (2. - 2 * Kb) * cb;
	r = r * 255 > 255 ? 255 : (r * 255 < 0 ? 0 : r * 255);
	g = g * 255 > 255 ? 255 : (g * 255 < 0 ? 0 : g * 255);
	b = b * 255 > 255 ? 255 : (b * 255 < 0 ? 0 : b * 255);
	im.pixel[i].f = round(r);
	im.pixel[i].s = round(g);
	im.pixel[i].t = round(b);
}


void to_ycbcr_601(image& im) {
	double Kr = 0.299, Kg = 0.587, Kb = 0.114;
	for (int i = 0; i < im.pixel.size(); i++) {
		to_ycbcr_form(im, i, Kr, Kg, Kb);
	}
}

void from_ycbcr_601(image& im) {
	double Kr = 0.299, Kg = 0.587, Kb = 0.114;
	for (int i = 0; i < im.pixel.size(); i++) {
		from_ycbcr_form(im, i, Kr, Kg, Kb);
	}
}

void from_ycbcr_709(image& im) {
	double Kr = 0.2126, Kg = 0.7152, Kb = 0.0722;
	for (int i = 0; i < im.pixel.size(); i++) {
		from_ycbcr_form(im, i, Kr, Kg, Kb);
	}
}

void to_ycbcr_709(image& im) {
	double Kr = 0.2126, Kg = 0.7152, Kb = 0.0722;
	for (int i = 0; i < im.pixel.size(); i++) {
		to_ycbcr_form(im, i, Kr, Kg, Kb);
	}
}

void to_YCoCg(image& im) {
	for (int i = 0; i < im.pixel.size(); i++) {
		double r = im.pixel[i].f * 1. / 255, g = im.pixel[i].s * 1. / 255, b = im.pixel[i].t * 1. / 255;
		double y = 0.25 * r + 0.5 * g + 0.25 * b;
		double co = 0.5 * r - 0.5 * b;
		double cg = -0.25 * r + 0.5 * g - 0.25 * b;
		y = y * 255 > 255 ? 255 : (y * 255 < 0 ? 0 : y * 255);
		im.pixel[i].f = round(y);
		im.pixel[i].s = round((co + 0.5) * 255);
		im.pixel[i].t = round((cg + 0.5) * 255);
	}
}

void from_YCoCg(image& im) {
	for (int i = 0; i < im.pixel.size(); i++) {
		double y = im.pixel[i].f * 1. / 255, co = (im.pixel[i].s * 1. / 255) - 0.5, cg = (im.pixel[i].t * 1. / 255) - 0.5;
		double r = y + co - cg;
		double g = y + cg;
		double b = y - co - cg;
		r = r * 255 > 255 ? 255 : (r * 255 < 0 ? 0 : r * 255);
		g = g * 255 > 255 ? 255 : (g * 255 < 0 ? 0 : g * 255);
		b = b * 255 > 255 ? 255 : (b * 255 < 0 ? 0 : b * 255);
		im.pixel[i].f = round(r);
		im.pixel[i].s = round(g);
		im.pixel[i].t = round(b);
	}
}

int main(int argc, char* argv[]) {

	if (argc != 11) {
		cerr << "Invalid request";
		return 1;
	}

	int _f = 0, _t = 0, _i = 0, _o = 0;
	string from_color, to_color;
	int from_cnt, to_cnt;
	string name_in, name_out;
	

	for (int i = 1; i < 11; i++) {
		if (!strcmp(argv[i], "-f")) {
			_f = 1;
			from_color = argv[i + 1];
		}
		if (!strcmp(argv[i], "-t")) {
			_t = 1;
			to_color = argv[i + 1];
		}
		if (!strcmp(argv[i], "-i")) {
			_i = 1;
			from_cnt = atoi(argv[i + 1]);
			name_in = argv[i + 2];
		}
		if (!strcmp(argv[i], "-o")) {
			_o = 1;
			to_cnt = atoi(argv[i + 1]);
			name_out = argv[i + 2];
		}
	}

	if (_t + _i + _o + _f < 4 || (to_cnt != 1 && to_cnt != 3) || (from_cnt != 1 && from_cnt != 3)) {
		cerr << "Invalid arguments";
		return 1;
	}
	vector<string> space = { "RGB", "HSL", "HSV", "YCbCr.601", "YCbCr.709", "YCoCg", "CMY" };
	
	int flag = 0;
	for (int i = 0; i < 7; i++) {
		if (from_color == space[i]) {
			flag++;
		}
		if (to_color == space[i]) {
			flag++;
		}
	}
	if (flag != 2) {
		cerr << "Invalid space";
		return 1;	
	}

	image im;

	if (from_cnt == 1) {
		FILE* fin = fopen(name_in.c_str(), "rb");
		if (!fin) {
			cerr << "Invalid input file";
			return 1;
		}
		int count = im.scan(fin);
		if (check_wrong(fin, im, '6', count)) {
			return 1;
		}
		int h = im.h, w = im.w;
		im.pixel.resize(h * w, {0, 0, 0 });
		pix new_pix;
		fread(&im.pixel[0], sizeof(pix), h * w, fin);
		fclose(fin);
	}
	else {
		for (int i = 0; i < 3; i++) {
			string new_name = name_in, s = "_";
			s += char('1' + i);
			new_name.insert(new_name.size() - 4, s);
			FILE* fin = fopen(new_name.c_str(), "rb");

			if (!fin) {
				cerr << "Invalid input file";
				return 1;
			}
			int count = im.scan(fin);
			if (check_wrong(fin, im, '5', count)) {
				return 1;
			}
			int h = im.h, w = im.w;
			if(i == 0)
				im.pixel.resize(h * w);
			vector<unsigned char> p(h * w, 0);
			int cnt = fread(&p[0], 1, p.size(), fin);
			if (cnt != h * w) {
				cerr << "Invalid input file";
				fclose(fin);
				return 1;
			}
			fclose(fin);
			for (int j = 0; j < h * w; j++) {

				if (i == 0)
					im.pixel[j].f = p[j];
				if (i == 1)
					im.pixel[j].s = p[j];
				if (i == 2)
					im.pixel[j].t = p[j];
			}
			
		}
	}

	if (to_color != from_color) {
		if (from_color == "HSL")
			from_HSL(im);
		if (from_color == "CMY")
			from_CMY(im);
		if (from_color == "HSV")
			from_HSV(im);
		if (from_color == "YCbCr.601")
			from_ycbcr_601(im);
		if (from_color == "YCbCr.709")
			from_ycbcr_709(im);
		if (from_color == "YCoCg")
			from_YCoCg(im);

		if (to_color == "HSL") 
			to_HSL(im);
		if (to_color == "CMY")
			to_CMY(im);
		if (to_color == "HSV")
			to_HSV(im);
		if (to_color == "YCbCr.601")
			to_ycbcr_601(im);
		if (to_color == "YCbCr.709")
			to_ycbcr_709(im);
		if (to_color == "YCoCg")
			to_YCoCg(im);
	}

	if (to_cnt == 1) {
		FILE* fout = fopen(name_out.c_str(), "wb");
		if (!fout) {
			cerr << "Invalid output file";
			return 1;
		}
		im.number = '6';
		im.print_header(fout);
		im.print_pixel(fout);
		fclose(fout);
	}
	else {
		vector<unsigned char> p(im.pixel.size(), 0);
		for (int i = 0; i < 3; i++) {
			string new_name = name_out, s = "_";
			s += char('1' + i);
			new_name.insert(new_name.size() - 4, s);
			FILE* fout = fopen(new_name.c_str(), "wb");

			if (!fout) {
				cerr << "Invalid output file";
				return 1;
			}
			im.number = '5';
			im.print_header(fout);
		
			for (int j = 0; j < im.pixel.size(); j++) {
				
				if (i == 0)
					p[j] = im.pixel[j].f;
				if (i == 1)
					p[j] = im.pixel[j].s;
				if (i == 2)
					p[j] = im.pixel[j].t;
			}
			
			fwrite(&p[0], 1, p.size(), fout);

			fclose(fout);
		}
	}
	return 0;

}
