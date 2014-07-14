#pragma once

class CIdOcr
{
public:
    CIdOcr();
    ~CIdOcr();

    void scanPrepare();

    bool ocrPrepare(string szFile, string &szResult);
    bool filterId(string szResult, string &szId);
    bool ocrAnalysis(string szFile, vector<Rect> &lstsquares);
    bool checkIdRect(Rect rect);
    bool ocrProcess(string szFile);
    bool cutIdRect(string szFile, Rect rect);
};

