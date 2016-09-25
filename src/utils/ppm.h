#ifndef PPM_H
#define PPM_H 1

#define PPM_BLACK  rgb2color(0, 0, 0)
#define PPM_WHITE  rgb2color(255, 255, 255)
#define PPM_RED    rgb2color(255,   0,   0)
#define PPM_GREEN  rgb2color(  0, 255,   0)
#define PPM_BLUE   rgb2color(  0,   0, 255)
#define PPM_YELLOW rgb2color( 255, 255,  0)

typedef struct PPMColor{
    unsigned char r;
    unsigned char g;
    unsigned char b;
}PPMColor;

PPMColor rgb2color(unsigned char r, unsigned char g, unsigned char b);

typedef struct PPM{
    int lines;
    int cols;
    int max;
    PPMColor *canvas;
}PPM;

PPM *ppm_new(int cols, int lines);
void ppm_free(PPM *ppm);
PPMColor ppm_get_px(PPM *ppm, int x, int y);
int ppm_get_maxy(PPM *ppm);
int ppm_get_maxx(PPM *ppm);
PPM *ppm_set_px(PPM *ppm, int x, int y, PPMColor color);
PPM *ppm_paint_dot(PPM *ppm, int x, int y, PPMColor color, int radius);
PPM *ppm_paint_vline(PPM *ppm, int x, int low_y, int hig_y, int width, PPMColor color);
PPM *ppm_paint_hline(PPM *ppm, int low_x, int hig_x, int y, int width, PPMColor color);
void ppm_write(PPM *ppm, char *filename);

#endif

