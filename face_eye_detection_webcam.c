#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <assert.h>

/*
status:Incomplete
TODO:cpu usage issue, code cleanup
 */

typedef IplImage* (*callback_prototype)(IplImage*, CvHaarClassifierCascade *, 
		CvMemStorage *);

//to locate cascade use: locate haarcascade_eye.xml
const char* eye_cascade_name = "/usr/share/opencv/haarcascades/haarcascade_eye.xml";


IplImage* detect_eye(IplImage* frame, CvHaarClassifierCascade *cascade, 
		CvMemStorage * storage)
{
	cvClearMemStorage(storage);

	IplImage * gray_frame = 0;
	gray_frame = cvCreateImage(cvSize(frame->width, frame->height), frame->depth, 1);
	if (!gray_frame)
	{
		fprintf(stderr, "cvCreateImage failed!\n" );
		return NULL;
	}

	cvCvtColor(frame, gray_frame, CV_RGB2GRAY);

	CvSeq *eyes = cvHaarDetectObjects(gray_frame, cascade, storage, 1.1, 2,
			CV_HAAR_DO_CANNY_PRUNING, cvSize(20, 20), cvSize(0, 0));
	CvPoint cvp;
	int LINE_TYPE = CV_AA;

	for(int i = 0; i < (eyes ? eyes->total: 0); i++) {

		CvRect *r  = (CvRect *)cvGetSeqElem(eyes, i);

		cvp.x = r->x + r->width/2;
		cvp.y = r->y + r->height/2;

		cvCircle(gray_frame, cvp, r->width/2, CV_RGB(255,0,0), 1,
				LINE_TYPE, 0 );
		cvCircle(gray_frame, cvp, 1, CV_RGB(255,0,0), 1, LINE_TYPE, 0 );
	}

	return gray_frame;
}

void process_video(callback_prototype custom_callback, 
		CvHaarClassifierCascade *cascade, CvMemStorage *storage)
{           
	char *window_name = "result";

	//initialize cascade
	cascade = (CvHaarClassifierCascade *)cvLoad(eye_cascade_name, 0, 0, 0);
	storage = cvCreateMemStorage(0);	

	assert(cascade);
	assert(storage);

	// Initialize camera
	CvCapture *capture = 0;

	capture = cvCaptureFromCAM(0);
	if (!capture) 
	{
		fprintf(stderr, "Error: webcam initialization failed\n" );
		return;
	}
	
	// set frame resolution	
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH, 320);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 240);

	// Create a window for the video 
	cvNamedWindow(window_name, CV_WINDOW_AUTOSIZE);

	IplImage* frame = 0;
	char key = 0;
	while (key != 27) {  // on ESC   

		frame = cvQueryFrame(capture);

		if(!frame) {
			fprintf( stderr, "Error: cvQueryFrame failed\n" );
			break;
		}

		IplImage* processed_frame = (*custom_callback)(frame,cascade ,storage);

		cvShowImage(window_name, processed_frame);
		cvReleaseImage(&processed_frame);
		key = cvWaitKey(10);
	}

	if(storage) 
		cvReleaseMemStorage(&storage);
	if(cascade)
		cvReleaseHaarClassifierCascade(&cascade);


	cvDestroyWindow(window_name);
	cvReleaseCapture(&capture);
}

int main( int argc, char **argv )
{
	CvHaarClassifierCascade * cascade = 0;
	CvMemStorage * storage = 0;

	process_video(detect_eye, cascade, storage);

	return 0;
}
