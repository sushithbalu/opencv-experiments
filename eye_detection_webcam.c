#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <assert.h>

/*
status:Incomplete
TODO:cpu usage issue, code cleanup
 */

typedef void (*callback_prototype)(IplImage *, CvHaarClassifierCascade *, 
		CvMemStorage *);

//to locate cascade use: locate haarcascade_eye.xml
const char* eye_cascade_name = "/usr/share/opencv/haarcascades/haarcascade_eye.xml";

char *window_name = "result";

void detect_eye(IplImage* frame, CvHaarClassifierCascade *cascade, 
		CvMemStorage * storage)
{
	cvClearMemStorage(storage);
	

	CvSeq *eyes = cvHaarDetectObjects(frame, cascade, storage, 1.1, 3,
			CV_HAAR_DO_CANNY_PRUNING, cvSize(10, 10), cvSize(30, 30));
	CvPoint cvp;

	int LINE_TYPE = CV_AA;
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1.0f, 1.0f, 0.0f, 1, CV_AA);

	for(int i = 0; i < (eyes ? eyes->total: 0); i++) {

		CvRect *r  = (CvRect *)cvGetSeqElem(eyes, i);

		cvp.x = r->x + cvRound(r->width/2);
		cvp.y = r->y + cvRound(r->height/2);

		cvCircle(frame, cvp, cvRound(r->width/2), CV_RGB(255,0,0), 1,
				LINE_TYPE, 0 );
		cvPutText(frame, "open", cvPoint(10, 20), &font, CV_RGB(255,0,0));
		//cvCircle(frame, cvp, 1, CV_RGB(255,0,0), 1, LINE_TYPE, 0 );
	}

	cvShowImage( window_name, frame );
}

void process_video(
		CvHaarClassifierCascade *cascade, CvMemStorage *storage)
{           

	//initialize cascade
	cascade = (CvHaarClassifierCascade *)cvLoad(eye_cascade_name, 0, 0, 0);
	storage = cvCreateMemStorage(0);	

	assert(cascade);
	assert(storage);

	// Initialize camera
	CvCapture *capture = 0;
	
	// , set -1 for first available cam
	capture = cvCaptureFromCAM(-1);
	if (!capture) 
	{
		fprintf(stderr, "Error: webcam initialization failed\n" );
		return;
	}
	
	// set frame resolution	
//	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH, 640);
//	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 480);
//	cvSetCaptureProperty(capture, CV_CAP_PROP_FPS , 10);

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

		detect_eye(frame,cascade ,storage);

		//cvShowImage(window_name, processed_frame);
		//cvReleaseImage(&processed_frame);
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

	process_video(cascade, storage);

	return 0;
}
