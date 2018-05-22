#include <QCoreApplication>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "iostream"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    cv::Mat img;
    cv::Mat gray;
    img = cv::imread("/mnt/hgfs/F/test_img/3.png");
    cv::cvtColor(img, gray,CV_BGR2GRAY);
    std::vector<cv::Point> points;
    uchar data;
    for(int row = 0; row<img.rows;row++){
        for(int col = 0;col<img.cols;col++){
            data = gray.data[row*img.cols + col];
            if(data <255){
                points.push_back(cv::Point(col,row));
//                std::cout<<int(data)<<std::endl;
            }
        }
    }
    std::vector<cv::Point>hull_points;
    cv::convexHull(points,hull_points,true);
    for(int i = 0;i<hull_points.size()-1;i++){
        cv::line(img,hull_points.at(i),hull_points.at(i+1),cv::Scalar(0,255,0));
    }

    //Get The Max point distance
    float dist = 0.0;
    int start_index;
    int end_index;
    for(int i = 0;i<hull_points.size()-1;i++){
        for(int j = i+1;j<hull_points.size();j++){
            cv::Point start = hull_points.at(i);
            cv::Point end = hull_points.at(j);
            float temp = sqrt(pow(start.x - end.x,2) + pow(start.y-end.y,2));
            if(temp > dist){
                dist = temp;
                start_index = i;
                end_index = j;
            }
        }
    }
    cv::line(img,hull_points.at(start_index),hull_points.at(end_index),cv::Scalar(255,0,255));
    //Get the max dist point
    float A;
    float B;
    float max_dist_index;
    cv::Point start = hull_points.at(start_index);
    cv::Point end = hull_points.at(end_index);
    A = float(end.y - start.y)/(end.x - start.x);
    B = end.y - A*end.x;
    dist = 0.0;
    for(int i = 0;i<hull_points.size();i++){
        if(i == start_index || i == end_index){
            continue;
        }
        cv::Point point = hull_points.at(i);
        float temp = fabs((A*point.x + B - point.y)/sqrt(A *A + B*B));
        if(temp>dist){
            dist = temp;
            max_dist_index = i;
        }
    }
    cv::line(img, hull_points.at(max_dist_index),start,cv::Scalar(0));
    cv::line(img, hull_points.at(max_dist_index),end,cv::Scalar(0));
    cv::RotatedRect rectPoint = cv::minAreaRect(cv::Mat(points));
    cv::Point2f fourPoint2f[4];
    rectPoint.points(fourPoint2f);
    //根据得到的四个点的坐标  绘制矩形
    for (int i = 0; i < 3; i++)
    {
        cv::line(img, fourPoint2f[i], fourPoint2f[i + 1]
                , cv::Scalar(0,0,255), 1);
    }
    cv::line(img, fourPoint2f[0], fourPoint2f[3]
            , cv::Scalar(0,0,255), 1);

    cv::imshow("show",img);
    cv::waitKey(0);
    return a.exec();
}
