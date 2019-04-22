#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

/*void sobel_operator(PPMImage *img, PPMImage *img2)
{
        int i;
        if(img){
                int * pixels = malloc(9 * sizeof(unsigned char))
                for(i = 0; i < img->x * img->y; i++){
                        if (i % img->x == 0) {
                                pixels[0] = 0;
                                pixels[3] = 0;
                                pixels[6] = 0;
                        }
                        if (i < img->x) {
                                pixels[0] = 0;
                                pixels[1] = 0;
                                pixels[2] = 0;
                        }
                        if ((i % img->x) == img->x - 1) {
                                pixels[2] = 0;
                                pixels[5] = 0;
                                pixels[8] = 0;
                        }
                        img->data[i].red=RGB_COMPONENT_COLOR-img->data[i].red;
                        img->data[i].green=RGB_COMPONENT_COLOR-img->data[i].green;
                        img->data[i].blue=RGB_COMPONENT_COLOR-img->data[i].blue;
                }
        }
}
*/
int main(){
        clock_t begin = clock();
        PPMImage *in;
        in = readPPM("img.ppm");
        PPMImage *out_image;
        writePPM("out.ppm",in);
        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Extecution time: %.1lf ms\n", time_spent * 1000);
}
