#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef struct {
        unsigned char red,green,blue;
} PPMPixel;

typedef struct {
        int x, y;
        PPMPixel **data;
} PPMImage;

#define RGB_COMPONENT_COLOR 255

static PPMImage *readPPM(const char *filename)
{
        char buff[16];
        PPMImage *img;
        FILE *fp;
        int c, rgb_comp_color;
        //open PPM file for reading
        fp = fopen(filename, "rb");
        if (!fp) {
                fprintf(stderr, "Unable to open file '%s'\n", filename);
                exit(1);
        }

        //read image format
        if (!fgets(buff, sizeof(buff), fp)) {
                perror(filename);
                exit(1);
        }

        //check the image format
        if (buff[0] != 'P' || buff[1] != '6') {
                fprintf(stderr, "Invalid image format (must be 'P6')\n");
                exit(1);
        }

        //alloc memory form image
        img = (PPMImage *)malloc(sizeof(PPMImage));
        if (!img) {
                fprintf(stderr, "Unable to allocate memory\n");
                exit(1);
        }

        //check for comments
        c = getc(fp);
        while (c == '#') {
        while (getc(fp) != '\n') ;
                c = getc(fp);
        }

        ungetc(c, fp);
        //read image size information
        if (fscanf(fp, "%d %d", &img->x, &img->y) != 2) {
                fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
                exit(1);
        }

        //read rgb component
        if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
                fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
                exit(1);
        }

        while (fgetc(fp) != '\n') ;
        //memory allocation for pixel data
        img->data = (PPMPixel **)malloc(img->x * sizeof(PPMPixel*));
        for (int i = 0; i < img->x; i++)
                img->data[i] = malloc(img->y * sizeof(PPMPixel));
        if (!img) {
                fprintf(stderr, "Unable to allocate memory\n");
                exit(1);
        }

        //read pixel data from file
        for (int i = 0; i < img->x; i++) {
                fread(img->data[i], 3,img->y, fp);
        }

        fclose(fp);
        return img;
}

void writePPM(const char *filename, PPMImage *img)
{
        FILE *fp;
        //open file for output
        fp = fopen(filename, "wb");
        if (!fp) {
                fprintf(stderr, "Unable to open file '%s'\n", filename);
                exit(1);
        }

        //write the header file
        fprintf(fp, "P6\n");

        //image size
        fprintf(fp, "%d %d\n",img->x,img->y);

        // rgb component depth
        fprintf(fp, "%d\n",RGB_COMPONENT_COLOR);

        // pixel data
        for (int i = 0; i < img->x; i++) {
                fwrite(img->data[i], 3 ,img->y, fp);
        }
        fclose(fp);
}

static PPMImage *sobel_operator(PPMImage *img)
{
        PPMImage * img2 = (PPMImage *)malloc(sizeof(PPMImage));
        img2->data = (PPMPixel **)malloc(img->x * sizeof(PPMPixel*));
        for (int i = 0; i < img->x; i++){
                img2->data[i] = malloc(img->y * sizeof(PPMPixel));
                for (int j = 0; j < img->y; j++) {
                        int temp = 0;
                        temp += img->data[i][j].red;
                        temp += img->data[i][j].green;
                        temp += img->data[i][j].blue;
                        temp = (int)temp/3;
                        img->data[i][j].red = temp;
                        img->data[i][j].green = temp;
                        img->data[i][j].blue = temp;
                }
        }
        img2->x = img->x;
        img2->y = img->y;
        if(img){
                for (int i = 0; i < img2->x; i++) {
                        int * pixels = malloc(9 * sizeof(int));
                        int temp = 0;
                        for (int j = 0; j < img2->y; j++){
                                for (int l = 0; l < 9; l++)
                                        pixels[l] = -1;
                                if (i == 0) {
                                        pixels[0] = 0;
                                        pixels[3] = 0;
                                        pixels[6] = 0;
                                }
                                if (j == 0) {
                                        pixels[0] = 0;
                                        pixels[1] = 0;
                                        pixels[2] = 0;
                                }
                                if (i == img2->x - 1) {
                                        pixels[2] = 0;
                                        pixels[5] = 0;
                                        pixels[8] = 0;
                                }
                                if (j == img2->y - 1) {
                                        pixels[6] = 0;
                                        pixels[7] = 0;
                                        pixels[8] = 0;
                                }
                                for (int k = 0; k < 9; k++) {
                                        printf("%d %d\n", i + (k / 3), j + (k % 3));
                                        if (pixels[k] != 0) {
                                                pixels[k] = img->data[i + (k / 3)][j + (k % 3)].red;
                                        }
                                }
                                int g_x = (pixels[6] + 2*pixels[7] + pixels[8]) - (pixels[0] + 2*pixels[1] + pixels[2]);
                                int g_y = (pixels[2] + 2*pixels[5] + pixels[8]) - (pixels[0] + 2*pixels[3] + pixels[6]);
                                int res = (int)sqrt(g_x * g_x + g_y * g_y);
                                temp += res;
                                temp = (int)temp/3;
                                img2->data[i][j].red = temp;
                                img2->data[i][j].green = temp;
                                img2->data[i][j].blue = temp;
                        }
                }
        }
        return img2;
}

int main(){
        clock_t begin = clock();
        PPMImage *in_image;
        in_image = readPPM("img.ppm");
        PPMImage *out_image;
        out_image = sobel_operator(in_image);
        writePPM("out.ppm",out_image);
        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Extecution time: %.1lf ms\n", time_spent * 1000);
}
