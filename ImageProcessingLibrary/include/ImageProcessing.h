#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <math.h>
#include <stdlib.h>

// BMP file format that I will be using
static const int _512by512_IMG_SIZE     = 262144; //512*512
static const int BMP_COLOR_TABLE_SIZE   = 1024;
static const int BMP_HEADER_SIZE        = 54;
static const int MAX_COLOR              = 255;
static const int MIN_COLOR              = 0;
static const int WHITE                  = MAX_COLOR;
static const int BLACK                  = MIN_COLOR;
static const int NO_OF_GRAYLEVELS       = 255;

struct Mask
{
    int rows;
    int cols;
    unsigned char *data;
};

class ImageProcessing
{
    private:
        const char *inImgName;
        const char *outImgName;
        int *width;
        int *height;
        int *bitDepth;
        unsigned char *header;
        unsigned char *colorTable;
        unsigned char *inBuffer;
        unsigned char *outBuffer;

    public:
        // constructor
        ImageProcessing(const char *inImgName,
                        const char *outImgName,
                        int *width,
                        int *height,
                        int *bitDepth,
                        unsigned char *header,
                        unsigned char *colorTable,
                        unsigned char *inBuffer,
                        unsigned char *outBuffer
                        );

        Mask myMask;
        void readImage(); // read image file
        void writeImage(); // write image in a file
        void copyImageData(unsigned char *srcBuffer, unsigned char *destBuffer, int bufferSize); // copy from source to destination
        void binarizeImage(unsigned char *inBuffer, unsigned char *outBuffer, int imgSize, int threshold); //binarization
        void brightnessUp(unsigned char *inBuffer, unsigned char *outBuffer, int bufferSize, int brightness); // increase brightness
        void brightnessDown(unsigned char *inBuffer, unsigned char *outBuffer, int bufferSize, int darkness); // decrease brightness
        void computeHistogram(unsigned char *inBuffer, int rows, int cols, float imgHist[]); //calculate histogram
        void computeHistogram2(unsigned char *inBuffer, int rows, int cols, float imgHist[], const char *histFile); //calculate histogram
        void equalizeHistogram(unsigned char *inBuffer, unsigned char *outBuffer, int rows, int cols); //equalize histogram
        void getImageNegative(unsigned char *inBuffer, unsigned char *outBuffer, int imgHeight, int imgWidth); //negative image
        void detectLine(unsigned char *inBuffer, unsigned char *outBuffer, int rows, int cols, const int MASK[][3]); //line detection
        void setMask(int maskRows, int maskCols, const int maskData[]); //mask
        void convolve2D(unsigned char *inBuffer, unsigned char *outBuffer, int rows, int cols, struct Mask *myMask); //convolution
        void generateGaussNoise(unsigned char *inBuffer, int rows, int cols, float var, float mean); //generate noise
        void saltAndPepper(unsigned char *_inputImgData, int imgCols, int imgRows, float prob);
        void maximumFilter(unsigned char *inBuffer, unsigned char *outBuffer, int rows,int cols);
        void medianFilter(unsigned char *inBuffer, unsigned char *outBuffer, int rows, int cols);
        void minimumFilter(unsigned char *inBuffer, unsigned char *outBuffer, int rows, int cols);

        virtual ~ImageProcessing(); // destructor

    protected:

};

static const int LINE_DETECTOR_HOR_MASK[3][3] = { {-1, 2, -1}, {-1, 2, -1}, {-1, 2, -1} };
static const int LINE_DETECTOR_VER_MASK[3][3] = { {-1, -1, -1}, {2, 2, 2}, {-1, -1, -1} };
static const int LINE_DETECTOR_LDIA_MASK[3][3] = { {2, -1, -1}, {-1, 2, -1}, {-1, -1, 2} };
static const int LINE_DETECTOR_RDIA_MASK[3][3] = { {-1, -1, 2}, {-1, 2, -1}, {2, -1, -1} };

/* Prewitt operator - vertical edgess
-1 0 1
-1 0 1
-1 0 1
*/
static const int PREWITT_VER[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};

/* Prewitt operator - Horizontal edgess
-1 -1 -1
0   0  0
1   1  1
*/
static const int PREWITT_HOR[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};

#endif // IMAGEPROCESSING_H
