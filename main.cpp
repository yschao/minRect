#include <QCoreApplication>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "iostream"
#include <stdlib.h>

#define random(x) (rand()%x)

float cal_point_line_dist(float k, float b,cv::Point start,cv::Point point){
    float dist = (k*(point.x) + b - point.y);
    return dist;
}

cv::Point get_max_min_dist_index(std::vector<cv::Point> points,cv::Point start, float k,float b){
    int MAX_VALUE = points.size();
    float max_dist = 0.0;
    float min_dist = 0.0;
    int max_dist_index = 0;
    int min_dist_index = 0;
    for(int i = 0;i<MAX_VALUE;i++)
    {
        cv::Point point = points.at(i);
        float dist = cal_point_line_dist(k,b,start,point);
        if(dist>max_dist){
            max_dist = dist;
            max_dist_index = i;
            continue;
        }
        if(dist < min_dist){
            min_dist = dist;
            min_dist_index = i;
        }

    }
    return cv::Point(max_dist_index,min_dist_index);
}

cv::Point Get_Rect_Point(float k, float b1,float b2){
    int x = k*(b2 - b1)/(k*k + 1);
    int y = k * x + b1 ;
    return cv::Point(x,y);
}

void draw_img(cv::Mat &img,cv::RotatedRect rectPoint){
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
}


void use_origin_get_minRect(cv::Mat &img ,std::vector<cv::Point> points)
{
    float k;
    float b0;
    float b[4];

    int max_dist_index;
    int min_dist_index;
    int MAX_VALUE  = points.size();
    int seed = random(MAX_VALUE);
    seed =8;
    k = 3;

    cv::Point start = points.at(seed);
    cv::Point end ;
    b0 = start.y - k*start.x;

    end.x = 20;
    end.y = k*end.x + b0 ;

    cv::Point max_min_index = get_max_min_dist_index(points,start,k,b0);
    max_dist_index = max_min_index.x;
    min_dist_index = max_min_index.y;

    cv::line(img,start,end,cv::Scalar(255,0,0));
    end = points.at(max_dist_index);
    b[0] = end.y - k*end.x;
    cv::line(img,end,cv::Point(20,k*(20) + b[0]),cv::Scalar(0,0,255));

    end = points.at(min_dist_index);
    b[1] = end.y - k*end.x;
    cv::line(img,end,cv::Point(20,k*(20)+ b[1]),cv::Scalar(0,255,0));

    //vertical
    k = -1/k;
    b0 = start.y - k * start.x;
    end.x = 20;
    end.y = k*end.x + b0 ;

    max_min_index = get_max_min_dist_index(points,start,k,b0);
    max_dist_index = max_min_index.x;
    min_dist_index = max_min_index.y;

    cv::line(img,start,end,cv::Scalar(255,0,0));
    end = points.at(max_dist_index);
    b[2] = end.y - k*end.x;
    cv::line(img,end,cv::Point(50,k*(50) + b[2]),cv::Scalar(0,0,255));

    end = points.at(min_dist_index);
    b[3] = end.y - k*end.x;
    cv::line(img,end,cv::Point(30,k*(30)+ b[3]),cv::Scalar(0,255,0));

    cv::Point p0 = Get_Rect_Point(-1/k,b[0],b[2]);
    cv::Point p1 = Get_Rect_Point(-1/k,b[1],b[2]);
    cv::Point p2 = Get_Rect_Point(-1/k,b[1],b[3]);
    cv::Point p3 = Get_Rect_Point(-1/k,b[0],b[3]);

    cv::line(img,p0,p1,cv::Scalar(255,255,0));
    cv::line(img,p1,p2,cv::Scalar(255,255,0));
    cv::line(img,p2,p3,cv::Scalar(255,255,0));
    cv::line(img,p0,p3,cv::Scalar(255,255,0));


    cv::imshow("show",img);
    cv::waitKey(0);


}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    cv::Mat img;
    cv::Mat gray;
    img = cv::imread("/mnt/hgfs/F/test_img/1.png");
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
    use_origin_get_minRect(img,hull_points);



    //Get The Max point distance
    float max_dist = 0.0;
    int start_index;
    int end_index;
    for(int i = 0;i<hull_points.size()-1;i++){
        for(int j = i+1;j<hull_points.size();j++){
            cv::Point start = hull_points.at(i);
            cv::Point end = hull_points.at(j);
            float temp = sqrt(pow(start.x - end.x,2) + pow(start.y-end.y,2));
            if(temp > max_dist){
                max_dist = temp;
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
    float dist = 0.0;
    cv::Point start = hull_points.at(start_index);
    cv::Point end = hull_points.at(end_index);
    cv::Point2f center;
    center.x = (start.x + end.x)/2;
    center.y = (start.y + end.y)/2;
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

    cv::Point max_dist_point = hull_points.at(max_dist_index);
    int h = sqrt(pow(end.x - max_dist_point.x,2) + pow(end.y-max_dist_point.y,2));
    int w = sqrt(max_dist*max_dist - h*h);
    float angle = atan2(float(start.y-max_dist_point.y)/(start.x - max_dist_point.x),1)*180/3.14;


    cv::RotatedRect rectPoint = cv::RotatedRect(center,cv::Size2f(w,h),angle);
    cv::line(img, hull_points.at(max_dist_index),start,cv::Scalar(0));
    cv::line(img, hull_points.at(max_dist_index),end,cv::Scalar(0));
    draw_img(img,rectPoint);
    rectPoint = cv::minAreaRect(cv::Mat(points));
    draw_img(img,rectPoint);
    cv::imshow("show",img);
    cv::waitKey(0);
    return a.exec();
}
