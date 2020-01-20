#include "ImageProcessing.h"

using namespace std;

ImageProcessing::ImageProcessing(const char *inImgName,
                        const char *outImgName,
                        int *width,
                        int *height,
                        int *bitDepth,
                        unsigned char *header,
                        unsigned char *colorTable,
                        unsigned char *inBuffer,
                        unsigned char *outBuffer
                        )
{
    // constructor
    this->inImgName = inImgName;
    this->outImgName = outImgName;
    this->width = width;
    this->height = height;
    this->bitDepth = bitDepth;
    this->header = header;
    this->colorTable = colorTable;
    this->inBuffer = inBuffer;
    this->outBuffer = outBuffer;
}

void ImageProcessing::readImage()
{
    FILE *inFile = fopen(inImgName,"rb");
    if(!inFile)
    {
        cout<<"File does not exist"<<endl;
        exit(0);
    }

    for(int i=0; i<BMP_HEADER_SIZE; i++)
    {
        header[i] = getc(inFile);
    }
    *width = *(int *)&header[18];
    *height = *(int *)&header[22];
    *bitDepth = *(int *)&header[28];

    if (*bitDepth <= 8)
    {
        fread(colorTable, sizeof(unsigned char), BMP_COLOR_TABLE_SIZE, inFile);
    }
    fread(inBuffer, sizeof(unsigned char), _512by512_IMG_SIZE, inFile);
    fclose(inFile);
}

void ImageProcessing::writeImage()
{
    FILE *outFile = fopen(outImgName, "wb");
    fwrite(header, sizeof(unsigned char), BMP_HEADER_SIZE, outFile);

    if(*bitDepth <= 8)
    {
        fwrite(colorTable, sizeof(unsigned char), BMP_COLOR_TABLE_SIZE, outFile);
    }
    fwrite(outBuffer, sizeof(unsigned char), _512by512_IMG_SIZE, outFile);
    fclose(outFile);
}

void ImageProcessing::copyImageData(unsigned char *inBuffer, unsigned char *outBuffer, int bufferSize)
{
    for(int i=0; i<bufferSize; i++)
    {
        outBuffer[i] = inBuffer[i];
    }
}

void ImageProcessing::binarizeImage(unsigned char *inBuffer, unsigned char *outBuffer, int bufferSize, int threshold)
{
    for(int i=0; i<bufferSize; i++)
    {
        outBuffer[i] = (inBuffer[i] > threshold ? WHITE: BLACK);
    }
}

void ImageProcessing::brightnessUp(unsigned char *inBuffer, unsigned char *outBuffer, int bufferSize, int brightness)
{
    for(int i=0; i<bufferSize; i++)
    {
        int temp = inBuffer[i] + brightness;
        outBuffer[i] = (temp > MAX_COLOR) ? MAX_COLOR : temp;
    }
}

void ImageProcessing::brightnessDown(unsigned char *inBuffer, unsigned char *outBuffer, int bufferSize, int darkness)
{
    for(int i=0; i<bufferSize; i++)
    {
        int temp = inBuffer[i] - darkness;
        outBuffer[i] = (temp < MIN_COLOR) ? MIN_COLOR : temp;
    }
}

void ImageProcessing::computeHistogram(unsigned char *inBuffer, int rows, int cols, float imgHist[])
{
    ofstream outFile;
    outFile.open("image_histogram.txt");
    vector<long int>tempHist(NO_OF_GRAYLEVELS);
    long int sum = 0;
    int k = 0;

    for(int i=0; i<rows; i++)
    {
        for (int j=0; j<cols; j++)
        {
            k = *(inBuffer+j+i*cols);
            tempHist[k] =  tempHist[k] + 1;
            sum = sum + 1;
        }
    }

    for(int i=0; i<NO_OF_GRAYLEVELS; i++)
    {
        imgHist[i] = (float)tempHist[i]/(float)sum;
        outFile<<imgHist[i]<<endl;
    }
    outFile.close();
}

void ImageProcessing::computeHistogram2(unsigned char *inBuffer, int rows, int cols, float imgHist[], const char *histFile)
{
    ofstream outFile;
    outFile.open(histFile);
    vector<long int>tempHist(NO_OF_GRAYLEVELS);
    long int sum = 0;
    int k = 0;

    for(int i=0; i<rows; i++)
    {
        for (int j=0; j<cols; j++)
        {
            k = *(inBuffer+j+i*cols);
            tempHist[k] =  tempHist[k] + 1;
            sum = sum + 1;
        }
    }

    for(int i=0; i<NO_OF_GRAYLEVELS; i++)
    {
        imgHist[i] = (float)tempHist[i]/(float)sum;
        outFile<<imgHist[i]<<endl;
    }
    outFile.close();
}

void ImageProcessing::equalizeHistogram(unsigned char *inBuffer, unsigned char *outBuffer, int rows, int cols)
{
    vector<int>histEq(NO_OF_GRAYLEVELS);
    vector<float>tempHist(NO_OF_GRAYLEVELS);
    float sum = 0;

    const char initialHistogram[] = "initial_histogram.txt";
    const char finalHistogram[] = "final_histogram.txt";

    computeHistogram2(&inBuffer[0], rows, cols, &tempHist[0], initialHistogram);

    for(int i=0; i<NO_OF_GRAYLEVELS; i++)
    {
        sum = 0.0;
        for(int j=0; j<=i; j++)
        {
            sum = sum + tempHist[j];
        }
        histEq[i] = (int)(NO_OF_GRAYLEVELS*sum + 0.5);
    }

    for(int i=0; i<rows; i++)
    {
        for(int j=0; j<cols; j++)
        {
            *(outBuffer+j+i*cols) = histEq[*(inBuffer+j+i*cols)];
        }
    }
    computeHistogram2(&outBuffer[0], rows, cols, &tempHist[0], finalHistogram);
}

void ImageProcessing::getImageNegative(unsigned char *inBuffer, unsigned char *outBuffer, int imgHeight, int imgWidth)
{
    for(int i=0; i<imgHeight; i++)
    {
        for(int j=0; j<imgWidth; j++)
        {
            outBuffer[i*imgWidth+j] = 255 - inBuffer[i*imgWidth+j];
        }
    }
}

void ImageProcessing::detectLine(unsigned char *inBuffer, unsigned char *outBuffer, int rows, int cols, const int MASK[][3])
{
    for(int r=1; r<=rows-1; r++)
    {
        for(int c=1; c<=cols; c++)
        {
            int sum = 0;
            for(int i=-1; i<=1; i++)
            {
                for(int j=-1; j<=1; j++)
                {
                    sum = sum + *(inBuffer+c+i+(long)(r+j)*cols)*MASK[i+1][j+1];
                }
            }
            if(sum > NO_OF_GRAYLEVELS)
            {
                sum = NO_OF_GRAYLEVELS;
            }
            if(sum < 0)
            {
                sum = 0;
            }
            *(outBuffer+c+(long)r*cols) = sum;
        }
    }
}
//void ImageProcessing::convolve2D(unsigned char *inBuffer, unsigned char *outBuffer,int rows, int cols, struct Mask *myMask)
//{

//}
ImageProcessing::~ImageProcessing()
{
    //destructor
}
