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

	//depth frame reader stuff
	hr = depthFrameSource->OpenReader(&m_depthFrameReader);
	if (FAILED(hr))
	{
		printf("Failed to open depth frame reader.\n");
		exit(10);
	}

	SafeRelease(depthFrameSource);

	m_depthBuffer = new uint16[512 * 424];
}

void KinectScan::Tick(float deltaTime)
{
	//put update and drawing stuff here
	HRESULT hr;
	IDepthFrame* depthFrame;
	hr = m_depthFrameReader->AcquireLatestFrame(&depthFrame);

	if (FAILED(hr))
	{
		return;
	}

	printf("Copying data\n");

	hr = depthFrame->CopyFrameDataToArray(512 * 424, m_depthBuffer);
	if (FAILED(hr))
	{
		SafeRelease(depthFrame);
		printf("Copying FAILED\n");
		return;
	}
	SafeRelease(depthFrame);

	//Copy depth data to the screen
	for (int i = 0; i < SCRHEIGHT * SCRWIDTH; ++i)
	{
		uint16 d = m_depthBuffer[i];
		uint8 a = d & 0xff;
		m_pixelBuffer[i] = (a << 16) | (a << 8) | a;
	}

}

void KinectScan::Shutdown()
{
	//put cleaning up stuff here
	delete[] m_depthBuffer;
	SafeRelease(m_depthFrameReader);
	SafeRelease(m_sensor);
}
