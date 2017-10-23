#pragma once

#ifndef _KINECT_OPENCV_TOOLS
#define _KINECT_OPENCV_TOOLS

#include <Kinect.h>
#include "EasyKinect.h"
#include <opencv2\opencv.hpp>
using namespace cv;
#include <iostream>
#include <string>
#include <vector>
using namespace std;

/// <summary>
/// Combine CV_16U infrared frame and CV_16U depth frame into a CV_8UC3 mat
/// </summary>
/// <param name="inframat">The Mat structure containing infrared frame</param>
/// <param name="depthmat">The Mat structure containing depth frame</param>
/// <returns>Returns a Mat in CV_8UC3 containing the combined frame</returns>
Mat InfraDepth2Mat(Mat inframat, Mat depthmat);

/// <summary>
/// Decode the combined mat to CV_16U infrared frame and CV_16U depth frame
/// </summary>
/// <param name="source">Input the combined mat</param>
/// <param name="inframat">Output the Mat structure containing infrared frame</param>
/// <param name="depthmat">Output the Mat structure containing depth frame</param>
void Mat2InfraDepth(Mat source, Mat& inframat, Mat& depthmat);

/// <summary>
/// Split user out of backgroung using a Mat of BodyIndex frame and a Mat of depth
/// </summary>
Mat SplitUserFromBackground(Mat depth, Mat body, int userID = -1);

Mat DrawBone(Mat input, Joint* joints, Point2f* points, JointType joint0, JointType joint1, Scalar color = Scalar::all(255));

Mat DrawBody(Mat input, Joint* joints, Point2f* points, Scalar color=Scalar(255));

Mat DrawHand(Mat input, Point2f leftHand, HandState leftState, Point2f rightHand, HandState rightState);

CameraSpacePoint TransformCameraSpacePoint(CameraSpacePoint from, Mat trans);

#endif
