#include "stdafx.h"
#include "Utility.h"


CUtility::CUtility()
{
}


CUtility::~CUtility()
{
}

// filesystem
string CUtility::getCwd()
{
    char *buffer;
    if ((buffer = _getcwd(NULL, 0)) == NULL){
        assert(0);
    }

    return buffer;
}

bool CUtility::scanFiles(string szPath, string szFileExt, vector<string> &files)
{
    // loop all image files from the path
    _finddata_t fileDir;
    string searchDir = szPath + "/" + szFileExt;
    long lfDir;

    string szPwd = getCwd();

    if ((lfDir = _findfirst(searchDir.c_str(), &fileDir)) == -1l) {
        cout << "[CUtility::scanFiles] " << searchDir << ": No file is found at " << szPath << endl;
        return false;
    }
    else {
        //cout << "file list:\n";
        do{
            //cout << fileDir.name << endl;
            string szFile = szPath + "/" + fileDir.name;
            files.push_back(szFile);
        } while (_findnext(lfDir, &fileDir) == 0);
    }

    _findclose(lfDir);
    cout << "[CUtility::scanFiles] " << files.size() << " files are found at " << szPath << endl;
    return true;
}

bool CUtility::parserPathFile(string szPathFile, string &szFilePath, string &szFileName)
{
    assert(szPathFile.size() > 0);
    for (int i = 0; i < szPathFile.size(); ++i){
        if (szPathFile[i] == '\\') {
            szPathFile[i] = '/';
        }
    }

    int nPos = szPathFile.rfind('/');
    if (nPos < 0){ // no found path, only return file name;
        szFileName = szPathFile;
        szFilePath = "";
    }
    else if (nPos == szPathFile.size()){ // at last, only return path
        szPathFile = szPathFile.substr(0, szPathFile.size() - 1);
        szFileName = "";
    }
    else{
        szFileName = szPathFile.substr(nPos + 1, szPathFile.size());
        szFilePath = szPathFile.substr(0, nPos);
    }

    return true;
}

Mat CUtility::normalSize(Mat img)
{
    // scale into ...
    Size dsize = Size(IDREC_IMAGE_NORMAL_ROWS, IDREC_IMAGE_NORMAL_COLS);
    Mat scaleImage = Mat(dsize, CV_32S);
    resize(img, scaleImage, dsize);
    return scaleImage;
}


bool CUtility::dumpFile(string szFile, Mat image, string szPrefix, string szOutPath)
{
    string szPath, szName;
    CUtility::parserPathFile(szFile, szPath, szName);

    string szOutPathFile = szOutPath;
    szOutPathFile = szOutPathFile + "/" + szPrefix + szName;

    if (!imwrite(szOutPathFile, image)) {
        cout << "[CUtility::dumpFile] fail to save file " << szOutPathFile << endl;
        return false;
    }

    cout << "[CUtility::dumpFile] save file to " << szOutPathFile << endl;
    return true;
}

Mat CUtility::scaleImage(Mat image)
{
    // scale into double
    Size dsize = Size(image.cols*2, image.rows*2);
    Mat scaleImage = Mat(dsize, CV_32S);
    resize(image, scaleImage, dsize);
    return scaleImage;
}
