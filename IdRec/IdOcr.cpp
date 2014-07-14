#include "stdafx.h"
#include "IdOcr.h"
#include "strngs.h"
#include "baseapi.h"
#include "Utility.h"

using namespace std;

#pragma comment(lib, "D:\\Project\\Common\\tesseract-3.02.02\\lib\\libtesseract302.lib")
#pragma comment(lib, "D:\\Project\\Common\\tesseract-3.02.02\\lib\\liblept168.lib")

CIdOcr::CIdOcr()
{
}

CIdOcr::~CIdOcr()
{
}


void CIdOcr::scanPrepare()
{
    // load raw data 
    string szRawPath = IDREC_DATA_PREPARE_PATH;
    string szRawExt = "*.jpg";
    vector<string> files;

    CUtility::scanFiles(szRawPath, szRawExt, files);

    for (int i = 0; i < files.size(); ++i) {
        ocrProcess(files[i]);
    }
}

bool CIdOcr::ocrPrepare(string szFile, string &szResult)
{
    tesseract::TessBaseAPI api;
    if (api.Init("D:\\Project\\ocr\\IdRec\\bin\\tessdata", "eng", tesseract::OEM_DEFAULT)){ // 初始化，设置语言包，中文简体： chi_sim; 英文：eng ；也可以自己训练语言包
        std::cout << "[CIdOcr::ocrPrepare] fail to init" << endl;
        return false;
    }

    api.SetVariable("tessedit_char_whitelist", "0123456789X");
    STRING text_out; // 定义输出的变量
    if  (!api.ProcessPages(szFile.c_str(), NULL, 0, &text_out)) {
        std::cout << "[CIdOcr::ocrPrepare] fail to ProcessPages for " << szFile << endl;
        return false;
    }

    szResult = text_out.string();
    return true;
}

bool CIdOcr::filterId(string szResult, string &szId)
{
    int nPos = 0;
    for (int i = 0; i < szResult.length(); ++i){
        char ch =szResult[i];
        if ((ch >= '0' && ch <= '9') || ch == 'X'){
            szId += ch;
            ++nPos;
        }
        else{
            if (nPos == IDREC_ID_NORMAL_SIZE){
                return true;
            }

            szId.clear();
            nPos = 0;
        }
    }

    return false;
}

bool CIdOcr::ocrAnalysis(string szFile, vector<Rect> &lstsquares)
{
    // draw retangle of text
    cout << "[CIdPrepare::ocrAnalysis] load a file " << szFile << endl;
    Mat image = imread(szFile.c_str(), CV_LOAD_IMAGE_GRAYSCALE); // CV_LOAD_IMAGE_GRAYSCALE);
    assert(image.rows != 0);

    tesseract::TessBaseAPI api;
    if (api.Init("D:\\Project\\ocr\\IdRec\\bin\\tessdata", "eng", tesseract::OEM_DEFAULT)){ // 初始化，设置语言包，中文简体： chi_sim; 英文：eng ；也可以自己训练语言包
        std::cout << "[CIdOcr::ocrAnalysis] fail to init" << endl;
        return false;
    }

    api.SetInputName(szFile.c_str());
    api.SetPageSegMode(tesseract::PSM_AUTO_OSD);

    api.SetImage((uchar*)image.data, image.cols, image.rows, 1, image.cols);
    tesseract::PageIterator* it = api.AnalyseLayout();
    if (it == NULL){
        std::cout << "[CIdOcr::ocrAnalysis] fail to AnalyseLayout for " << szFile << endl;
        return false;
    }

    RNG rng(12345);
    while (it->Next(tesseract::RIL_WORD)){
        int top, bottom, left, right;
        it->BoundingBox(tesseract::RIL_WORD, &left, &top, &right, &bottom);

        // check rect rate
        Rect rect(Point(left, top), Point(right, bottom));

        if (checkIdRect(rect)){
            Point squares[] = { Point(left, top), Point(right, top), Point(right, bottom), Point(left, bottom) };
            const Point* p = &squares[0];

            int n = 4;
            Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
            polylines(image, &p, &n, 1, true, color, 1, CV_AA); // Scalar(0, 255, 0)
            lstsquares.push_back(rect);
        }
    }

    CUtility::dumpFile(szFile, image, "ocr_", IDREC_DATA_OCR_PATH);
    return true;
}


bool CIdOcr::checkIdRect(Rect rect)
{
    if (rect.height > 0){
        int nRate = rect.width / rect.height;
        if (nRate >= 5 && nRate < 25){
            return true;
        }
        else if (nRate > 4){
            return true;
        }
    }

    return false;
}


bool CIdOcr::ocrProcess(string szFile)
{
    string szResult;
    vector<Rect> lstsquares;

    if (!ocrAnalysis(szFile, lstsquares)){
        return false;
    }

    for (int i = 0; i < lstsquares.size(); ++i){
        if (cutIdRect(szFile, lstsquares[i])){
            break;
        }
    }


    //if (ocrPrepare(szFile, szResult)){
    //    std::cout << szFile << " ocr result: " << endl;
    //    std::cout << szResult << endl;// 输出到命令行中
    //    std::cout << "---------------------------------------------------------" << endl;

    //    // filter id
    //    string szId;
    //    if (filterId(szResult, szId)){
    //        std::cout << "[CIdOcr::scanPrepare]" << files[i] << " get id:" << szId << endl;
    //        std::cout << "---------------------------------------------------------" << endl;
    //    }
    //}

    return false;
}

bool CIdOcr::cutIdRect(string szFile, Rect rect)
{
    cout << "[CIdPrepare::cutIdRect] load a file " << szFile << endl;
    Mat image = imread(szFile.c_str(), CV_LOAD_IMAGE_GRAYSCALE); // CV_LOAD_IMAGE_GRAYSCALE);
    assert(image.rows != 0);

    Mat cutImage = image(rect);
    Mat scaleImage = CUtility::scaleImage(cutImage);

    tesseract::TessBaseAPI api;
    if (api.Init("D:\\Project\\ocr\\IdRec\\bin\\tessdata", "eng", tesseract::OEM_DEFAULT)){ // 初始化，设置语言包，中文简体： chi_sim; 英文：eng ；也可以自己训练语言包
        std::cout << "[CIdOcr::ocrAnalysis] fail to init" << endl;
        return false;
    }

    api.SetInputName(szFile.c_str());
    api.SetPageSegMode(tesseract::PSM_AUTO_OSD);
    api.SetImage((uchar*)scaleImage.data, scaleImage.cols, scaleImage.rows, 1, scaleImage.cols);
    api.SetVariable("tessedit_char_whitelist", "0123456789X");

    string szResult = api.GetUTF8Text();
    cout << "[CIdOcr::cutIdRect] " << szFile << " ocr result:" << szResult << endl;

    // filter id
    string szId;
    if (filterId(szResult, szId)){
        std::cout << "[CIdOcr::cutIdRect]" << szFile << " get id:" << szId << endl;
        CUtility::dumpFile(szFile, scaleImage, "cut_", IDREC_DATA_OCR_PATH);
        return true;
    }

    static int nIndex = 0;
    char szBuf[16] = { 0, };
    char *szIndex = _itoa(++nIndex, szBuf, 10);
    CUtility::dumpFile(szFile, scaleImage, szIndex, IDREC_DATA_OCR_PATH);
    return false;
}
