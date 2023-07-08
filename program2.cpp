// Name: Khanh Le
// ID:   1001741583
// Due:  8 July 2022

#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"

// configuration parameters
#define NUM_COMMAND_LINE_ARGUMENTS 1
#define PI 3.14159265358979323846;


int main( int argc, char **argv)
{
    cv::Mat imageIn;

    // validate and parse the command line arguments
    if( argc != NUM_COMMAND_LINE_ARGUMENTS + 1 )
    {
        std::printf("USAGE: %s <image_path> \n ", argv[0]);
        return 0;
    }
    else
    {
        // open the input image
        std::string inputFileName = argv[1];
        imageIn = cv::imread(inputFileName, cv::IMREAD_COLOR);

        // check for file error
        if( !imageIn.data )
        {
            std::cout << "Error while opening file " << inputFileName << std::endl;
            return 0;
        }

        // Convert the image to grayscale
        cv::Mat imageGray;
        cv::cvtColor(imageIn, imageGray, cv::COLOR_BGR2GRAY);

        // Normalize
        cv::Mat imageNormalized;
        cv::normalize(imageGray, imageNormalized, 0, 255, cv::NORM_MINMAX, CV_8UC1);

        // Find the image edges
        cv::Mat imageEdges;
        const double cannyThreshold1 = 100;
        const double cannyThreshold2 = 200;
        const int cannyAperture = 3; 
        cv::Canny(imageGray, imageEdges, cannyThreshold1, cannyThreshold2, cannyAperture);
        
        // Erode and Dilate the edges to remove noise
        int morphologySize = 1;
        cv::Mat edgesDilated;
        cv::dilate(imageEdges, edgesDilated, cv::Mat(), cv::Point(-1, -1), morphologySize);
        cv::Mat edgesEroded;
        cv::erode(edgesDilated, edgesEroded, cv::Mat(), cv::Point(-1, -1), morphologySize);
    
        // locate the image contours (after applying a threshold or canny)
        std::vector<std::vector<cv::Point> > contours;
        cv::findContours(edgesEroded, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

        // fit ellipses to contours containing sufficient inliers
        std::vector<cv::RotatedRect> fittedEllipses(contours.size());
        for(int i = 0; i < contours.size(); i++)
        {
            // compute an ellipse only if the contour has more than 5 points (the minimum for ellipse fitting)
            if(contours.at(i).size() > 5)
            {
                fittedEllipses[i] = cv::fitEllipse(contours[i]);
            }
        }

        /* 5 parameters of ellipse: 
            * xc: x coordinate of the center
            * xy: y coordinate of the center
            * a : major semi-axis
            * b : minor semi-axis           */

        // Draw the ellipses
        cv::Mat imageEllipse = imageIn.clone();
        const int minEllipseInliers = 50;

        // Store parameters of ellipse
        float xc, yc, a, b;
        // Total amount of money for coins; area of coin
        float total = 0, area = 0;
        // Value of coins 
        float penny = 0.01, nickel = 0.05, dime = 0.10, quarter = 0.25;
        // Count number of coins
        int countPenny = 0, countNickel = 0, countDime = 0, countQuarter = 0;

        cv::Scalar red(255,0,0), yellow(0,255,255), blue(0,0,255), green(0,128,0);


        for(int i = 0; i < contours.size(); i++)
        {
            // Draw any ellipse with sufficient inliers
            if(contours.at(i).size() > minEllipseInliers)
            {

                xc = fittedEllipses[i].center.x;
                yc = fittedEllipses[i].center.y;
                a  = fittedEllipses[i].size.width/2;
                b  = fittedEllipses[i].size.height/2;

                // Area of elipse is calculated by (major semi-axis * minor semi-axis) * Pi
                area = (a * b) * PI;

                // Based on the area, each coin will be identified as either quarter, nickel, dime, or penny
                // The color of the ellipse will depend on the type of coin
                // Green for quarter ; Yellow for nickel ; Blue for dime ; Red for penny
                // Each count of the corresponding identified coin will increase.
                // The value of the identified coin will be added to the total 
                if( area > 50000 ) //quarter
                {
                    cv::ellipse(imageEllipse, fittedEllipses[i], green, 2);
                    countQuarter++;
                    total += quarter;
                }
                else if( area > 40500 && area < 49000 ) // nickel
                {
                    cv::ellipse(imageEllipse, fittedEllipses[i], yellow, 2);
                    countNickel++;
                    total += nickel;
                }
                else if( area > 31000 && area < 40000 ) // dime
                {
                    cv::ellipse(imageEllipse, fittedEllipses[i], blue , 2);
                    countDime++;
                    total += dime;
                }
                else
                {
                    cv::ellipse(imageEllipse, fittedEllipses[i], red, 2); //penny
                    countPenny++;
                    total += penny;
                }


            }

        }

        // Print to the console the counting of each type of coin
        // and the total value.
        std::cout << "Penny   -  " << countPenny   << std::endl;
        std::cout << "Nickel  -  " << countNickel  << std::endl;
        std::cout << "Dime    -  " << countDime    << std::endl;
        std::cout << "Quarter -  " << countQuarter << std::endl;
        std::cout << "Total   - $" << total        << std::endl;


        // display the images
        cv::imshow("imageIn", imageIn);
        cv::imshow("imageEllipse", imageEllipse);
        cv::waitKey();
    }
    return 0;
}


