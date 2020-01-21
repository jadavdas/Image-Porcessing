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

void ImageProcessing::setMask(int maskRows, int maskCols, const int maskData[])
{
    signed char *tmp;
    int elementSize;

    myMask.rows = maskRows;
    myMask.cols = maskCols;
    elementSize = myMask.rows * myMask.cols;
    myMask.data = new unsigned char[elementSize];

    tmp = (signed char *)myMask.data;

    for(int i=0; i<elementSize; i++)
    {
        *tmp = maskData[i];
        ++tmp;
    }
}
void ImageProcessing::convolve2D(unsigned char *inBuffer, unsigned char *outBuffer, int rows, int cols, struct Mask *myMask)
{
    long i, j, m, n, idx, jdx;
    int ms, im, val;
    unsigned char *tmp;

    for(i=0; i<rows; ++i)
    {
        for(j=0; j<cols; ++j)
        {
            val = 0;
            for(m=0; m<myMask->rows; ++m)
            {
                for(n=0; n<myMask->cols; ++n)
                {
                    ms = (signed char) *(myMask->data + m*myMask->rows + n);
                    idx = i - m;
                    jdx = j - n;
                    if(idx >= 0 && jdx >= 0)
                    {
                        im = *(inBuffer + idx*rows + jdx);
                    }
                    val += ms * im;
                }
            }
            if(val > NO_OF_GRAYLEVELS) val = NO_OF_GRAYLEVELS;
            if(val < 0) val = 0;
            tmp = outBuffer + i*rows + j;
            *tmp = (unsigned char)val;
        }
    }
}

void ImageProcessing::generateGaussNoise(unsigned char *inBuffer, int rows, int cols, float var, float mean)
{
    float theta, noise;

    for(int i=0; i<rows; i++)
    {
        for(int j=0; j<cols; j++)
        {
            noise = sqrt(-2*var*log(1.0 - (float)rand()/32767.1));
            theta = (float)rand()*1.9175345e-4 - 3.14159265;
            noise = noise * cos(theta);
            noise = noise + mean;

            if(noise > NO_OF_GRAYLEVELS) noise = NO_OF_GRAYLEVELS;
            if(noise < 0 ) noise = 0;

            *(inBuffer+j+(long)i*cols) = (unsigned char)(noise + 0.5);
        }
    }
}

void ImageProcessing::saltAndPepper(unsigned char *_inputImgData, int imgCols, int imgRows, float prob)
{
    int x,y,data1,data2,data;
    data = (int)(prob*32768/2);
    data1 = data +16384;
    data2 =  16384 - data;

    for(y =0;y<imgRows;y++)
        for(x=0;x<imgCols;x++)
    {
          data = rand();
         if(data >= 16384 && data< data1)
            *(_inputImgData+x+(long)y*imgCols) =0;
         if(data>=data2&& data<16384)
            *(_inputImgData+x+(long)y*imgCols) =255;
    }

}

void ImageProcessing::maximumFilter(unsigned char *inBuffer, unsigned char *outBuffer, int rows, int cols)
{
    int smax, n = 3;
    int a[11][11];
    for(int r=n/2; r<cols-n/2; r++)
    {
       for(int c=n/2; c<cols-n/2; c++)
       {
           smax = 0;
           for(int j=-n/2; j<=n/2; j++)
           {
               for(int i=-n/2; i<=n/2; i++)
                {
                    a[i+n/2][j+n/2] = *(inBuffer+c+i+(long)(r+j)*cols);
                }
                for(int j=0;j<=n-1;j++)
                {
                    for(int i =0;i<=n-1;i++)
                    {
                        if(a[i][j] > smax)
                        smax = a[i][j];
                    }
                }
                *(outBuffer+c+(long)r*cols) = smax;
           }
        }
    }
}

void ImageProcessing::medianFilter(unsigned char *inBuffer, unsigned char *outBuffer, int rows, int cols)
{
    int i, j, k, r, c;
    int n = 7, ar[121],a;

    for(r=n/2; r<rows-n/2; r++)
    {
        for(c=n/2; c<cols-n/2; c++)
        {
            k = 0;
            for(j=-n/2; j<=n/2; j++)
            {
                for(i=-n/2; i<=n/2; i++)
                {
                    ar[k] = *(inBuffer+c+i+(long)(r+j)*cols);
                    k++;
                }
            }
            for(j=1; j<=n*n-1; j++)
            {
                a = ar[j];
                i = j-1;
                while(i>=0 && ar[i] > a)
                {
                    ar[i+1] = ar[i];
                    i =i-1;
                }
                ar[i+1] = a;
            }
            *(outBuffer+c+(long)r*cols) =  ar[n*n/2];
        }
    }
}

void ImageProcessing::minimumFilter(unsigned char *inBuffer, unsigned char *outBuffer, int rows, int cols)
{
    int x, y,i,j,smin,n,a[11][11];
    n = 5;
    for(y=n/2; y<rows-n/2; y++)
    {
        for(x=n/2; x<cols-n/2; x++)
        {
            smin = 255;
            for(j=-n/2; j<=n/2; j++)
            {
                for(i=-n/2; i<=n/2; i++)
                {
                    a[i+n/2][j+n/2] =  *(inBuffer+x+i+(long)(y+j)*cols);

                }
            }
            for(j=0;j<=n-1;j++)
            {
                for(i=0;i<=n-1;i++)
                {
                    if(a[i][j]<smin)smin = a[i][j];
                }
            }
            *(outBuffer+x+(long)y*cols) = smin;
        }
    }
}

ImageProcessing::~ImageProcessing()
{
    //destructor
}
