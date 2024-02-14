#include <iomanip>	
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string>
#include <opencv2/opencv.hpp>

#define W 1440

using namespace cv;
using namespace std;

class Coin{
	public:
   double radius;	
   string name;
   int val;
};

int main(int argc, char** argv ){
   Mat image;
   if ( argc != 2 ){
      printf("usage: ./coins <Image_Path>\n");
      return -1;
   }

   image = imread( argv[1], 1 );
   if ( !image.data ){
      printf("No image data \n");
      return -1;
   }

   resize(image, image, Size(W, (int)((double)W*image.rows/image.cols)));
   Mat blurImage(image.rows, image.cols, CV_8UC1);
   for(int x = 0; x < image.cols; x++){
      for(int y = 0; y < image.rows; y++){
         Vec3b intens = image.at<Vec3b>(y, x);
         blurImage.at<uchar>(y, x) = (int)(0.057*intens[0] + 0.294*intens[1] + 0.649*intens[2]);
      }
   }
   GaussianBlur(blurImage, blurImage, Size(9, 9), 0, 0);
   vector<Vec3f> circles;
   HoughCircles(blurImage, circles, CV_HOUGH_GRADIENT, 1, 33, 90, 27, 20, 65);

   Coin coins[6] = {{1.064, "Penny", 1}, {1.184, "Nickel", 5}, {1.000, "Dime", 10}, {1.355, "Quarter", 25}, {2.127, "Eisenhower dollar", 100}, {1.479, "Presidential dollar", 100}};
   double scale[65];	
   Scalar ms[circles.size()];
   for( size_t i = 0; i < circles.size(); i++ )
   {
      Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      int radius = cvRound(circles[i][2]);
      Mat roi = image(Range(center.y-radius, center.y+radius+1), Range(center.x-radius, center.x+radius+1));
      Mat1b mask(roi.rows, roi.cols);
      mask.setTo(0);
      circle(mask, Point(radius, radius), radius, 1, CV_FILLED, 8, 0);
      Scalar m = mean(roi, mask);
      ms[i] = m;
      for(int j = 0; j < 6; j++){
         Coin c = coins[j];
         int loc = (int)(pow(radius/c.radius, 2)/64);
         if(loc < 65)
            scale[loc] += 1/pow(c.radius, 2);
      }
   }
   double dime = 0;
   double max = 0;
   for(int j = 0; j < 65; j++){
      if(scale[j] > max){
         max = scale[j];
         dime = (double)j;
      }
   }
   dime += 0.5;
   dime *= 64;
   dime = sqrt(dime);

   int total[6];
   cvtColor(image, image, COLOR_BGR2GRAY);	
   cvtColor(image, image, COLOR_GRAY2BGR);
   for( size_t i = 0; i < circles.size(); i++ )
   {
      Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      int radius = cvRound(circles[i][2]);
   	
      double times = radius/dime;	
      double minlike = 1.0;
      int bestind = -1;
      for(int j = 0; j < 6; j++){
         Coin c = coins[j];
         double errrad = (times-c.radius)/c.radius;
         double likely = 3*fabs(errrad);
         if(likely < minlike){
            minlike = likely;
            bestind = j;
         }
      }
      Scalar color = Scalar(0,0,255);
      if(bestind != -1){
         total[bestind] += 1;
         circle( image, center, 3, Scalar(0,255,0), -1, 8, 0 );
         circle( image, center, radius, color, 3, 8, 0 );
      }
   }
   double val = 0;
   for(int j = 0; j < 6; j++)
      val += total[j] * coins[j].val;
	
   val /= 100;

   cout << fixed;
   cout << setprecision(2);
   cout << val << endl;
   namedWindow("Coins", WINDOW_NORMAL);
   resizeWindow("Coins", 1440, 1080);
   imshow("Coins", image);
   return 0;
}
