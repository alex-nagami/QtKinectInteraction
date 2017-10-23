#include "KinectOpenCvTools.h"

Mat InfraDepth2Mat(Mat inframat, Mat depthmat)
{
	if (inframat.size() != depthmat.size())
		return Mat();
	Size size = inframat.size();
	Mat result(size, CV_8UC3, Scalar::all(0));
	for (int i = 0; i < size.height; i++)
	{
		for (int j = 0; j < size.width; j++)
		{
			result.at<Vec3b>(i, j)[0] = inframat.at<unsigned short>(i, j) / 256;
			result.at<Vec3b>(i, j)[1] = depthmat.at<unsigned short>(i, j) / 256 * 50;
			result.at<Vec3b>(i, j)[2] = depthmat.at<unsigned short>(i, j) % 256;
		}
	}
	return result;
}

void Mat2InfraDepth(Mat source, Mat& inframat, Mat& depthmat)
{
	Size size = source.size();
	inframat = Mat(size, CV_16U, Scalar::all(0));
	depthmat = Mat(size, CV_16U, Scalar::all(0));
	for (int i = 0; i < size.height; i++)
	{
		for (int j = 0; j < size.width; j++)
		{
			inframat.at<unsigned short>(i, j) = source.at<Vec3b>(i, j)[0] * 256;
			depthmat.at<unsigned short>(i, j) = source.at<Vec3b>(i, j)[1] * 256 + source.at<Vec3b>(i, j)[2];
		}
	}
}

Mat SplitUserFromBackground(Mat depth, Mat body, int userID)
{
	Mat result = depth.clone();
	for (int i = 0; i <body.rows; i++)
		for (int j = 0; j < body.cols; j++)
		{
			if (!(body.data[i*body.cols + j] == userID || (userID == -1 && body.data[i*body.cols + j] < 6)))
			{
				((unsigned short*)result.data)[i*body.cols + j] = 0;
			}
		}
	return result;
}

Mat DrawBone(Mat input, Joint* joints, Point2f* points, JointType joint0, JointType joint1, Scalar color)
{
	Mat output = input.clone();
	TrackingState joint0State = joints[joint0].TrackingState;
	TrackingState joint1State = joints[joint1].TrackingState;
	// If we can't find either of these joints, exit
	if ((joint0State == TrackingState_NotTracked) || (joint1State == TrackingState_NotTracked))
		return output;
	// Don't draw if both points are inferred
//	if ((joint0State == TrackingState_Inferred) && (joint1State == TrackingState_Inferred))
//		return output;
	// We assume all drawn bones are inferred unless BOTH joints are tracked
	if ((joint0State == TrackingState_Tracked) && (joint1State == TrackingState_Tracked))
    line(output, points[joint0], points[joint1], color, 2, 8, 0);
	else
    line(output, points[joint0], points[joint1], color, 1, 8, 0);
	return output;
}

Mat DrawBody(Mat input, Joint* joints, Point2f* points, Scalar color)
{
	Mat output;
	// Torso
	output = DrawBone(input, joints, points, JointType_Head, JointType_Neck, color);
	output = DrawBone(output, joints, points, JointType_Neck, JointType_SpineShoulder, color);
	output = DrawBone(output, joints, points, JointType_SpineShoulder, JointType_SpineMid, color);
	output = DrawBone(output, joints, points, JointType_SpineMid, JointType_SpineBase, color);
	output = DrawBone(output, joints, points, JointType_SpineShoulder, JointType_ShoulderRight, color);
	output = DrawBone(output, joints, points, JointType_SpineShoulder, JointType_ShoulderLeft, color);
  output = DrawBone(output, joints, points, JointType_SpineBase, JointType_HipRight, color);
  output = DrawBone(output, joints, points, JointType_SpineBase, JointType_HipLeft, color);
	// Right Arm
	output = DrawBone(output, joints, points, JointType_ShoulderRight, JointType_ElbowRight, color);
	output = DrawBone(output, joints, points, JointType_ElbowRight, JointType_WristRight, color);
	output = DrawBone(output, joints, points, JointType_WristRight, JointType_HandRight, color);
	output = DrawBone(output, joints, points, JointType_HandRight, JointType_HandTipRight, color);
	output = DrawBone(output, joints, points, JointType_WristRight, JointType_ThumbRight, color);
	// Left Arm
	output = DrawBone(output, joints, points, JointType_ShoulderLeft, JointType_ElbowLeft, color);
	output = DrawBone(output, joints, points, JointType_ElbowLeft, JointType_WristLeft, color);
	output = DrawBone(output, joints, points, JointType_WristLeft, JointType_HandLeft, color);
	output = DrawBone(output, joints, points, JointType_HandLeft, JointType_HandTipLeft, color);
	output = DrawBone(output, joints, points, JointType_WristLeft, JointType_ThumbLeft, color);
	// Right Leg
  output = DrawBone(output, joints, points, JointType_HipRight, JointType_KneeRight, color);
  output = DrawBone(output, joints, points, JointType_KneeRight, JointType_AnkleRight, color);
  output = DrawBone(output, joints, points, JointType_AnkleRight, JointType_FootRight, color);
	// Left Leg
  output = DrawBone(output, joints, points, JointType_HipLeft, JointType_KneeLeft, color);
  output = DrawBone(output, joints, points, JointType_KneeLeft, JointType_AnkleLeft, color);
  output = DrawBone(output, joints, points, JointType_AnkleLeft, JointType_FootLeft, color);
	return output;
}


CameraSpacePoint TransformCameraSpacePoint(CameraSpacePoint from, Mat trans)
{
	assert(trans.type()==CV_32F && trans.rows==4 && trans.cols==4 && trans.channels()==1);
	CameraSpacePoint result;
	result.X = trans.at<float>(0, 0)*from.X+trans.at<float>(0, 1)*from.Y+trans.at<float>(0, 2)*from.Z+trans.at<float>(0, 3);
	result.Y = trans.at<float>(1, 0)*from.X+trans.at<float>(1, 1)*from.Y+trans.at<float>(1, 2)*from.Z+trans.at<float>(1, 3);
	result.Z = trans.at<float>(2, 0)*from.X+trans.at<float>(2, 1)*from.Y+trans.at<float>(2, 2)*from.Z+trans.at<float>(2, 3);
	return result;
}

Mat DrawHand(Mat input, Point2f leftHand, HandState leftState, Point2f rightHand, HandState rightState)
{
  Scalar color;
  Mat output = input.clone();
  // left hand
  if(leftState != HandState_NotTracked && leftState != HandState_Unknown)
  {
    switch(leftState)
    {
    case HandState_Closed: color = Scalar(255, 0, 0); break;
    case HandState_Open: color = Scalar(0, 255, 0); break;
    case HandState_Lasso: color = Scalar(0, 0, 255); break;
    };
    circle(output, leftHand, 15, color, -1);
  }

  // right hand
  if(rightState != HandState_NotTracked && rightState != HandState_Unknown)
  {
    switch(rightState)
    {
    case HandState_Closed: color = Scalar(255, 0, 0); break;
    case HandState_Open: color = Scalar(0, 255, 0); break;
    case HandState_Lasso: color = Scalar(0, 0, 255); break;
    };
    circle(output, rightHand, 15, color, -1);
  }

  return output;
}
