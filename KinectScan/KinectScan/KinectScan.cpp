#include "KinectScan.h"

void KinectScan::Init()
{
	//put initialization stuff here 
	HRESULT hr;
	hr = GetDefaultKinectSensor(&m_sensor);
	if (FAILED(hr))
	{
		printf("Failed to find kinect sensor.\n");
		exit(10);
	}

	m_sensor->Open();
	
	//depth frame source stuff
	IDepthFrameSource* depthFrameSource;
	hr = m_sensor->get_DepthFrameSource(&depthFrameSource);
	if (FAILED(hr))
	{
		printf("Failed to get depth frame source.\n");
		exit(10);
	}

	//dynamically allocate depth frame
	IFrameDescription* frameDesc;
	depthFrameSource->get_FrameDescription(&frameDesc);
	frameDesc->get_Width(&m_depthWidth);
	frameDesc->get_Height(&m_depthHeight);

	//depth frame reader stuff
	hr = depthFrameSource->OpenReader(&m_depthFrameReader);
	if (FAILED(hr))
	{
		printf("Failed to open depth frame reader.\n");
		exit(10);
	}

	SafeRelease(depthFrameSource);

	m_depthBuffer = new uint16[m_depthWidth * m_depthHeight];

	//color frame source stuff
	IColorFrameSource* colorFrameSource;
	hr = m_sensor->get_ColorFrameSource(&colorFrameSource);
	if (FAILED(hr))
	{
		printf("Failed to get color frame source.\n");
		exit(10);
	}

	//dynamically allocate color frame
	colorFrameSource->get_FrameDescription(&frameDesc);
	frameDesc->get_Width(&m_colorWidth);
	frameDesc->get_Height(&m_colorHeight);

	//color frame reader stuff
	hr = colorFrameSource->OpenReader(&m_colorFrameReader);
	if (FAILED(hr))
	{
		printf("Failed to open color frame reader.\n");
		exit(10);
	}

	SafeRelease(colorFrameSource);

	m_colorBuffer = new uint32[m_colorWidth * m_colorHeight];

	//get coordinateMapper
	hr = m_sensor->get_CoordinateMapper(&m_coordinateMapper);
	if (FAILED(hr))
	{
		printf("Failed to get coordinate mapper.\n");
		exit(10);
	}

	//allocate color space buffer
	m_colorSpacePoints = new ColorSpacePoint[m_depthWidth * m_depthHeight];
}

void KinectScan::Tick(float deltaTime)
{
	//put update and drawing stuff here
	HRESULT hr;
	
	//depth stuff
	IDepthFrame* depthFrame;
	hr = m_depthFrameReader->AcquireLatestFrame(&depthFrame);

	if (FAILED(hr))
	{
		return;
	}

	//printf("Copying data\n");

	hr = depthFrame->CopyFrameDataToArray(m_depthWidth * m_depthHeight, m_depthBuffer);
	if (FAILED(hr))
	{
		SafeRelease(depthFrame);
		printf("Copying FAILED\n");
		return;
	}
	SafeRelease(depthFrame);

	//color stuff
	IColorFrame* colorFrame;
	hr = m_colorFrameReader->AcquireLatestFrame(&colorFrame);

	if (FAILED(hr))
	{
		return;
	}

	hr = colorFrame->CopyConvertedFrameDataToArray(m_colorWidth * m_colorHeight * 4, (BYTE*)m_colorBuffer, ColorImageFormat_Bgra);
	
	if (FAILED(hr))
	{
		return;
	}
	SafeRelease(colorFrame);

	/*
	//copy color to pixel buffer
	for (int i = 0; i < SCRHEIGHT * SCRWIDTH; ++i)
	{
		m_pixelBuffer[i] = m_colorBuffer[i];
	}
	*/
	
	hr = m_coordinateMapper->MapDepthFrameToColorSpace(
		m_depthWidth * m_depthHeight, m_depthBuffer,
		m_depthWidth * m_depthHeight, m_colorSpacePoints
	);
	if (FAILED(hr))
	{
		printf("Failed to map depth to color space.\n");
		return;
	}

	//Copy depth data to the screen
	for (int i = 0; i < SCRHEIGHT * SCRWIDTH; ++i)
	{
		ColorSpacePoint csp = m_colorSpacePoints[i];
		int ix = (int)csp.X;
		int iy = (int)csp.Y;

		if (ix >= 0 && ix < m_colorWidth && iy >= 0 && iy < m_colorHeight)
		{
			m_pixelBuffer[i] = m_colorBuffer[ix + iy * 1920];
		}
		else
		{
			m_pixelBuffer[i] = 0xff0000;
		}

		//uint16 d = m_depthBuffer[i];
		//uint8 a = d & 0xff;
		//m_pixelBuffer[i] = (a << 16) | (a << 8) | a;
	}
	
}

void KinectScan::Shutdown()
{
	//put cleaning up stuff here

	delete[] m_colorBuffer;
	SafeRelease(m_colorFrameReader);

	delete[] m_depthBuffer;
	SafeRelease(m_depthFrameReader);
	SafeRelease(m_sensor);
}
