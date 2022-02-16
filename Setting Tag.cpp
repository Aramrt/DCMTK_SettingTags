#include <iostream>
#include "dcmtk/config/osconfig.h" 
#include "dcmtk/dcmdata/dctk.h"
#include <fstream>
#include <string>
#include <opencv2/core.hpp>
#include<opencv2/opencv.hpp>
#include<vector>
#include <numeric>
#define UINT16_MAX 65535

using namespace std;
using namespace cv;

void Setting_PixelTag(DcmTag Tag, Mat src, DcmDataset* Radio_dataset);
void Setting_StringTag(DcmTag Tag, string Value, DcmDataset* Radio_dataset);
void Warning_Error(OFCondition status, string Tag_name);
string Casting_I2S(int dicom_value);

int main()
{
    
    DcmFileFormat Radio_Slice;
    OFCondition status = Radio_Slice.loadFile("D:/ct.dcm");
    if (status.bad())
    {
        cerr << "Not Loaded First Slice\n";
    }

    DcmDataset* Radio_dataset = Radio_Slice.getDataset();
    if (Radio_dataset->clear().bad())
    {
        cerr << "Dataset Not Cleared!\n";
    }
    // Setting Tags
    char uid[100];
    Mat Tiff = imread("V1.tiff", IMREAD_UNCHANGED);
    int Height = Tiff.rows;
    int Width = Tiff.cols;
    Setting_PixelTag(DCM_PixelData, Tiff, Radio_dataset);
    Setting_StringTag(DCM_PatientName, "Arshia Nikbakht", Radio_dataset);
    Setting_StringTag(DCM_Rows, Casting_I2S(Height), Radio_dataset);
    Setting_StringTag(DCM_Columns, Casting_I2S(Width), Radio_dataset);
    Setting_StringTag(DCM_BitsAllocated, "16", Radio_dataset);
    Setting_StringTag(DCM_BitsStored, "16", Radio_dataset);
    Setting_StringTag(DCM_HighBit, "15", Radio_dataset);
    Setting_StringTag(DCM_NumberOfFrames, "1", Radio_dataset);
    Setting_StringTag(DCM_PixelRepresentation, "0", Radio_dataset);

    DcmFileFormat Result(Radio_dataset);
    if (Result.saveFile("Final1.dcm").bad())
    {
        cout << "Not saved" << endl;
    }
   
    return 0;
}
void Warning_Error(OFCondition status, string Tag_name)
{
    if (status.bad())
    {
        cout << Tag_name << " Not Set\n";
    }
}
void Setting_StringTag(DcmTag Tag, string Value, DcmDataset* Radio_dataset)
{
    string Tag_name = Tag.getTagName();
    OFCondition status = Radio_dataset->putAndInsertString(Tag, Value.c_str());
    fflush(stdout);
    Warning_Error(status, Tag_name);
}
void Setting_PixelTag(DcmTag Tag, Mat Src, DcmDataset* Radio_dataset)
{
    string Tag_name = Tag.getTagName();
    Uint16* pixelDataOut;
    vector<Uint16> Pixels;
    int Height = Src.rows;
    int Width = Src.cols;
    for (int row = 0; row < Height; row++)
    {
        for (int col = 0; col < Width; col++)
        {
            Pixels.push_back((Uint16)Src.at<Uint16>(row, col));
        }
    }
    pixelDataOut = (Uint16*)malloc(Height * Width * sizeof(Uint16));
    for (int pixel = 0; pixel < Pixels.size(); pixel++)
    {
        Uint16 temp;
        temp = (uint)abs(floor(Pixels[pixel]));
        pixelDataOut[pixel] = temp;
    }
    OFCondition status = Radio_dataset->putAndInsertUint16Array(DCM_PixelData, pixelDataOut, Height * Width);
    fflush(stdout);
    Warning_Error(status, Tag_name);
}

string Casting_I2S(int Value)
{
    string OutCaster = to_string(Value).c_str();
    return OutCaster;
}