#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <assert.h>

/*
status:Incomplete
TODO:cpu usage issue, code cleanup
 */

//to locate cascade use: locate haarcascade_eye.xml
const char* eye_cascade_name = "/usr/share/opencv/haarcascades/haarcascade_eye.xml";
const char* face_cascade_name = "/usr/share/opencv/haarcascades/haarcascade_frontalface_default.xml";

const char* window_name = "result";

void detect_face(IplImage* frame, CvHaarClassifierCascade* eye_cascade, 
		CvHaarClassifierCascade* face_cascade, CvMemStorage * storage)
{
	CvPoint cvp, cvp_face;
	CvFont font;
	int LINE_TYPE = CV_AA;
	cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1.0f, 1.0f, 0.0f, 1, LINE_TYPE);

	cvClearMemStorage(storage);

	CvSeq *faces = cvHaarDetectObjects(frame, face_cascade, storage, 1.2, 9,
			CV_HAAR_SCALE_IMAGE, cvSize(100, 100), cvSize(0, 0));
	if(faces->total == 0) 
		return;
	
	for(int j = 0; j < (faces ? faces->total : 0); j++){

		CvRect *r = (CvRect*)cvGetSeqElem(faces, 0);
		cvp_face.x = r->x + r->width;
		cvp_face.y = r->y + r->height;
		cvRectangle(frame, cvPoint(r->x, r->y), cvp_face, CV_RGB(255, 0, 0), 1,
			 LINE_TYPE, 0);

		cvSetImageROI(frame, cvRect(r->x, r->y + (r->height/5.0), r->width, r->height/3.0));

		CvSeq *eyes = cvHaarDetectObjects(frame, eye_cascade, storage, 1.1, 8,
				CV_HAAR_SCALE_IMAGE, cvSize(20, 20), cvSize(0, 0));

		for(int i = 0; i < (eyes ? eyes->total: 0); i++) {

			r  = (CvRect *)cvGetSeqElem(eyes, i);

			cvp.x = r->x + cvRound(r->width/2);
			cvp.y = r->y + cvRound(r->height/2);

			cvCircle(frame, cvp, cvRound(r->width/2), CV_RGB(255,0,0), 1,
					LINE_TYPE, 0 );
			cvPutText(frame, "open", cvPoint(r->x, r->y), &font, CV_RGB(0,255,0));
		}
		cvResetImageROI(frame);
	}

	cvShowImage( window_name, frame );
}

void process_video(CvHaarClassifierCascade *eye_cascade, 
		CvHaarClassifierCascade *face_cascade, CvMemStorage *storage)
{           


	// Initialize camera
	CvCapture *capture = 0;

	// , set -1 for first available cam
	capture = cvCaptureFromCAM(-1);
	assert(capture);

	// set frame resolution	
		cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH, 640);
		cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 480);

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

		detect_face(frame,eye_cascade, face_cascade, storage);
		key = cvWaitKey(40);
	}

	if(storage) 
		cvReleaseMemStorage(&storage);
	if(eye_cascade)
		cvReleaseHaarClassifierCascade(&eye_cascade);
	if(face_cascade)
		cvReleaseHaarClassifierCascade(&face_cascade);

	cvDestroyWindow(window_name);
	cvReleaseCapture(&capture);
}

int main( int argc, char **argv )
{
	CvHaarClassifierCascade * eye_cascade = 0;
	CvHaarClassifierCascade * face_cascade = 0;
	CvMemStorage * storage = 0;

	//initialize cascade
	eye_cascade = (CvHaarClassifierCascade *)cvLoad(eye_cascade_name, 0, 0, 0);
	face_cascade = (CvHaarClassifierCascade *)cvLoad(face_cascade_name, 0, 0, 0);
	storage = cvCreateMemStorage(0);	

	assert(eye_cascade);
	assert(face_cascade);
	assert(storage);

	process_video(eye_cascade, face_cascade, storage);

	return 0;
}
