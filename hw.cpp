#include "hw.h"

#include <cstdlib>

HW::HW()
{

}

double HW::deg2rad(double deg)
{
    return deg*_PI/180;
}

double HW::rad2deg(double rad)
{
    return rad*180/_PI;
}

void HW::splitRGB(KImageColor &imgRGB)
{
    KImageGray imgGray[3];

    imgRGB.SplitInto(imgGray[0], imgGray[1], imgGray[2]);

    for(unsigned int i = 0; i < imgRGB.Row(); i++)
    {
        for(unsigned int j = 0;j < imgRGB.Col(); j++)
        {
            imgRGB[i][j].r = imgGray[0][i][j];
            imgRGB[i][j].g = imgGray[1][i][j];
            imgRGB[i][j].b = imgGray[2][i][j];
        }
    }
}


///// ===== HW ===== /////

/// --- HW01 --- ///
KArray<KHSI> HW::RGB2HSI(KImageColor igRGB)
{
    KArray<KHSI> igHSI(igRGB.Row(), igRGB.Col());

    float dMin, dTheta, dThetaDen, dThetaNum;

    for(int i = 0, ii = igRGB.Row(); ii; i++, ii--)
    {
        for(int j = 0, jj = igRGB.Col(); jj; j++, jj--)
        {
            dMin  = _MIN(_MIN(igRGB[i][j].r, igRGB[i][j].g), igRGB[i][j].b);

            // Intensity
            igHSI[i][j].i = (float)((igRGB[i][j].r + igRGB[i][j].g + igRGB[i][j].b) / 3);

            // Saturation
            if(igHSI[i][j].i != 0)
                igHSI[i][j].s = (float)(1 - dMin / igHSI[i][j].i);
            else
                igHSI[i][j].s = 0;

            // Hue
            dThetaNum = (float)( ( (igRGB[i][j].r - igRGB[i][j].g) + (igRGB[i][j].r - igRGB[i][j].b) ) / 2 );
            dThetaDen = (float)( pow(igRGB[i][j].r - igRGB[i][j].g, 2) + (igRGB[i][j].r - igRGB[i][j].b) * (igRGB[i][j].g - igRGB[i][j].b) );
            dTheta = (float)( ( acos(dThetaNum / sqrt(dThetaDen)) ) );
            dTheta = rad2deg(dTheta); // 단위변환

            if(igRGB[i][j].g >= igRGB[i][j].b)
                igHSI[i][j].h = dTheta;
            else
                igHSI[i][j].h = (float)( 360.0 - dTheta );
        }
    }

    return igHSI;
}

KArray<KTRIPLETF> HW::RGB2NRG(KImageColor igRGB)
{
    KArray<KTRIPLETF> igNRG(igRGB.Row(), igRGB.Col());

    for(int i = 0, ii = igRGB.Row(); ii; i++, ii--)
    {
        for(int j = 0, jj = igRGB.Col(); jj; j++, jj--)
        {
            // R + G + B
            igRGB[i][j].dummy = igRGB[i][j].r + igRGB[i][j].g + igRGB[i][j].b;

            // Normalized Red
            igNRG[i][j].r = (igRGB[i][j].dummy != 0) ? (float)(igRGB[i][j].r / igRGB[i][j].dummy) : 0.0;

            // Normalized Green
            igNRG[i][j].g = (igRGB[i][j].dummy != 0) ? (float)(igRGB[i][j].g / igRGB[i][j].dummy) : 0.0;

            // Blue
            //igNRG[i][j].b = (float)(1 - (igNRG[i][j].r + igNRG[i][j].g));
        }
    }

    return igNRG;
}


/// --- HW02 --- ///
KImageColor HW::ContrastTransform_RGB(KImageColor igRGB, const int& nA, const int& nB,const int& nAp, const int& nBp)
{
#ifdef _DEBUG
    assert(nB > nA && nB<256 && nA<256);
#endif

    double dXin_R, dXin_G, dXin_B, dSlope = (double)(nBp-nAp) / (double)(nB-nA);

    for(int i = 0, ii = igRGB.Row(); ii; i++, ii--)
    {
        for(int j = 0, jj = igRGB.Col(); jj; j++, jj--)
        {
            dXin_R = (igRGB[i][j].r < nA ? nA : (igRGB[i][j].r > nB ? nB : igRGB[i][j].r));
            igRGB[i][j].r = (unsigned char)(dSlope * (dXin_R - nA) + nAp);

            dXin_G = (igRGB[i][j].g < nA ? nA : (igRGB[i][j].g > nB ? nB : igRGB[i][j].g));
            igRGB[i][j].g = (unsigned char)(dSlope * (dXin_G - nA) + nAp);

            dXin_B = (igRGB[i][j].b < nA ? nA : (igRGB[i][j].b > nB ? nB : igRGB[i][j].b));
            igRGB[i][j].b = (unsigned char)(dSlope * (dXin_B - nA) + nAp);
        }
    }

    return igRGB;
}


/// --- HW03 --- ///
KImageGray HW::BinaryDilation(KImageGray imgGray, const int& nType, int kernel)
{
    KImageGray igOut = imgGray;
    int nHalf = kernel/2; //마스크 중앙 기준 몇칸 옆으로 갈지(마스크 사이즈에 따라 설정)
    // 3->1, 5->2

    if(nType == _FOREGROUND)
    {
        for(unsigned int i = nHalf; i < igOut.Row() - nHalf; i++)
            for(unsigned int j = nHalf; j < igOut.Col() - nHalf; j++)
            {
                if(igOut[i][j] == 0)
                {
                    // Mask
                    for(unsigned int mi = i - nHalf; mi <= i + nHalf; mi++)
                        for(unsigned int mj = j - nHalf; mj <= j + nHalf; mj++)
                        {
                            if(imgGray[mi][mj])
                            {
                                igOut[i][j] = 255;
                                break;
                            }
                        }
                }
            }
    }
    if(nType == _BACKGROUND)
    {
        for(unsigned int i = nHalf; i < igOut.Row() - nHalf; i++)
            for(unsigned int j = nHalf; j < igOut.Col() - nHalf; j++)
            {
                if(igOut[i][j] != 0)
                {
                    // Mask
                    for(unsigned int mi = i - nHalf; mi < i + nHalf; mi++)
                        for(unsigned int mj = j - nHalf; mj < j + nHalf; mj++)
                            if(!igOut[mi][mj])
                            {
                                igOut[i][j] = 0;
                                break;
                            }
                }
            }
    }

    return igOut;
}

KImageGray HW::BinaryErosion(KImageGray imgGray, const int& nType, int kernel)
{
    KImageGray igOut = imgGray;
    int nHalf = kernel/2;

    if(nType == _FOREGROUND)
    {
        for(unsigned int i = nHalf; i < igOut.Row() - nHalf; i++)
            for(unsigned int j = nHalf; j < igOut.Col() - nHalf; j++)
            {
                if(imgGray[i][j] != 0)
                {
                    // Mask
                    for(unsigned int mi = i - nHalf; mi <= i + nHalf; mi++)
                        for(unsigned int mj = j - nHalf; mj <= j + nHalf; mj++)
                        {
                            if(!imgGray[mi][mj])
                            {
                                igOut[i][j] = 0;
                                break;
                            }
                        }
                }
            }
    }
    if(nType == _BACKGROUND)
    {
        for(unsigned int i = nHalf; i < igOut.Row() - nHalf; i++)
            for(unsigned int j = nHalf; j < igOut.Col() - nHalf; j++)
            {
                if(igOut[i][j] == 0)
                {
                    // Mask
                    for(unsigned int mi = i - nHalf; mi < i + nHalf; mi++)
                        for(unsigned int mj = j - nHalf; mj < j + nHalf; mj++)
                            if(igOut[mi][mj])
                            {
                                igOut[i][j] = 255;
                                break;
                            }
                }
            }
    }

    return igOut;
}

KImageColor HW::Labeling_Neighbor4(KImageGray imgGray, int kernel, int &index)
{ 
    KImageColor imgLabeling(imgGray.Row(), imgGray.Col()); // Result Image
    KImageWord idLabeling(imgGray.Row(), imgGray.Col()); // Assign ID
    vector<int> idTable;

    int nHalf = kernel/2;
    int id = 1;

    // Labeling
    for(unsigned int i = nHalf + 1; i < imgGray.Row() - nHalf; i++)
    {
        for(unsigned int j = nHalf + 1; j < imgGray.Col() - nHalf; j++)
        {
            int curLeft = imgGray[i][j-1]; // current pixel's Left value
            int curUp = imgGray[i-1][j]; // current pixel's Up value

            // Background
            if(imgGray[i][j] == 0)
                continue;

            if(curUp == 0 && curLeft == 0) // new Label
            {
                idLabeling[i][j] = id++;
            }
            else if(curUp && curLeft == 0) // Up과 같은 상황
            {
                if(idLabeling[i-1][j] != 0)                 // Up이 라벨링된 상태일 때
                    idLabeling[i][j] = idLabeling[i-1][j];
                else                                        // Up에 라벨링이 안 된 상태일 때
                {
                    idLabeling[i][j] = id++;                // new Label
                    idLabeling[i-1][j] = idLabeling[i][j];  // Up에 같은 ID 부여
                }
            }
            else if(curUp == 0 && curLeft) // Left와 같은 상황
            {
                if(idLabeling[i][j-1] != 0)                 // Left가 라벨링된 상태일 때
                    idLabeling[i][j] = idLabeling[i][j-1];
                else                                        // Left가 라벨링이 안 된 상태일 때
                {
                    idLabeling[i][j] = id++;                // new Label
                    idLabeling[i][j-1] = idLabeling[i][j];  // Left에 같은 ID 부여
                }
            }
            else // Up과 Left 둘 다 foreground인 상황
            {
                if(idLabeling[i-1][j] != 0 && idLabeling[i][j-1] != 0) // Up과 Left 둘 다 라벨링이 된 상태
                {
                    idLabeling[i][j] = idLabeling[i-1][j];
                    if(idLabeling[i][j-1] != idLabeling[i-1][j]) // Up과 Left의 ID가 다를 때
                    {
                        idTable.push_back(idLabeling[i][j-1]); // curLeft
                        idTable.push_back(idLabeling[i-1][j]); // curUp
                    }
                }
                else if(idLabeling[i-1][j] != 0 && idLabeling[i][j-1] == 0) // Left에 라벨링이 안 된 상태
                {
                    idLabeling[i][j] = idLabeling[i-1][j];
                    idLabeling[i][j-1] = idLabeling[i][j];
                }
                else if(idLabeling[i-1][j] == 0 && idLabeling[i][j-1] != 0) // Up에 라벨링이 안 된 상태
                {
                    idLabeling[i][j] = idLabeling[i][j-1];
                    idLabeling[i-1][j] = idLabeling[i][j];
                }
                else if(idLabeling[i-1][j] == 0 && idLabeling[i][j-1] == 0) // Up과 Left 둘 다 라벨링이 안 된 상태
                {
                    idLabeling[i][j] = id++;
                    idLabeling[i-1][j] = idLabeling[i][j];
                    idLabeling[i][j-1] = idLabeling[i][j];
                }
            }
        }
    }

    // Merge ID
    for(unsigned int k = 0; k < idTable.size(); k+=2)
    {
        for(unsigned int i = nHalf; i < imgGray.Row() - nHalf; i++)
        {
            for(unsigned int j = nHalf; j < imgGray.Col() - nHalf; j++)
            {
                if(idLabeling[i][j] == idTable[k+1])
                    idLabeling[i][j] = idTable[k];
            }
        }
    }

    // Color
    index = randomColor(idLabeling, id, imgLabeling);

    return imgLabeling;
}

KImageColor HW::Labeling_Neighbor8(KImageGray imgGray, int kernel, int &index)
{
    KImageColor imgLabeling(imgGray.Row(), imgGray.Col()); // Result Image
    KImageWord idLabeling(imgGray.Row(), imgGray.Col()); // Assign ID
    vector<int> idTable;

    int nHalf = kernel/2;
    int id = 1;

    // Labeling
    for(unsigned int i = nHalf + 1; i < imgGray.Row() - nHalf; i++)
    {
        for(unsigned int j = nHalf + 1; j < imgGray.Col() - nHalf; j++)
        {
            int curLeft = imgGray[i][j-1]; // current pixel's Left value
            int curUp = imgGray[i-1][j]; // current pixel's Up value
            int curUL = imgGray[i-1][j-1]; // current pixel's Upper Left value

            // Background
            if(imgGray[i][j] == 0)
                continue;

            if(curUp == 0 && curUL == 0 && curLeft == 0) // new Label
            {
                idLabeling[i][j] = id++;
            }
            else if(curUp && curUL == 0 && curLeft == 0) // Up
            {
                if(idLabeling[i-1][j] != 0)
                    idLabeling[i][j] = idLabeling[i-1][j];
                else
                {
                    idLabeling[i][j] = id++;
                    idLabeling[i-1][j] = idLabeling[i][j];
                }
            }
            else if(curUp == 0 && curUL && curLeft == 0) // Upper Left
            {
                if(idLabeling[i-1][j-1] != 0)
                    idLabeling[i-1][j-1] = idLabeling[i-1][j-1];
                else
                {
                    idLabeling[i][j] = id++;
                    idLabeling[i-1][j-1] = idLabeling[i][j];
                }
            }
            else if(curUp == 0 && curUL == 0 && curLeft) // Left
            {
                if(idLabeling[i][j-1] != 0)
                    idLabeling[i][j] = idLabeling[i][j-1];
                else
                {
                    idLabeling[i][j] = id++;
                    idLabeling[i][j-1] = idLabeling[i][j];
                }
            }
            else if(curUp && curUL && curLeft == 0) // Up이랑 Upper Left ==========================
            {

                if(idLabeling[i-1][j] != 0 && idLabeling[i-1][j-1] != 0)
                {
                    idLabeling[i][j] = idLabeling[i-1][j];

                    if(idLabeling[i-1][j] != idLabeling[i-1][j-1]) // 서로 ID가 다를 때
                    {
                        idTable.push_back(idLabeling[i-1][j]); // curUp
                        idTable.push_back(idLabeling[i-1][j-1]); // curUL
                    }
                }
                else if(idLabeling[i-1][j] != 0 && idLabeling[i-1][j-1] == 0)
                {
                    idLabeling[i][j] = idLabeling[i-1][j];
                    idLabeling[i-1][j-1] = idLabeling[i-1][j];
                }
                else if(idLabeling[i-1][j] == 0 && idLabeling[i-1][j-1] != 0)
                {
                    idLabeling[i][j] = idLabeling[i-1][j-1];
                    idLabeling[i-1][j] = idLabeling[i-1][j-1];
                }
                else
                {
                    idLabeling[i][j] = id++;
                    idLabeling[i-1][j] = idLabeling[i][j];
                    idLabeling[i-1][j-1] = idLabeling[i][j];
                }

            }
            else if(curUp && curUL == 0 && curLeft) // Neighbor4 참고
            {
                if(idLabeling[i-1][j] != 0 && idLabeling[i][j-1] != 0) // Up과 Left 둘 다 라벨링이 된 상태
                {
                    idLabeling[i][j] = idLabeling[i-1][j];

                    if(idLabeling[i][j-1] != idLabeling[i-1][j]) // Up과 Left의 ID가 다를 때
                    {
                        idTable.push_back(idLabeling[i][j-1]); // curLeft
                        idTable.push_back(idLabeling[i-1][j]); // curUp
                    }
                }
                else if(idLabeling[i-1][j] != 0 && idLabeling[i][j-1] == 0) // Left에 라벨링이 안 된 상태
                {
                    idLabeling[i][j] = idLabeling[i-1][j];
                    idLabeling[i][j-1] = idLabeling[i][j];
                }
                else if(idLabeling[i-1][j] == 0 && idLabeling[i][j-1] != 0) // Up에 라벨링이 안 된 상태
                {
                    idLabeling[i][j] = idLabeling[i][j-1];
                    idLabeling[i-1][j] = idLabeling[i][j];
                }
                else if(idLabeling[i-1][j] == 0 && idLabeling[i][j-1] == 0) // Up과 Left 둘 다 라벨링이 안 된 상태
                {
                    idLabeling[i][j] = id++;
                    idLabeling[i-1][j] = idLabeling[i][j];
                    idLabeling[i][j-1] = idLabeling[i][j];
                }
            }
            else if(curUp == 0 && curUL && curLeft) // Upper Left랑 Left
            {
                if(idLabeling[i-1][j-1] != 0 && idLabeling[i][j-1] != 0) // 둘 다 라벨링이 된 상태
                {
                    idLabeling[i][j] = idLabeling[i][j-1];

                    if(idLabeling[i-1][j-1] != idLabeling[i][j-1]) // Up과 Left의 ID가 다를 때
                    {
                        idTable.push_back(idLabeling[i-1][j-1]);
                        idTable.push_back(idLabeling[i][j-1]);
                    }
                }
                else if(idLabeling[i-1][j-1] != 0 && idLabeling[i][j-1] == 0)
                {
                    idLabeling[i][j] = idLabeling[i-1][j-1];
                    idLabeling[i][j-1] = idLabeling[i-1][j-1];
                }
                else if(idLabeling[i-1][j-1] == 0 && idLabeling[i][j-1] != 0)
                {
                    idLabeling[i][j] = idLabeling[i][j-1];
                    idLabeling[i-1][j-1] = idLabeling[i][j-1];
                }
                else if(idLabeling[i-1][j] == 0 && idLabeling[i][j-1] == 0)
                {
                    idLabeling[i][j] = id++;
                    idLabeling[i-1][j-1] = idLabeling[i][j];
                    idLabeling[i][j-1] = idLabeling[i][j];
                }
            }
            else // 3영역 다 foreground
            {
                if(idLabeling[i-1][j] != 0 && idLabeling[i][j-1] != 0 && idLabeling[i-1][j-1] != 0) // 3영역 다 라벨링된 상태
                {
                    idLabeling[i][j] = idLabeling[i-1][j];

                    if(idLabeling[i][j-1] != idLabeling[i-1][j]) // Up과 Left의 ID가 다를 때   =========================================================
                    {
                        idTable.push_back(idLabeling[i-1][j]); // curUp
                        idTable.push_back(idLabeling[i][j-1]); // curLeft
                    }
                    if(idLabeling[i-1][j-1] != idLabeling[i-1][j])
                    {
                        idTable.push_back(idLabeling[i-1][j-1]); // curUpper Left
                        idTable.push_back(idLabeling[i-1][j]); // curUp
                    }
                }
                else if(idLabeling[i-1][j] != 0 && idLabeling[i][j-1] == 0 && idLabeling[i-1][j-1] == 0)
                {
                    idLabeling[i][j] = idLabeling[i-1][j];
                    idLabeling[i-1][j-1] = idLabeling[i-1][j];
                    idLabeling[i][j-1] = idLabeling[i-1][j];
                }
                else if(idLabeling[i-1][j] == 0 && idLabeling[i][j-1] != 0 && idLabeling[i-1][j-1] == 0)
                {
                    idLabeling[i][j] = idLabeling[i-1][j-1];
                    idLabeling[i-1][j-1] = idLabeling[i][j-1];
                    idLabeling[i-1][j] = idLabeling[i][j-1];
                }
                else if(idLabeling[i-1][j] == 0 && idLabeling[i][j-1] == 0 && idLabeling[i-1][j-1] != 0)
                {
                    idLabeling[i][j] = idLabeling[i-1][j-1];
                    idLabeling[i-1][j] = idLabeling[i-1][j-1];
                    idLabeling[i][j-1] = idLabeling[i-1][j-1];
                }
                else if(idLabeling[i-1][j] != 0 && idLabeling[i][j-1] != 0 && idLabeling[i-1][j-1] == 0)
                {
                    idLabeling[i][j] = idLabeling[i-1][j];
                    idLabeling[i-1][j-1] = idLabeling[i-1][j];

                    if(idLabeling[i-1][j] != idLabeling[i][j-1])
                    {
                        idTable.push_back(idLabeling[i-1][j]);
                        idTable.push_back(idLabeling[i][j-1]);
                    }
                }
                else if(idLabeling[i-1][j] != 0 && idLabeling[i][j-1] == 0 && idLabeling[i-1][j-1] != 0)
                {
                    idLabeling[i][j] = idLabeling[i-1][j];
                    idLabeling[i][j-1] = idLabeling[i-1][j];

                    if(idLabeling[i-1][j] != idLabeling[i-1][j-1])
                    {
                        idTable.push_back(idLabeling[i-1][j]);
                        idTable.push_back(idLabeling[i-1][j-1]);
                    }
                }
                else if(idLabeling[i-1][j] == 0 && idLabeling[i][j-1] != 0 && idLabeling[i-1][j-1] != 0)
                {
                    idLabeling[i][j] = idLabeling[i][j-1];
                    idLabeling[i-1][j] = idLabeling[i][j-1];

                    if(idLabeling[i-1][j-1] != idLabeling[i][j-1])
                    {
                        idTable.push_back(idLabeling[i][j-1]);
                        idTable.push_back(idLabeling[i-1][j-1]);
                    }
                }
                else if(idLabeling[i-1][j] == 0 && idLabeling[i][j-1] == 0 && idLabeling[i-1][j-1] == 0)
                {
                    idLabeling[i][j] = id++;
                    idLabeling[i-1][j] = idLabeling[i][j];
                    idLabeling[i-1][j-1] = idLabeling[i][j];
                    idLabeling[i][j-1] = idLabeling[i][j];
                }
            }
        }
    }

    // Merge ID
    for(unsigned int k = 0; k < idTable.size(); k+=2)
    {
        for(unsigned int i = nHalf; i < imgGray.Row() - nHalf; i++)
        {
            for(unsigned int j = nHalf; j < imgGray.Col() - nHalf; j++)
            {
                if(idLabeling[i][j] == idTable[k+1])
                    idLabeling[i][j] = idTable[k];
            }
        }
    }

    // Color
    index = randomColor(idLabeling, id, imgLabeling);

    return imgLabeling;
}

int HW::randomColor(KImageWord idLabeling, int id, KImageColor &imgLabeling)
{
    // Color
    int index = 0;
    srand((unsigned int)time(NULL));
    for(int colorID = 1; colorID < id; colorID++)
    {
        bool check = false;

        int ColorR = rand() % 255 + 1;
        int ColorG = rand() % 255 + 1;
        int ColorB = rand() % 255 + 1;

        for(unsigned int i = 1; i < imgLabeling.Row() - 1; i++)
        {
            for(unsigned int j = 1; j < imgLabeling.Col() - 1; j++)
            {
                if(idLabeling[i][j] == colorID)
                {
                    imgLabeling[i][j].r = ColorR;
                    imgLabeling[i][j].g = ColorG;
                    imgLabeling[i][j].b = ColorB;
                    check = true;
                }
            }
        }
        if(check == true)
            index++;
    }

    return index;
}


/// --- HW04 --- ///
KImageGray HW::BinaryExtractionExternal(KImageGray imgGray, KImageGray imgResult)
{
    //Boundary Extraction
    for(unsigned int i = 0; i < imgGray.Row(); i++)
    {
        for(unsigned int j = 0;j < imgGray.Col(); j++)
        {
            if(imgResult[i][j] == 0)
                continue;
            else if(imgGray[i][j])
                imgResult[i][j] = 0;
        }
    }

    return imgResult;
}

KImageGray HW::BinaryExtractionInternal(KImageGray imgGray, KImageGray imgResult)
{
    //Boundary Extraction
    for(unsigned int i = 0; i < imgGray.Row(); i++)
    {
        for(unsigned int j = 0;j < imgGray.Col(); j++)
        {
            if(imgGray[i][j] == 0)
                continue;
            else if(imgResult[i][j])
                imgGray[i][j] = 0;
        }
    }

    return imgGray;
}

KImageGray HW::BinaryDilation_N4(KImageGray imgGray, const int& nType, int kernel)
{
    KImageGray igOut = imgGray;
    int nHalf = kernel/2;

    if(nType == _FOREGROUND)
    {
        for(unsigned int i = nHalf; i < igOut.Row() - nHalf; i++)
            for(unsigned int j = nHalf; j < igOut.Col() - nHalf; j++)
            {
                if(imgGray[i][j] == 0)
                {
                    // Mask
                    if(imgGray[i-1][j]) // 상
                    {
                        igOut[i][j] = 255;
                        continue;
                    }
                    if(imgGray[i+1][j]) // 하
                    {
                        igOut[i][j] = 255;
                        continue;
                    }
                    if(imgGray[i][j-1]) // 좌
                    {
                        igOut[i][j] = 255;
                        continue;
                    }
                    if(imgGray[i][j+1]) // 우
                    {
                        igOut[i][j] = 255;
                        continue;
                    }
                }
            }
    }

    return igOut;
}

KImageGray HW::BinaryErosion_N4(KImageGray imgGray, const int& nType, int kernel)
{
    KImageGray igOut = imgGray;
    int nHalf = kernel/2;

    if(nType == _FOREGROUND)
    {
        for(unsigned int i = nHalf; i < igOut.Row() - nHalf; i++)
            for(unsigned int j = nHalf; j < igOut.Col() - nHalf; j++)
            {
                if(imgGray[i][j] != 0)
                {
                    // Mask
                    if(!imgGray[i-1][j]) // 상
                    {
                        igOut[i][j] = 0;
                        continue;
                    }
                    if(!imgGray[i+1][j]) // 하
                    {
                        igOut[i][j] = 0;
                        continue;
                    }
                    if(!imgGray[i][j-1]) // 좌
                    {
                        igOut[i][j] = 0;
                        continue;
                    }
                    if(!imgGray[i][j+1]) // 우
                    {
                        igOut[i][j] = 0;
                        continue;
                    }
                }
            }
    }

    return igOut;
}

/// --- HW05 --- ///
void HW::Histogram(KImageColor &imgRGB, vector<double> &histoR, vector<double> &histoG, vector<double> &histoB)
{
    splitRGB(imgRGB);

    for(unsigned int i = 0; i < imgRGB.Row(); i++)
    {
        for(unsigned int j = 0;j < imgRGB.Col(); j++)
        {
            histoR[imgRGB[i][j].r]++;
            histoG[imgRGB[i][j].g]++;
            histoB[imgRGB[i][j].b]++;
        }
    }
}
void HW::accumulHisto(KImageColor &imgRGB, vector<double> &histoR, vector<double> &histoG, vector<double> &histoB)
{
    // Accumulated Histogram
    for(int i = 1; i < 256; i++)
    {
        histoR[i] += histoR[i-1];
        histoG[i] += histoG[i-1];
        histoB[i] += histoB[i-1];
    }

    for(int i = 0; i < 256; i++)
    {
        histoR[i] = histoR[i] / (double)(imgRGB.Size());
        histoG[i] = histoG[i] / (double)(imgRGB.Size());
        histoB[i] = histoB[i] / (double)(imgRGB.Size());
    }
}

void HW::HEQ(KImageColor &imgRGB, vector<double> &HEQ_R, vector<double> &HEQ_G, vector<double> &HEQ_B)
{
    accumulHisto(imgRGB, HEQ_R, HEQ_G, HEQ_B);

    // Histogram Equalization
    for(unsigned int i = 0; i < imgRGB.Row(); i++)
    {
        for(unsigned int j = 0; j < imgRGB.Col(); j++)
        {
            imgRGB[i][j].r = HEQ_R[imgRGB[i][j].r] * 255.0;
            imgRGB[i][j].g = HEQ_G[imgRGB[i][j].g] * 255.0;
            imgRGB[i][j].b = HEQ_B[imgRGB[i][j].b] * 255.0;
        }
    }
}

void HW::Histogram_Matching(KImageColor &imgResult, KImageColor imgSrc, vector<double> histoTar, vector<double> &histoSrc, int ch)
{
    int index = 0;
    int tmpArr[256] = {0,};

    for(int i = 0; i < 256; i++)
    {
        double argMin = 1.0;
        double Ysrc = histoSrc[i];

        if(index < 256)
        {
            for(int j = index; j < 256; j++)
            {
                double Ytar = histoTar[j];
                double dMin = abs(Ysrc - Ytar);
                if(dMin < argMin)
                {
                    argMin = dMin;
                    index = j;
                }
            }
        }

        tmpArr[i] = index;
    }

    for(unsigned int i = 0; i < imgSrc.Row(); i++)
    {
        for(unsigned int j = 0; j < imgSrc.Col(); j++)
        {
            if(ch == 1)
                imgResult[i][j].r = (unsigned char)tmpArr[imgSrc[i][j].r];
            else if(ch == 2)
                imgResult[i][j].g = (unsigned char)tmpArr[imgSrc[i][j].g];
            else if(ch == 3)
                imgResult[i][j].b = (unsigned char)tmpArr[imgSrc[i][j].b];
        }
    }

}

/// --- HW06 --- ///
vector<KEDGEPIXEL> HW::CannyEdge(double dSigma, double dLow,double dHigh,const KImageGray& igIn, KImageGray& igOut)
{
    vector<KEDGEPIXEL> v_edgePixel;
    v_edgePixel.clear();

    // Set Dimension of Image
    int nWidth, nHeight;     //image size
    nWidth = igIn.Col();
    nHeight = igIn.Row();

    //init. mymories
    KImageDouble    idMag, idBuf;         //buffer for edge gradients and magnitudes
    KImageWord      iwAng;                //0~360
    KImageGray      igDir;                //0~3

    idMag.Create(nHeight,nWidth);
    iwAng.Create(nHeight,nWidth);
    igDir.Create(nHeight,nWidth);
    idBuf.Create(nHeight,nWidth);

    //compute magnitude and direction of edge
    int     i, j, r, c, rr, cc;
    double  dGradX, dGradY, dTmp;

    //dimension of convolution mask
    int nHalf = ( 3.0 * dSigma < 2.5 ? 2 : (int)(3.0 * dSigma + 0.5) );
    int nMaskLength = nHalf * 2 + 1;

    //create the conv. mask
    KMatrix mKernelX, mKernelY; //convolution mask
    mKernelX.Create(nMaskLength,nMaskLength);
    mKernelY.Create(nMaskLength,nMaskLength);

    //compute the mask
    int    ii,jj;
    double dScale = 0.0, dSigma2 = 2.0 * _SQR(dSigma);

    for(i = -nHalf, ii = 0; i <= nHalf; i++, ii++)
    {
        dTmp = -i * exp(-(i*i)/2./dSigma/dSigma);
        for(j = -nHalf, jj = 0; j <= nHalf; j++, jj++)
        {
            mKernelY[ii][jj] = dTmp * exp(-(j*j)/dSigma2);
            mKernelX[jj][ii] = mKernelY[ii][jj];
            dScale += (i < 0 ? mKernelY[ii][jj] : 0.0);
        }
    }

    for(i = 0; i < nMaskLength; i++)
    {
        for(j = 0; j < nMaskLength; j++)
        {
            mKernelY[i][j] /= -dScale;
            mKernelX[i][j] /= -dScale;
        }
    }

    for(i = nHalf; i < nHeight - nHalf; i++)
    {
        for(j = nHalf; j < nWidth - nHalf; j++)
        {
            //convolution
            dGradX = dGradY = 0.0;
            for(r = -nHalf, rr=0; r <= nHalf; r++,rr++)
                for(c = -nHalf, cc=0; c <= nHalf; c++,cc++){
                    dGradX += igIn._ppA[i+r][j+c] * mKernelX._ppA[rr][cc];
                    dGradY += igIn._ppA[i+r][j+c] * mKernelY._ppA[rr][cc];
                }

            //magnitude
            idMag._ppA[i][j] = _ABS(dGradX) + _ABS(dGradY);

            //direction
            if(idMag[i][j] > dLow)
            {
                dTmp 		   		=  (atan2(dGradY, dGradX) + _PI) * 180.0 / _PI ;
                iwAng._ppA[i][j] = (unsigned short)(dTmp + 0.5);               //0~360
                igDir._ppA[i][j] = (unsigned char)((((int)(dTmp / 22.5) + 1) >> 1) & 0x00000003); //0~3
            }
            else
                idMag[i][j] = 0.0;
        }
    }

    //non-maxima suppression
    int         nShiftX[4] = {-1, 1, 0, -1};
    int         nShiftY[4] = {0, 1, 1, 1};
    int         nH = nHeight - nHalf - 1, nW = nWidth - nHalf - 1;
    KEDGEPIXEL oEdgePixel;

    for(i = nHalf + 1; i < nH; i++)
    {
        for(j = nHalf + 1; j < nW; j++)
        {
            if(idMag[i][j] == 0.0)
                continue;

            if(idMag._ppA[i][j] > idMag._ppA[ i + nShiftY[igDir._ppA[i][j]] ][ j + nShiftX[igDir._ppA[i][j]] ] && idMag._ppA[i][j] > idMag._ppA[ i - nShiftY[igDir._ppA[i][j]] ][ j - nShiftX[igDir._ppA[i][j]] ])
            {
                if(idMag._ppA[i][j] > dHigh)
                {
                    oEdgePixel.u    = (unsigned short)j;
                    oEdgePixel.v 	  = (unsigned short)i;
                    oEdgePixel.wAng = iwAng._ppA[i][j];
                    oEdgePixel.wDir = igDir._ppA[i][j];
                    oEdgePixel.dMag = idMag._ppA[i][j];
                    v_edgePixel.push_back(oEdgePixel);
                }
                idBuf._ppA[i][j] = idMag._ppA[i][j];
            }
        }
    }

    //hysteresis thresholding
    vector<KEDGEPIXEL> tmpEdgePixel;
    while(v_edgePixel.empty() == false)
    {
        //pop out an edge pixel
        oEdgePixel = v_edgePixel.back();
        v_edgePixel.pop_back();

        //store the edge pixel
        tmpEdgePixel.push_back(oEdgePixel);

        //get edge coordinate
        KRect rcArea;
        int jx  = oEdgePixel.u - rcArea._nLeft;
        int iy  = oEdgePixel.v - rcArea._nTop;

        //search neighbor edges
        for(i = -1; i < 2; i++)
        {
            for(j = -1; j < 2; j++)
            {
                if(idBuf._ppA[iy + i][jx + j] && idBuf._ppA[iy + i][jx + j] <= dHigh)
                {
                    oEdgePixel.u    = (unsigned short)(jx + j + rcArea._nLeft);
                    oEdgePixel.v    = (unsigned short)(iy + i + rcArea._nTop);
                    oEdgePixel.wAng = iwAng._ppA[iy + i][jx + j];
                    oEdgePixel.wDir = igDir._ppA[iy + i][jx + j];
                    oEdgePixel.dMag = idBuf._ppA[iy + i][jx + j];

                    v_edgePixel.push_back(oEdgePixel);
                    idBuf._ppA[iy + i][jx + j] = 0.0;
                }
            }
        }
    }

    //output image
    igOut.Create(nHeight, nWidth);
    for(auto& element : tmpEdgePixel)
    {
        oEdgePixel = element;
        igOut._ppA[oEdgePixel.v][oEdgePixel.u] = 255;
    }

    return tmpEdgePixel;
}

KCircle HW::CircleHoughTransform(CIRCLEHOUGH_INFO& circleInfo, vector<KEDGEPIXEL> edge)
{
    // Init
    CIRCLEHOUGH_INFO oInfo = circleInfo;
    vector<vector<vector<double>>> lllVotes;
    double dSx, dSy, dSr;

    // Create Voting Table
    lllVotes.resize(oInfo.nNumGridCy);
    for(auto& itemY : lllVotes)
    {
        itemY.resize(oInfo.nNumGridCx);
        for(auto& itemX : itemY)
            itemX.resize(oInfo.nNumGridR);
    }

    //equal ratio for quantization of center positon and radius
    dSx = (double)(oInfo.nRight - oInfo.nLeft) / (double)(oInfo.nNumGridCx - 1); //Cx = nLeft  + k*dSx
    dSy = (double)(oInfo.nBottom - oInfo.nTop) / (double)(oInfo.nNumGridCy - 1); //Cy = nTop   + k*dSy
    dSr = (double)(oInfo.nMaxR  - oInfo.nMinR) / (double)(oInfo.nNumGridR - 1); //R  = nMinR  + k*dSr

    // Run
    //Hough Transform
    int			nGridCx, nGridCy, nGridR;
    double		dRadius;
    double		dAngle;
    KPoint		ptCen;
    KCircle*	ccpFrom = (KCircle*)0;
    KCircle*	ccpTo   = (KCircle*)0;

    //Hough Transform
    for(auto& oEdgePxl : edge)
    {
        //check if the edge is in effective region
        if(ccpTo && !ccpTo->InCircle((double)oEdgePxl.u, (double)oEdgePxl.v))
            continue;
        if(ccpFrom && ccpFrom->InCircle((double)oEdgePxl.u, (double)oEdgePxl.v))
            continue;

        //votes for radius
        nGridR 	= 1;
        dRadius = (double)oInfo.nMinR;
        do{
            //compute circle centers for voting
            dAngle 	  =	_RADIAN(oEdgePxl.wAng);

            if(oInfo.nDirEdge == _OUTWARD_EDGE)
            {
                ptCen._dX = oEdgePxl.u - dRadius*cos(dAngle);
                ptCen._dY = oEdgePxl.v - dRadius*sin(dAngle);
            }
            else //_INWARD_EDGE
            {
                ptCen._dX = oEdgePxl.u + dRadius*cos(dAngle);
                ptCen._dY = oEdgePxl.v + dRadius*sin(dAngle);
            }

            //compute grid coordinates
            nGridCx	  = (int)( (ptCen._dX - (double)oInfo.nLeft)/dSx );
            if(nGridCx < 2 || nGridCx > oInfo.nNumGridCx - 3)
                continue;
            nGridCy	  = (int)( (ptCen._dY - (double)oInfo.nTop)/dSy );
            if(nGridCy < 2 || nGridCy > oInfo.nNumGridCy - 3)
                continue;

            //votes to avoid edge noises
            for(int i = -2; i < 3; i++)
                for(int j = -2; j < 3; j++)
                {
                    lllVotes[nGridCy + i][nGridCx + j][nGridR - 1] += 0.7;
                    lllVotes[nGridCy + i][nGridCx + j][nGridR]     += 0.7;
                    lllVotes[nGridCy + i][nGridCx + j][nGridR + 1] += 0.7;
                }
            lllVotes[nGridCy][nGridCx][nGridR] += 0.3;

            //increment
            dRadius += dSr;

        }while(++nGridR < oInfo.nNumGridR - 1);
    }

    //select peaks
    int	nGmax = 0;
    for(int i = 0; i < oInfo.nNumGridCy; i++)
    {
        for(int j = 0; j < oInfo.nNumGridCx; j++)
        {
            for(int k = 0; k < oInfo.nNumGridR; k++)
                if(lllVotes[i][j][k] > nGmax)
                {
                    nGmax 	= (int)(lllVotes[i][j][k]);
                    nGridCx	= j;
                    nGridCy	= i;
                    nGridR	= k;
                }
        }
    }

    int nThresh = (oInfo.nThreshVote == 0 ? _PI*(oInfo.nMinR + nGridR*dSr)*0.5 : oInfo.nThreshVote);
    if(nGmax < nThresh)
        return oDetected;

    //중심 및 반지름의 그리드 값으로 부터 실제 픽셀 단위로 환산하여 반환한다
    oDetected.Create(oInfo.nLeft + nGridCx*dSx, oInfo.nTop + nGridCy*dSy, oInfo.nMinR + nGridR*dSr);
    return oDetected;
}

vector<KCircle*> HW::CircleHoughTransform_Multi(CIRCLEHOUGH_INFO& circleInfo, vector<KEDGEPIXEL> edge)
{
    // Init
    CIRCLEHOUGH_INFO oInfo = circleInfo;
    vector<vector<vector<double>>> lllVotes;
    double dSx, dSy, dSr;

    //equal ratio for quantization of center positon and radius
    dSx = (double)(oInfo.nRight - oInfo.nLeft) / (double)(oInfo.nNumGridCx - 1);
    dSy = (double)(oInfo.nBottom- oInfo.nTop)  / (double)(oInfo.nNumGridCy - 1);
    dSr = (double)(oInfo.nMaxR  - oInfo.nMinR) / (double)(oInfo.nNumGridR - 1);

    // Run
    //Hough Transform
    int			nGridCx, nGridCy, nGridR;
    double		dRadius;
    double		dAngle;
    KPoint		ptCen;
    KCircle*	ccpFrom = (KCircle*)0;
    KCircle*	ccpTo   = (KCircle*)0;

    // Create Voting Table
    lllVotes.resize(oInfo.nNumGridCy);
    for(auto& itemY : lllVotes)
    {
        itemY.resize(oInfo.nNumGridCx);
        for(auto& itemX : itemY)
            itemX.resize(oInfo.nNumGridR);
    }

    //Hough Transform
    for(auto& oEdgePxl : edge)
    {
        //check if the edge is in effective region
        if(ccpTo && !ccpTo->InCircle((double)oEdgePxl.u, (double)oEdgePxl.v))
            continue;
        if(ccpFrom && ccpFrom->InCircle((double)oEdgePxl.u, (double)oEdgePxl.v))
            continue;

        //votes for radius
        nGridR 	= 1;
        dRadius = (double)oInfo.nMinR;
        do{
            //compute circle centers for voting
            dAngle 	  =	_RADIAN(oEdgePxl.wAng);

            if(oInfo.nDirEdge == _OUTWARD_EDGE)
            {
                ptCen._dX = oEdgePxl.u - dRadius*cos(dAngle);
                ptCen._dY = oEdgePxl.v - dRadius*sin(dAngle);
            }
            else //_INWARD_EDGE
            {
                ptCen._dX = oEdgePxl.u + dRadius*cos(dAngle);
                ptCen._dY = oEdgePxl.v + dRadius*sin(dAngle);
            }

            //compute grid coordinates
            nGridCx	  = (int)( (ptCen._dX - (double)oInfo.nLeft)/dSx );
            if(nGridCx < 2 || nGridCx > oInfo.nNumGridCx - 3)
                continue;
            nGridCy	  = (int)( (ptCen._dY - (double)oInfo.nTop)/dSy );
            if(nGridCy < 2 || nGridCy > oInfo.nNumGridCy - 3)
                continue;

            //votes to avoid edge noises
            for(int i = -2; i < 3; i++)
                for(int j = -2; j < 3; j++)
                {
                    lllVotes[nGridCy + i][nGridCx + j][nGridR - 1] += 0.7;
                    lllVotes[nGridCy + i][nGridCx + j][nGridR]     += 0.7;
                    lllVotes[nGridCy + i][nGridCx + j][nGridR + 1] += 0.7;
                }
            lllVotes[nGridCy][nGridCx][nGridR] += 0.3;

            //increment
            dRadius += dSr;

        }while(++nGridR < oInfo.nNumGridR - 1);
    }

    //obtain the global max
    double dGmax = 0.0;
    for(int i=1; i < oInfo.nNumGridCy - 1; i++)
        for(int j=1; j < oInfo.nNumGridCx - 1; j++)
            for(int k=1; k < oInfo.nNumGridR - 1; k++)
                dGmax = ( lllVotes[i][j][k] > dGmax ? lllVotes[i][j][k] : dGmax );

    if(dGmax < oInfo.nThreshVote)
        return lDetected;

    //select peaks
    lDetected.clear();
    lVote.clear();

    for(int i = 1, ii = oInfo.nNumGridCy - 1; ii; i++, ii--)
    {
        for(int j = 1, jj = oInfo.nNumGridCx - 1; jj; j++, jj--)
        {
            for(int k = 1, kk = oInfo.nNumGridR - 1; kk; k++, kk--)
                if(lllVotes[i][j][k] > oInfo.nThreshVote)
                {
                    if(lllVotes[i][j][k]/dGmax > 0.5)
                    {
                        lDetected.push_back(new KCircle(oInfo.nLeft + j*dSx, oInfo.nTop + i*dSy, oInfo.nMinR + k*dSr));
                        lVote.push_back(lllVotes[i][j][k]);
                    }
                }
        }
    }

    //merge peaks
    int	   nRef, nTar;
    KCircle ccRef;

    for(int i = 0; i < lVote.size() - 1; i++)
        for(int j = i + 1; j < lVote.size(); j++)
        {
            if(lDetected[i]->_dRadius > lDetected[j]->_dRadius)
            {
                nRef = i; nTar = j;
            }
            else{
                nRef = j; nTar = i;
            }
            ccRef = *lDetected[nRef];
            ccRef._dRadius *= 0.5;

            if(ccRef.InCircle(lDetected[nTar]->Center()))
            {
                if(lVote[i] > lVote[j])
                {
                    lVote.erase(lVote.begin() + j);
                    lDetected.erase(lDetected.begin() + j--);
                }
                else
                {
                    lVote.erase(lVote.begin() + i);
                    lDetected.erase(lDetected.begin() + i--);
                    break;
                }
            }
        }

    //return
    return lDetected;
}

void HW::read_txt(QVector<QPoint>& point)
{
    FILE* fp = freopen("./data/mask.txt", "r", stdin);

    int num; cin >> num;

    if(num == 0)
    {
        cout << "# Cannot open File #" << endl;
    }
    else
    {
        cout << "! open File Success !" << endl;

        for(int i = 0; i < num; i++){
            int x = 0, y = 0;
            cin >> x >> y;
            point.append(QPoint(x, y));
        }
    }

    fclose(fp);
}

void HW::initShapeTable(QPolygon& poly)
{
    for(int i = 0; i < 4; i++)
    {
        int size = vTables[i].size();
        for(int j = 0; j < size; j++)
            delete vTables[i][j];

        vTables[i].clear();
    }

    for (const auto& point : poly)
    {
        center += point;
    }
    center /= poly.size();

    double dTmp, dDist, dAng;
    int nDir;

    for(int j = 1, jj = poly.size() - 2; jj; j++, jj--)
    {
        dTmp = atan2(poly[j+1].y() - poly[j-1].y(), poly[j+1].x() - poly[j-1].x()) * 180 / M_PI; //0~360
        nDir = ((((int)(dTmp/22.5)+1)>>1) & 3);
        dDist = sqrt(_SQR(center.x()-poly[j].x()) + _SQR(center.y()-poly[j].y()));
        dAng = atan2(poly[j].y() - center.y(), poly[j].x() - center.x());

        vTables[nDir].push_back(new KPoint(dDist, dAng));
    }
}

void HW::Generalized_Hough_Transform(KEdge& oEdge, GENERALHOUGH_INFO generalInfo, GENERALHOUGH& detected)
{
    // Init
    gInfo = generalInfo;

    GHTdSx = (double)(gInfo.nRight - gInfo.nLeft) / (double)(gInfo.nNumGridCx - 1);
    GHTdSy = (double)(gInfo.nBottom - gInfo.nTop) / (double)(gInfo.nNumGridCy - 1);
    GHTdSang = (double)(gInfo.nMaxAng - gInfo.nMinAng) / (double)(gInfo.nNumGridAng - 1);
    GHTdSscale  = (double)(gInfo.nMaxScale - gInfo.nMinScale)  / (double)(gInfo.nNumGridScale - 1);

    // Run
    int         nGridCx, nGridCy, nGridAng, nGridScale;
    double		dScale;
    double		dAngle;
    int iDir;
    double dTmp;
    KPoint		ptCen;
    KEdge*      opEdge = &oEdge;

    //voting 초기화
    llllVotes.resize(gInfo.nNumGridCx);
    for(auto& itemAng : llllVotes)
    {
        itemAng.resize(gInfo.nNumGridCy);
        for(auto& itemS : itemAng)
        {
            itemS.resize(gInfo.nNumGridScale);
            for(auto& itemY : itemS)
            {
                itemY.resize(gInfo.nNumGridAng);
            }
        }
    }

    for(auto& oEdgePxl : *opEdge)
    {
        nGridAng = 1;
        dAngle = (double)gInfo.nMinAng;
        do{
            nGridScale = 1;
            dScale = (double)gInfo.nMinScale;

            do{
                dTmp = oEdgePxl.wAng - dAngle;
                iDir = ((((int)(dTmp/22.5) + 1) >> 1) & 3);

                for(int k = 0, kk = vTables[iDir].size(); kk; k++, kk--)
                {
                    ptCen._dX = oEdgePxl.u - vTables[iDir][k]->_dX * dScale * cos(vTables[iDir][k]->_dY + dAngle);
                    ptCen._dY = oEdgePxl.v - vTables[iDir][k]->_dX * dScale * sin(vTables[iDir][k]->_dY + dAngle);

                    //좌표 계산
                    nGridCx	  = (int)( (ptCen._dX - (double)gInfo.nLeft)/GHTdSx );
                    if(nGridCx < 1 || nGridCx > gInfo.nNumGridCx - 2)
                        continue;
                    nGridCy	  = (int)( (ptCen._dY - (double)gInfo.nTop)/GHTdSy );
                    if(nGridCy < 1 || nGridCy > gInfo.nNumGridCy - 2)
                        continue;

                    for(int l = -1; l < 2; l++){
                        for(int m = -1; m < 2; m++){
                            for(int n = -1; n < 2; n++){
                                for(int o = -1; o < 2; o++){
                                    llllVotes[nGridCx+l][nGridCy+m][nGridScale+n][nGridAng+o] += 0.7;
                                }
                            }
                        }
                    }
                    llllVotes[nGridCx][nGridCy][nGridScale][nGridAng] += 0.3;
                }

                dScale += GHTdSscale;
            }while(++nGridScale < gInfo.nNumGridScale - 1);

            dAngle += GHTdSang;
        }while(++nGridAng < gInfo.nNumGridAng - 1);
    }

    cout << "Debug_Start" << endl;

    //최대 voting 선택
    double	dGmax = 0.0;
    for(int i = 0; i < gInfo.nNumGridCx; i++)
    {
        for(int j = 0; j < gInfo.nNumGridCy; j++)
        {
            for(int k = 0; k < gInfo.nNumGridScale - 1; k++)
            {
                for(int l = 0;l < gInfo.nNumGridAng - 1; l++)
                {
                    if(llllVotes[i][j][k][l] > dGmax)
                    {
                        dGmax = (int)(llllVotes[i][j][k][l]);
                        nGridCx = i;
                        nGridCy = j;
                        nGridScale = k;
                        nGridAng = l;
                    }
                }
            }
        }
    }

    int nThresh = gInfo.nThreshVote;

    cout << "Debug_Finish" << endl;

    if(dGmax < nThresh)
        return ;

    detected.cX = gInfo.nLeft + nGridCx * GHTdSx;
    detected.cY = gInfo.nTop + nGridCy * GHTdSy;
    detected.rotation = gInfo.nMinAng + nGridAng * GHTdSang;
    detected.scale = gInfo.nMinScale + nGridScale * GHTdSscale;
}

