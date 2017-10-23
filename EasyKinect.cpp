#include "EasyKinect.h"
#include <opencv2\opencv.hpp>
#define _OPENCV_USED

#ifdef _OPENCV_USED

using namespace cv;

/// <summary>
/// Convert depthframe in IDepthFrame* to a CV_16U Mat.
/// </summary>
/// <param name="depthframe">The pointer to the obtained depth frame</param>
/// <returns>Returns a Mat in CV_16U containing the depth frame</returns>
Mat depth2mat(IDepthFrame* depthframe)
{
	if (depthframe == NULL)
	{
		return Mat();
	}
	IFrameDescription* size = NULL;
	depthframe->get_FrameDescription(&size);
	int height = 0, width = 0;
	size->get_Height(&height);
	size->get_Width(&width);
	SafeRelease(size);
	Mat frame(height, width, CV_16U, Scalar::all(0));
	UINT16* depthbuffer = NULL;
	UINT buffersize = 0;
	if (SUCCEEDED(depthframe->AccessUnderlyingBuffer(&buffersize, &depthbuffer)))
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				frame.at<unsigned short>(i, j) = depthbuffer[i*width + j];
			}
		}
	}
	return frame;
}

/// <summary>
/// Convert colorframe in IColorFrame* to a CV_8UC3 Mat.
/// </summary>
/// <param name="colorframe">The pointer to the obtained color frame</param>
/// <returns>Returns a Mat in CV_8UC3 containing the color frame</returns>
Mat color2mat(IColorFrame* colorframe)
{
	if (colorframe == NULL)
	{
		return Mat();
	}
	IFrameDescription* size = NULL;
	colorframe->get_FrameDescription(&size);
	int height = 0, width = 0;
	size->get_Height(&height);
	size->get_Width(&width);
	SafeRelease(size);
	Mat frame(height, width, CV_8UC3, Scalar::all(0));
	static RGBQUAD* colorbuffer = new RGBQUAD[height * width];
	UINT buffersize = height * width * sizeof(RGBQUAD);
	HRESULT hr = colorframe->CopyConvertedFrameDataToArray(buffersize, reinterpret_cast<BYTE*>(colorbuffer), ColorImageFormat_Bgra);
	if (SUCCEEDED(hr))
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				frame.at<Vec3b>(i, j)[0] = colorbuffer[i*width + j].rgbBlue;
				frame.at<Vec3b>(i, j)[1] = colorbuffer[i*width + j].rgbGreen;
				frame.at<Vec3b>(i, j)[2] = colorbuffer[i*width + j].rgbRed;
			}
		}
	}
	return frame;
}

/// <summary>
/// Convert infraredframe in IInfraredFrame* to a CV_16U Mat.
/// </summary>
/// <param name="infraframe">The pointer to the obtained infrared frame</param>
/// <returns>Returns a Mat in CV_16U containing the infrared frame</returns>
Mat infra2mat(IInfraredFrame* infraframe)
{
	IFrameDescription* size = NULL;
	if (infraframe == NULL)
	{
		return Mat();
	}
	infraframe->get_FrameDescription(&size);
	int height = 0, width = 0;
	size->get_Height(&height);
	size->get_Width(&width);
	SafeRelease(size);
	Mat frame(height, width, CV_16U, Scalar::all(0));
	UINT16* buffer = NULL;
	UINT buffersize = 0;
	if (SUCCEEDED(infraframe->AccessUnderlyingBuffer(&buffersize, &buffer)))
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				frame.at<unsigned short>(i, j) = buffer[i*width + j];
			}
		}
	}
	return frame;
}

/// <summary>
/// Convert infraredframe in IInfraredFrame* to a CV_16U Mat.
/// </summary>
/// <param name="infraframe">The pointer to the obtained infrared frame</param>
/// <returns>Returns a Mat in CV_16U containing the infrared frame</returns>
Mat longinfra2mat(ILongExposureInfraredFrame* infraframe)
{
	IFrameDescription* size = NULL;
	infraframe->get_FrameDescription(&size);
	int height = 0, width = 0;
	size->get_Height(&height);
	size->get_Width(&width);
	SafeRelease(size);
	Mat frame(height, width, CV_16U, Scalar::all(0));
	UINT16* buffer = NULL;
	UINT buffersize = 0;
	if (SUCCEEDED(infraframe->AccessUnderlyingBuffer(&buffersize, &buffer)))
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				frame.at<unsigned short>(i, j) = buffer[i*width + j];
			}
		}
	}
	return frame;
}

Mat bodyindex2mat(IBodyIndexFrame* bodyindex)
{
	IFrameDescription* size = NULL;
	bodyindex->get_FrameDescription(&size);
	int height = 0, width = 0;
	size->get_Height(&height);
	size->get_Width(&width);
	SafeRelease(size);
	Mat frame(height, width, CV_8U, Scalar::all(0));
	BYTE* buffer = NULL;
	UINT buffersize = 0;
	if (SUCCEEDED(bodyindex->AccessUnderlyingBuffer(&buffersize, &buffer)))
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				frame.at<uchar>(i, j) = buffer[i*width + j];
			}
		}
		return frame;
	}
#ifdef _LJX_DEBUG
	else
		cout << "bodyindex->AccessUnderlyingBuffer failed." << endl;
#endif
	return Mat();
}

#endif

void PrintMatrix4(Matrix4 mat, int fw, ostream &stream)
{
	stream << fixed << setw(fw) << mat.M11 << ", " << mat.M12 << ", " << mat.M13 << ", " << mat.M14 << endl;
	stream << fixed << setw(fw) << mat.M21 << ", " << mat.M22 << ", " << mat.M23 << ", " << mat.M24 << endl;
	stream << fixed << setw(fw) << mat.M31 << ", " << mat.M32 << ", " << mat.M33 << ", " << mat.M34 << endl;
	stream << fixed << setw(fw) << mat.M41 << ", " << mat.M42 << ", " << mat.M43 << ", " << mat.M44 << endl;
}

HRESULT IBF2KBody(IBodyFrame* frame, KinectBody bodies[])
{
	HRESULT result;
	if (frame == NULL)
	{
		return -1;
	}
	INT64 time = 0;
	result = frame->get_RelativeTime(&time);
	if (FAILED(result))
	{
		return result;
	}
	IBody* body[BODY_COUNT] = { 0 };
	result = frame->GetAndRefreshBodyData(BODY_COUNT, body);
	for (int i = 0; i < BODY_COUNT; i++)
	{
		result = body[i]->get_IsTracked(&bodies[i].tracked);
		if (SUCCEEDED(result) && bodies[i].tracked)
		{
			bodies[i].time = time;
			body[i]->get_HandLeftState(&bodies[i].left);
			body[i]->get_HandRightState(&bodies[i].right);
			result = body[i]->GetJoints(_countof(bodies[i].joints), bodies[i].joints);
		}
	}
	return result;
}

void SetIdentityMatrix(Matrix4 &mat)
{
	mat.M11 = 1; mat.M12 = 0; mat.M13 = 0; mat.M14 = 0;
	mat.M21 = 0; mat.M22 = 1; mat.M23 = 0; mat.M24 = 0;
	mat.M31 = 0; mat.M32 = 0; mat.M33 = 1; mat.M34 = 0;
	mat.M41 = 0; mat.M42 = 0; mat.M43 = 0; mat.M44 = 1;
}