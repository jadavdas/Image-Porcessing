#include "ImageProcessing.h"

using namespace std;

int main()
{
    int imgWidth = 0, imgHeight= 0, imgBitDepth = 0;
    unsigned char imgHeader[BMP_HEADER_SIZE] = {0};
    unsigned char imgColorTable[BMP_COLOR_TABLE_SIZE] = {0};
    unsigned char imgInBuffer[_512by512_IMG_SIZE] = {0};
    unsigned char imgOutBuffer[_512by512_IMG_SIZE] = {0};
    float imgHistogram[NO_OF_GRAYLEVELS];

    const char imgName[] = "Images/houses.bmp";
    const char newImgName[] = "Images/houses_rdia.bmp";

    ImageProcessing *myImage = new ImageProcessing(imgName,
                                                   newImgName,
                                                   &imgWidth,
                                                   &imgHeight,
                                                   &imgBitDepth,
                                                   &imgHeader[0],
                                                   &imgColorTable[0],
                                                   &imgInBuffer[0],
                                                   &imgOutBuffer[0]
                                                   );

    myImage->readImage();
    //myImage->copyImageData(imgInBuffer, imgOutBuffer, _512by512_IMG_SIZE);
    // myImage->binarizeImage(imgInBuffer, imgOutBuffer, _512by512_IMG_SIZE, 100);
    //myImage->brightnessUp(imgInBuffer, imgOutBuffer, _512by512_IMG_SIZE, 50);
    //myImage->brightnessDown(imgInBuffer, imgOutBuffer, _512by512_IMG_SIZE, 50);
    //myImage->computeHistogram(imgInBuffer, imgHeight, imgWidth, imgHistogram);
    //myImage->equalizeHistogram(imgInBuffer, imgOutBuffer, imgHeight, imgWidth);
    //myImage->getImageNegative(imgInBuffer, imgOutBuffer, imgHeight, imgWidth);
    myImage->detectLine(imgInBuffer, imgOutBuffer, imgHeight, imgWidth, LINE_DETECTOR_RDIA_MASK);

    myImage->writeImage();

    cout<<"Success !"<<endl;
    cout<<"Image Width : "<<imgWidth<<endl;
    cout<<"Image Height : "<<imgHeight<<endl;

    return 0;
}
