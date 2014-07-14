#include "stdafx.h"
#include "IdPrepare.h"

int thresh = 50, N = 11;

CIdPrepare::CIdPrepare()
{
}


CIdPrepare::~CIdPrepare()
{
}

void CIdPrepare::loopPrepare()
{
    // load raw data 
    string szRawPath = IDREC_DATA_INPUT_PATH;
    string szRawExt = "*.jpg";
    vector<string> files;

    CUtility::scanFiles(szRawPath, szRawExt, files);

    for (int i = 0; i < files.size(); ++i) {
        singlePrepare(files[i]);
    }
}

bool CIdPrepare::singlePrepare(string szFile)
{
    cout << "[CIdPrepare::singlePrepare] load a file " << szFile << endl;
    Mat image = imread(szFile.c_str(), 1); // CV_LOAD_IMAGE_GRAYSCALE);
    assert(image.rows != 0);

    // split R channel
    //vector<Mat> mv;
    //Mat rImage;
    //split(image, mv);
    //rImage = mv[0];

    // find square
    vector<vector<Point> > squares;
    findSquares(image, squares);
    drawSquares(image, squares);
    CUtility::dumpFile(szFile, image, "squar_", IDREC_DATA_PREPARE_PATH);

    Rect rect;
    if (getSquares(image, squares, rect)){
        Mat imageSquares = image(rect);
        CUtility::dumpFile(szFile, imageSquares, "rect_", IDREC_DATA_PREPARE_PATH);

        // 转为二值图  
        Mat binImage;
        threshold(imageSquares, binImage, 100, 255, CV_THRESH_BINARY);
        CUtility::dumpFile(szFile, binImage, "bin_", IDREC_DATA_PREPARE_PATH);
    }
        
    // 转为二值图  
    //Mat binImage;
    //threshold(image, binImage, 150, 255, CV_THRESH_BINARY);
    //CUtility::dumpFile(szFile, binImage, "bin_", IDREC_DATA_PREPARE_PATH);

    // contour analysis
    //Mat cannyImage;
    //prepareCanny(image, cannyImage);
    //CUtility::dumpFile(szFile, cannyImage, "canny_",, IDREC_DATA_PREPARE_PATH);

    // scale size
    //Mat scaleImage = CUtility::normalSize(cannyImage);
    //equalizeHist(scaleImage, scaleImage);
    //CUtility::dumpFile(szFile, scaleImage, "prepare_", IDREC_DATA_PREPARE_PATH);
    return true;
}

bool CIdPrepare::prepareCanny(Mat srcImage, Mat &dstImage)
{
    int lowThreshold = 30; // 0,50, 100
    int ratio = 3;
    int kernel_size = 3;
    RNG rng(12345);

    dstImage.create(srcImage.size(), srcImage.type());

    /// Reduce noise with a kernel 3x3  
    Mat blurImage;
    // blur(srcImage, dstImage, Size(3, 3));
    GaussianBlur(srcImage, blurImage, Size(3, 3), 0, 0, BORDER_DEFAULT);

    /// Canny detector  
    Mat cannyImage;
    Canny(blurImage, cannyImage, lowThreshold, lowThreshold*ratio, kernel_size);

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    /// Detect edges using canny
    // Canny(src_gray, canny_output, thresh, thresh * 2, 3);
    /// Find contours
    findContours(cannyImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0)); // CV_CHAIN_CODE

    /// Draw contours
    Mat drawing = Mat::zeros(cannyImage.size(), CV_8UC3);
    for (int i = 0; i < contours.size(); i++)
    {
        // check rectangle
        checkRetangle(contours[i]);
        
        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
    }

    drawing.copyTo(dstImage);
    return true;
}

bool CIdPrepare::checkRetangle(vector<Point> retangle)
{
    // cout << "[CIdPrepare::checkRetangle] " << retangle.size() << endl;
    return true;
}

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double CIdPrepare::angle(Point pt1, Point pt2, Point pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
void CIdPrepare::findSquares(const Mat& image, vector<vector<Point> >& squares)
{
    squares.clear();

    Mat pyr, timg, gray0(image.size(), CV_8U), gray;

    // down-scale and upscale the image to filter out the noise
    pyrDown(image, pyr, Size(image.cols / 2, image.rows / 2));
    pyrUp(pyr, timg, image.size());
    vector<vector<Point> > contours;

    // find squares in every color plane of the image
    for (int c = 0; c < 3; c++)
    {
        int ch[] = { c, 0 };
        mixChannels(&timg, 1, &gray0, 1, ch, 1);

        // try several threshold levels
        for (int l = 0; l < N; l++)
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if (l == 0)
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                Canny(gray0, gray, 0, thresh, 5);
                // dilate canny output to remove potential
                // holes between edge segments
                dilate(gray, gray, Mat(), Point(-1, -1));
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l + 1) * 255 / N;
            }

            // find contours and store them all as a list
            findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

            vector<Point> approx;

            // test each contour
            for (size_t i = 0; i < contours.size(); i++)
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if (approx.size() == 4 &&
                    fabs(contourArea(Mat(approx))) > 1000 &&
                    isContourConvex(Mat(approx)))
                {
                    double maxCosine = 0;

                    for (int j = 2; j < 5; j++)
                    {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if (maxCosine < 0.3)
                        squares.push_back(approx);
                }
            }
        }
    }
}

bool CIdPrepare::getSquares(Mat& image, const vector<vector<Point> >& squares, Rect &rect)
{
    for (size_t i = 0; i < squares.size(); i++)
    {
        const Point* p = &squares[i][0];
        int n = (int)squares[i].size();

        // filter
        if (n == 4){
            const Point p0 = squares[i][0];
            const Point p1 = squares[i][1];
            const Point p2 = squares[i][2];
            const Point p3 = squares[i][3];

            int nBorder = 5;
            if ((p0.x < nBorder && p0.y < nBorder)
                && ((p1.x < nBorder) && (p1.y >(image.rows - nBorder)))
                && ((p3.x >(image.cols - nBorder)) && (p3.y < nBorder))
                && (p2.x >(image.cols - nBorder) && (p2.y >(image.rows - nBorder)))){
                continue;
            }

            if (((p0.x < image.cols / 2) && (p0.y < image.rows / 2))
                && ((p1.x < image.cols / 2) && (p1.y > image.rows / 2))
                && ((p3.x > image.cols / 2) && (p3.y < image.rows / 2))
                && ((p2.x > image.cols / 2) && (p2.y > image.rows / 2))){

                cout << "[CIdPrepare::drawSquares] retangle 1!" << endl;
                rect = Rect(p0, p2);
                return true;
            }

            if (((p0.x < image.cols / 2) && (p0.y < image.rows / 2))
                && ((p1.x > image.cols / 2) && (p1.y < image.rows / 2))
                && ((p3.x < image.cols / 2) && (p3.y > image.rows / 2))
                && ((p2.x > image.cols / 2) && (p2.y > image.rows / 2))){

                cout << "[CIdPrepare::drawSquares] retangle 2!" << endl;
                rect = Rect(p0,p2);
                return true;
            }
        }
    }

    return false;
}

// the function draws all the squares in the image
void CIdPrepare::drawSquares(Mat& image, const vector<vector<Point> >& squares)
{
    RNG rng(12345);
    cout << "[CIdPrepare::drawSquares] size " << squares.size() << endl;  
    for (size_t i = 0; i < squares.size(); i++)
    {
        const Point* p = &squares[i][0];
        int n = (int)squares[i].size();

        // filter
        if (n == 4){
            const Point p0 = squares[i][0];
            const Point p1 = squares[i][1];
            const Point p2 = squares[i][2];
            const Point p3 = squares[i][3];

            cout << "[CIdPrepare::drawSquares] retangle p0:(" << p0.x << ", " << p0.y << ")" << endl;
            cout << "[CIdPrepare::drawSquares] retangle p1:(" << p1.x << ", " << p1.y << ")" << endl;
            cout << "[CIdPrepare::drawSquares] retangle p2:(" << p2.x << ", " << p2.y << ")" << endl;
            cout << "[CIdPrepare::drawSquares] retangle p3:(" << p3.x << ", " << p3.y << ")" << endl;

            int nBorder = 5;
            if ((p0.x < nBorder && p0.y < nBorder)
                && ((p1.x < nBorder) && (p1.y >(image.rows - nBorder)))
                && ((p3.x >(image.cols - nBorder)) && (p3.y < nBorder))
                && (p2.x >(image.cols - nBorder) && (p2.y > (image.rows - nBorder)))){
                continue;
            }

            Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
            polylines(image, &p, &n, 1, true, color, 3, CV_AA); // Scalar(0, 255, 0)

            //if (((p0.x < image.cols / 2) && (p0.y < image.rows / 2))
            //    && ((p1.x < image.cols / 2) && (p1.y > image.rows / 2))
            //    && ((p3.x > image.cols / 2) && (p3.y < image.rows / 2))
            //    && ((p2.x > image.cols / 2) && (p2.y > image.rows / 2))){

            //    cout << "[CIdPrepare::drawSquares] retangle 1!" << endl;

            //    Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
            //    polylines(image, &p, &n, 1, true, color, 3, CV_AA); // Scalar(0, 255, 0)
            //    break;
            //}

            //if (((p0.x < image.cols / 2) && (p0.y < image.rows / 2))
            //    && ((p1.x > image.cols / 2) && (p1.y < image.rows / 2))
            //    && ((p3.x < image.cols / 2) && (p3.y > image.rows / 2))
            //    && ((p2.x > image.cols / 2) && (p2.y > image.rows / 2))){

            //    cout << "[CIdPrepare::drawSquares] retangle 2!" << endl;

            //    Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
            //    polylines(image, &p, &n, 1, true, color, 3, CV_AA); // Scalar(0, 255, 0)
            //    break;
            //}
        }
    }
}
