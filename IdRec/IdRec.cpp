// IdRec.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "IdPrepare.h"
#include "IdOcr.h"

void test_prepare()
{
    CIdPrepare preproccessor;

    preproccessor.loopPrepare();
}

void test_ocr()
{
    CIdOcr ocr;

    ocr.scanPrepare();
}

void showVersion()
{
    cout << IDREC_APP_NAME << endl;
    cout << "version:" << IDREC_APP_VERSION << endl;
}

void showUsage()
{
    cerr << "参数错误" << endl;
    cerr << "usage:" << endl;
    cerr << IDREC_APP_NAME << " -prepare --- 图像预处理模式" << endl;
    cerr << IDREC_APP_NAME << " -ocr     --- 图像OCR识别模式" << endl;
    system("pause");
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        showUsage();
        system("pause");
        return -1;
    }
    
    //check mode
    if (0 == strcmp(argv[1], "-prepare")) {
        cout << "当前模式: 图像预处理模式" << endl;
        cout << "excute: " << argv[0] << " " << argv[1] << endl;
        test_prepare();
    }
    else if (0 == strcmp(argv[1], "-ocr")) {
        cout << "当前模式: 图像OCR识别模式" << endl;
        cout << "excute: " << argv[0] << " " << argv[1] << endl;
        test_ocr();
    }
    else{
        cout << "excute: " << argv[0] << " " << argv[1] << endl;
        showUsage();
    }

    system("pause");
	return 0;
}

