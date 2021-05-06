#include <iostream>
#include "OCR_Func.h"

using namespace std;

int main(int argc, char** argv)
{
	bool isROIbox(true);
	// Initialize OCR
	cv::VideoCapture cap(0);
	cap.set(cv::CAP_PROP_FPS, 30);
	int resize_factor(1);

	if (!cap.isOpened()) {
		printf("Can't open the capture board\n"); return -1;
	}


	cout << "    Initialize OCR" << endl;
	Config config;
	config.loadConfig();
	ImageProcessor proc(config);
	proc.DebugWindow();
	proc.DebugPower();
	proc.DebugDigits();
	proc.DebugOCR();
	KNearestOcr ocr(config);
	if (!ocr.loadTrainingData()) { cout << "      Failed to load OCR training data" << endl; return 1; }
	cout << "      OCR training data loaded." << endl;

	ROIBox* roi = new ROIBox();
	bool pushcrop = false;

	int roi_cols(0), roi_rows(0);
	// Set ROI
	std::cout << "    >> Select 4 ROI box to OCR, Last ROI box will check the On/Off" << std::endl;
	std::cout << "       1. Tube Voltage, 2. Tube Current, 3. DAP, 4. Power"          << std::endl;
	cv::Mat img;
	while (1) {
		cap >> img;
		cv::Mat imgCopy, imgCrop;
//		cv::resize(img, img2, cv::Size(floor(img.cols/resize_factor), floor(img.rows/resize_factor)));
		img.copyTo(imgCopy);

		cv::setMouseCallback("Select ROI", onMouseCropImage, &imgCopy);

		if (clicked) pushcrop = true;
		if (cropRect.width > 0 && clicked == false) {
			imgCrop = img(cropRect);
			if (pushcrop) {
				std::cout << "      push ROI box" << std::endl;
				blackBox.push_back(cropRect);
				pushcrop = false;
			}
		}
		else img.copyTo(imgCrop);

		cv::rectangle(imgCopy, P1, P2, CV_RGB(255,255,0), 2);

		cv::putText(imgCopy, "Resolution: "+std::to_string(img.cols)+"x"+std::to_string(img.rows),
				cv::Point(10,20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(60000), 1);

		cv::imshow("crop", imgCrop);
		cv::imshow("Select ROI", imgCopy);

		int key = cv::waitKey(1);
		if (key == 'q') break;
	}
	cv::destroyAllWindows();
	roi->setROIBox(blackBox);

	ofstream ofs("ocrtest.txt");

	cv::Mat imgNone = cv::Mat::zeros(img.rows,img.cols,CV_8UC3);
	int frameNo(0);
	while(1) {
		Timer ocr_timer;
		ocr_timer.start();
		cap >> img;
		cv::Mat imgCopy(imgNone);
//		cv::resize(img, img2, cv::Size(floor(img.cols/resize_factor), floor(img.rows/resize_factor)));
		// Read ROI box screen
		for (int k=0; k<blackBox.size(); k++) {
			for (int i=blackBox[k].x; i< blackBox[k].x + blackBox[k].width; i++) {
				for (int j=blackBox[k].y; j<blackBox[k].y + blackBox[k].height; j++) {
					imgCopy.at<cv::Vec3b>(j,i) = img.at<cv::Vec3b>(j,i);
				}
			}
		}
		proc.SetInput(imgCopy);
		proc.Process(roi);
		bool powerOn = proc.GetPowerOn();
		string voltage  = ocr.recognize(proc.GetOutputkV());
		string current  = ocr.recognize(proc.GetOutputmA());
		string dapRate  = ocr.recognize(proc.GetOutputDAP());
		string fluoTime = ocr.recognize(proc.GetOutputTime());
		ocr_timer.stop();
		cout << "Frame #                    : " << frameNo++ << endl;
		cout << "Tube Voltage   (kVp)       : " << voltage << endl;
		cout << "Tube Current   (mA)        : " << current << endl;
		cout << "DAP rate       (Gy-cm2/s)  : " << dapRate << endl;
		cout << "Fluo Signal    (1:ON/0:OFF): " << powerOn << endl;
//		cout << "Fluo Time      (mm:ss)     : " << fluoTime.substr(0,2) << ":" << fluoTime.substr(2,4) << endl;
		cout << "Frame Time     (s)         : " << ocr_timer.time() << endl << endl;
	}

	return 0;
}
