#include <iostream>
#include <fstream>
#include <sstream>
#include "aruco.h"
#include "cvdrawingutils.h"
using namespace cv;
using namespace aruco;

string TheInputVideo;
string TheIntrinsicFile;
float TheMarkerSize=-1;
int ThePyrDownLevel;
MarkerDetector MDetector;
VideoCapture TheVideoCapturer;
vector<Marker> TheMarkers;
Mat TheInputImage,TheInputImageCopy;
CameraParameters TheCameraParameters;
void cvTackBarEvents(int pos,void*);
bool readCameraParameters(string TheIntrinsicFile,CameraParameters &CP,Size size);

pair<double,double> AvrgTime(0,0) ;//determines the average time required for detection
double ThresParam1,ThresParam2;
int iThresParam1,iThresParam2;
int waitTime=0;

bool readArguments ( int argc,char **argv )
{
    if (argc<2) {
        cerr<<"Invalid number of arguments"<<endl;
        cerr<<"Usage: (in.avi|live) [intrinsics.yml] [size]"<<endl;
        return false;
    }
    TheInputVideo=argv[1];
    if (argc>=3)
        TheIntrinsicFile=argv[2];
    if (argc>=4)
        TheMarkerSize=atof(argv[3]);
    if (argc==3)
        cerr<<"NOTE: You need makersize to see 3d info!!!!"<<endl;
    return true;
}

int main(int argc,char **argv)
{
    try
    {
        if (readArguments (argc,argv)==false) {
            return 0;
        }
        if (TheInputVideo=="live") {
            TheVideoCapturer.open(0);
            waitTime=10;
        }
        else  TheVideoCapturer.open(TheInputVideo);
        //check video is open
        if (!TheVideoCapturer.isOpened()) {
            cerr<<"Could not open video"<<endl;
            return -1;
        }

        //read first image to get the dimensions
        TheVideoCapturer >> TheInputImage;

        //read camera parameters if passed
        if (TheIntrinsicFile!="") {
            TheCameraParameters.readFromXMLFile(TheIntrinsicFile);
            TheCameraParameters.resize(TheInputImage.size());
        }
        //Configure other parameters
        if (ThePyrDownLevel>0)
            MDetector.pyrDown(ThePyrDownLevel);


        //Create gui

        cv::namedWindow("thres",1);
        cv::namedWindow("in",1);
        MDetector.getThresholdParams( ThresParam1,ThresParam2);
        MDetector.setCornerRefinementMethod(MarkerDetector::LINES);
        iThresParam1=ThresParam1;
        iThresParam2=ThresParam2;
        cv::createTrackbar("ThresParam1", "in",&iThresParam1, 13, cvTackBarEvents);
        cv::createTrackbar("ThresParam2", "in",&iThresParam2, 13, cvTackBarEvents);

        char key=0;
        int index=0;
		bool initialized = false;
		while ( key != 27 && !TheVideoCapturer.grab() )
			key=cv::waitKey(waitTime);
        while ( key!=27 )
        {
			if ( !TheVideoCapturer.grab() ) {
				key=cv::waitKey(waitTime);
				continue;
			}
            TheVideoCapturer.retrieve( TheInputImage);
			cv::resize(TheInputImage, TheInputImage, cv::Size(640, 360));

            index++; //number of images captured
            double tick = (double)getTickCount();//for checking the speed
            //Detection of markers in the image passed
            MDetector.detect(TheInputImage,TheMarkers,TheCameraParameters,TheMarkerSize);
            //chekc the speed by calculating the mean speed of all iterations
            AvrgTime.first+=((double)getTickCount()-tick)/getTickFrequency();
            AvrgTime.second++;
 //           cout<<"Time detection="<<1000*AvrgTime.first/AvrgTime.second<<" milliseconds"<<endl;
			if (!initialied && key == 13) {
				// detect the markers and contruct the 3D scene.
				construct_3D(TheMarkers);
			}
            //print marker info and draw the markers in image
            TheInputImage.copyTo(TheInputImageCopy);
            for (unsigned int i=0;i<TheMarkers.size();i++) {
                TheMarkers[i].draw(TheInputImageCopy,Scalar(0,0,255),1);
            }
            //print other rectangles that contains no valid markers
       /**     for (unsigned int i=0;i<MDetector.getCandidates().size();i++) {
                aruco::Marker m( MDetector.getCandidates()[i],999);
                m.draw(TheInputImageCopy,cv::Scalar(255,0,0));
            }*/

            //draw a 3d cube in each marker if there is 3d info
            if (  TheCameraParameters.isValid())
                for (unsigned int i=0;i<TheMarkers.size();i++) {
                    CvDrawingUtils::draw3dCube(TheInputImageCopy,TheMarkers[i],TheCameraParameters);
                    CvDrawingUtils::draw3dAxis(TheInputImageCopy,TheMarkers[i],TheCameraParameters);
                }
            //DONE! Easy, right?
            //show input with augmented information and  the thresholded image
            cv::imshow("in",TheInputImageCopy);
            cv::imshow("thres",MDetector.getThresholdedImage());

            key=cv::waitKey(waitTime);//wait for key to be pressed
        }

    } catch (std::exception &ex)
    {
        cout<<"Exception :"<<ex.what()<<endl;
    }

}
/************************************
 *
 *
 *
 *
 ************************************/

void cvTackBarEvents(int pos,void*)
{
    if (iThresParam1<3) iThresParam1=3;
    if (iThresParam1%2!=1) iThresParam1++;
    if (ThresParam2<1) ThresParam2=1;
    ThresParam1=iThresParam1;
    ThresParam2=iThresParam2;
    MDetector.setThresholdParams(ThresParam1,ThresParam2);
//recompute
    MDetector.detect(TheInputImage,TheMarkers,TheCameraParameters);
    TheInputImage.copyTo(TheInputImageCopy);
    for (unsigned int i=0;i<TheMarkers.size();i++)	TheMarkers[i].draw(TheInputImageCopy,Scalar(0,0,255),1);
    //print other rectangles that contains no valid markers
    /*for (unsigned int i=0;i<MDetector.getCandidates().size();i++) {
        aruco::Marker m( MDetector.getCandidates()[i],999);
        m.draw(TheInputImageCopy,cv::Scalar(255,0,0));
    }*/

//draw a 3d cube in each marker if there is 3d info
    if (TheCameraParameters.isValid())
        for (unsigned int i=0;i<TheMarkers.size();i++)
            CvDrawingUtils::draw3dCube(TheInputImageCopy,TheMarkers[i],TheCameraParameters);

    cv::imshow("in",TheInputImageCopy);
    cv::imshow("thres",MDetector.getThresholdedImage());
}

void construct_3D(vector<Marker> markers) {

}
