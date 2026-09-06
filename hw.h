#ifndef HW_H
#define HW_H

#include "kfc.h"
#include <ctime>
#include "imageform.h"
#include <iostream>
#include "edge.h"
#include "qpolygon.h"

using namespace std;

struct CIRCLEHOUGH_INFO
{
    int nLeft;
    int nRight;
    int nTop;
    int nBottom;

    int nMinR;
    int nMaxR;

    int nNumGridCx;
    int nNumGridCy;
    int nNumGridR;
    int nDirEdge; //_OUTWARD 0
                  //_INWARD 1
    int nThreshVote;
};

struct GENERALHOUGH_INFO
{
    int nLeft;
    int nRight;
    int nTop;
    int nBottom;

    double nMinAng;
    double nMaxAng;
    double nMinScale;
    double nMaxScale;

    int nNumGridCx;
    int nNumGridCy;
    int nNumGridAng;
    int nNumGridScale;

    int nThreshVote;
};

struct GENERALHOUGH
{
    int cX, cY;
    double rotation;
    double scale;
};

class HW
{
public:
    HW();

    // HW1
    KArray<KHSI> RGB2HSI(KImageColor igRGB);
    KArray<KTRIPLETF> RGB2NRG(KImageColor igRGB);

    // HW2
    KImageColor ContrastTransform_RGB(KImageColor igRGB, const int &nA, const int &nB, const int &nAp, const int &nBp);

    // HW3
    KImageGray BinaryDilation(KImageGray imgGray, const int &nType, int kernel);
    KImageGray BinaryErosion(KImageGray imgGray, const int &nType, int kernel);
    KImageColor Labeling_Neighbor4(KImageGray imgGray, int kernel, int &index);
    KImageColor Labeling_Neighbor8(KImageGray imgGray, int kernel, int &index);

    // HW4
    KImageGray BinaryExtractionExternal(KImageGray imgGray, KImageGray imgResult);
    KImageGray BinaryExtractionInternal(KImageGray imgGray, KImageGray imgResult);
    KImageGray BinaryDilation_N4(KImageGray imgGray, const int &nType, int kernel);
    KImageGray BinaryErosion_N4(KImageGray imgGray, const int &nType, int kernel);

    // HW5
    void Histogram(KImageColor &imgRGB, vector<double> &histoR, vector<double> &histoG, vector<double> &histoB);
    void accumulHisto(KImageColor &imgRGB, vector<double> &histoR, vector<double> &histoG, vector<double> &histoB);
    void HEQ(KImageColor &imgRGB, vector<double> &HEQ_R, vector<double> &HEQ_G, vector<double> &HEQ_B);
    void Histogram_Matching(KImageColor &imgResult, KImageColor imgSrc, vector<double> histoTar, vector<double> &histoSrc, int ch);

    // HW6
    KCircle oDetected;           // CircleHT
    vector<KCircle *> lDetected; // CircleHT_Multi
    vector<double> lVote;        // CircleHT_Multi

    GENERALHOUGH_INFO gInfo;
    vector<vector<vector<vector<double>>>> llllVotes;
    double GHTdSx, GHTdSy, GHTdSang, GHTdSscale;
    GENERALHOUGH gDetected;      // GHT
    vector<KPoint *> vTables[4]; // GHT
    QPoint center;

    vector<KEDGEPIXEL> CannyEdge(double dSigma, double dLow, double dHigh, const KImageGray &igIn, KImageGray &igOut);
    KCircle CircleHoughTransform(CIRCLEHOUGH_INFO &circleInfo, vector<KEDGEPIXEL> edge);
    vector<KCircle *> CircleHoughTransform_Multi(CIRCLEHOUGH_INFO &circleInfo, vector<KEDGEPIXEL> edge);

    void initShapeTable(QPolygon &poly);
    void read_txt(QVector<QPoint> &point);
    void Generalized_Hough_Transform(KEdge &oEdge, GENERALHOUGH_INFO generalInfo, GENERALHOUGH &detected);

private:
    double deg2rad(double deg);
    double rad2deg(double rad);
    int randomColor(KImageWord idLabeling, int id, KImageColor &imgLabeling);
    void splitRGB(KImageColor &imgRGB);
};

#endif // HW_H
