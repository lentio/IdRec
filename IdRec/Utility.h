#pragma once
class CUtility
{
public:
    CUtility();
    ~CUtility();

    // filesystem
    static string getCwd();
    static bool scanFiles(string szPath, string szFileExt, vector<string> &files);
    static bool parserPathFile(string szPathFile, string &szFilePath, string &szFileName);

    // opencv
    static Mat normalSize(Mat img);
    static bool dumpFile(string szFile, Mat image, string szPrefix, string szOutPath);
    static Mat scaleImage(Mat image);
};

