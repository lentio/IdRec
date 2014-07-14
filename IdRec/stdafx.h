// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS 
#endif

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <Windows.h>
#include <stdarg.h>
#include <io.h>
#include <direct.h>
#include <conio.h>

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

// OpenCV头文件
#include <opencv2/video/tracking.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/nonfree/nonfree.hpp>  // SURF is nonfree

// 声明命名空间
// using namespace std;
using namespace cv;
using std::string;
using std::cout;
using std::endl;
using std::cerr;

// TODO: reference additional headers your program requires here
#include "Utility.h"

#define IDREC_APP_NAME              "IdRec"
#define IDREC_APP_VERSION           "0.0.1"

#define IDREC_DATA_INPUT_PATH       "./data/id"
#define IDREC_DATA_PREPARE_PATH     "./data/prepare"
#define IDREC_DATA_OCR_PATH         "./data/ocr"

#define IDREC_IMAGE_NORMAL_ROWS     400
#define IDREC_IMAGE_NORMAL_COLS     400

#define IDREC_ID_NORMAL_SIZE        18

