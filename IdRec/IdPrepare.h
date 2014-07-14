#pragma once
class CIdPrepare
{
public:
    CIdPrepare();
    ~CIdPrepare();

    void loopPrepare();
    bool singlePrepare(string szFile);

private:
    bool dumpFile(string szFile, Mat image, string szPrefix);

    bool prepareCanny(Mat src, Mat &dst);

    bool checkRetangle(vector<Point> retangle);

    double angle(Point pt1, Point pt2, Point pt0);
    void drawSquares(Mat& image, const vector<vector<Point> >& squares);
    void findSquares(const Mat& image, vector<vector<Point> >& squares);

    bool getSquares(Mat& image, const vector<vector<Point> >& squares, Rect &rect);
};

