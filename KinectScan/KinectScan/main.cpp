
//Kinect v2
#include <iostream>
#include <chrono>

#include <SDL.h>

#include "KinectScan.h"

using namespace std::chrono;
typedef steady_clock Clock;

void DrawPixelbuffer(SDL_Texture* texture, SDL_Renderer* renderer,
	const uint32* pixelBuffer)
{
	//upload the pixel buffer to a texture
	void* pixels;
	int pitch;
	SDL_LockTexture(texture, nullptr, &pixels, &pitch);
	if (pitch == SCRWIDTH * 4)
		memcpy(pixels, pixelBuffer, SCRWIDTH * SCRHEIGHT * 4);
	else
	{
		const uint32* src = pixelBuffer;
		char* dst = (char*)pixels;
		for (int y = 0; y < SCRHEIGHT; ++y)
		{
			memcpy(dst, src, SCRWIDTH * 4);
			src += SCRWIDTH;
			dst += pitch;
		}
	}
	SDL_UnlockTexture(texture);

	//copy the texture to the frame buffer
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);

	//present the frame buffer on the screen
	SDL_RenderPresent(renderer);
}

#undef main
int main(int, char**)
{
	//initialize SDL
	SDL_Init(SDL_INIT_VIDEO);

	//create a window
	SDL_Window* window = SDL_CreateWindow(
		"title", 100, 100, SCRWIDTH, SCRHEIGHT, 0);
	if (window == nullptr)
		return 1;

	//create a renderer
	SDL_Renderer* renderer = SDL_CreateRenderer(
		window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr)
		return 2;

	//create a texture
	SDL_Texture* texture = SDL_CreateTexture(
		renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
		SCRWIDTH, SCRHEIGHT);
	if (texture == nullptr)
		return 3;

	//allocate a pixel buffer
	uint32* pixelBuffer = new uint32[SCRWIDTH * SCRHEIGHT];
	if (pixelBuffer == nullptr)
		return 4;

	//clear the pixel buffer
	memset(pixelBuffer, 0, SCRWIDTH * SCRHEIGHT * 4);

	//draw pixel buffer to the screen
	DrawPixelbuffer(texture, renderer, pixelBuffer);

	KinectScan app;
	app.SetPixelBuffer(pixelBuffer);
	app.Init();

	auto lastTime = Clock::now();

	bool running = true;
	while (running)
	{
		//get events
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				//pressing the x or pressing escape will quit the application
			case SDL_QUIT:
				running = false;
				break;

			case SDL_KEYDOWN:
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
					running = false;
				break;

			default: //ignore other events for this demo
				break;
			}
		}

		//calculate delta time
		const auto now = Clock::now();
		const auto duration = duration_cast<microseconds>(now - lastTime);
		const float deltaTime = duration.count() / 1000000.0f;
		lastTime = now;

		//update the application
		app.Tick(deltaTime);

		//draw pixel buffer to the screen
		DrawPixelbuffer(texture, renderer, pixelBuffer);
	}


	//clean up
	app.Shutdown();
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}


/*
//OpenCV Test Code

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main() {
	VideoCapture stream1(0);   //0 is the id of video device.0 if you have only one camera.

	if (!stream1.isOpened()) { //check if video device has been initialised
		cout << "cannot open camera";
	}

	//unconditional loop
	while (true) {
		Mat cameraFrame;
		stream1.read(cameraFrame);
		imshow("cam", cameraFrame);
		if (waitKey(30) >= 0)
			break;
	}
	return 0;
}*/
/*
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	VideoCapture cap(1); // open the video camera no. 0

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "ERROR: Cannot open the video file" << endl;
		return -1;
	}

	namedWindow("MyVideo", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

	cout << "Frame Size = " << dWidth << "x" << dHeight << endl;

	Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));

	VideoWriter oVideoWriter("C:/MyVideo.asf", CV_FOURCC('P', 'I', 'M', '1'), 20, frameSize, true); //initialize the VideoWriter object 

	if (!oVideoWriter.isOpened()) //if not initialize the VideoWriter successfully, exit the program
	{
		cout << "ERROR: Failed to write the video" << endl;
		return -1;
	}

	while (1)
	{

		Mat frame;
		//// estimates FPS of camera
		int64 start = cv::getTickCount();

		//Grab a frame
		cap >> frame;

		if (waitKey(3) >= 0) {
			break;
		}

		double fps = cv::getTickFrequency() / (cv::getTickCount() - start);
		std::cout << "FPS : " << fps << std::endl;
		////
		bool bSuccess = cap.read(frame); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "ERROR: Cannot read a frame from video file" << endl;
			break;
		}

		oVideoWriter.write(frame); //writer the frame into the file

		imshow("MyVideo", frame); //show the frame in "MyVideo" window

		if (waitKey(10) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}

	return 0;

}
*/