#include "Game.pch.h"
#include "ImgRecognition.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2\imgcodecs.hpp>
#include <opencv2/ml.hpp>

#include <iostream>
#include <vector>
#include "ComponentTools\ThreadPool.h"

// global variables ///////////////////////////////////////////////////////////////////////////////
const int MIN_CONTOUR_AREA = 5000;

const int RESIZED_IMAGE_WIDTH = 100;
const int RESIZED_IMAGE_HEIGHT = 100;

using namespace cv;
using namespace cv::ml;



class ContourWithData {
public:
	// member variables ///////////////////////////////////////////////////////////////////////////
	std::vector<cv::Point> ptContour;           // contour
	cv::Rect boundingRect;                      // bounding rect for contour
	float fltArea;                              // area of contour

	///////////////////////////////////////////////////////////////////////////////////////////////
	bool checkIfContourIsValid() {                              // obviously in a production grade program
		if (fltArea < MIN_CONTOUR_AREA) return false;           // we would have a much more robust function for 
		return true;                                            // identifying if a contour is valid !!
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	static bool sortByBoundingRectXPosition(const ContourWithData& cwdLeft, const ContourWithData& cwdRight) {      // this function allows us to sort
		return(cwdLeft.boundingRect.x < cwdRight.boundingRect.x);                                                   // the contours from left to right
	}

};


void ReadImageSVM(cv::Mat img, Ptr<SVM> svm, BrushSymbol& outSymbol, const std::function<void(bool success)>& aCallback)
{
	cv::Mat inputImageGray;
	cv::cvtColor(img, inputImageGray, cv::COLOR_BGR2GRAY); // convert to grayscale

	cv::Mat inputImageThresh;
	cv::adaptiveThreshold(inputImageGray, inputImageThresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 11, 2); // apply thresholding


	cv::Mat matThreshCopy = inputImageThresh.clone();              // make a copy of the thresh image, this in necessary b/c findContours modifies the image

	std::vector<ContourWithData> allContoursWithData;           // declare empty vectors,
	std::vector<ContourWithData> validContoursWithData;         // we will fill these shortly
	std::vector<std::vector<cv::Point> > ptContours;        // declare a vector for the contours
	std::vector<cv::Vec4i> v4iHierarchy;                    // declare a vector for the hierarchy (we won't use this in this program but this may be helpful for reference)

	cv::findContours(matThreshCopy, ptContours, v4iHierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < ptContours.size(); i++) {               // for each contour
		ContourWithData contourWithData;                                                    // instantiate a contour with data object
		contourWithData.ptContour = ptContours[i];                                          // assign contour to contour with data
		contourWithData.boundingRect = cv::boundingRect(contourWithData.ptContour);         // get the bounding rect
		contourWithData.fltArea = cv::contourArea(contourWithData.ptContour);               // calculate the contour area
		allContoursWithData.push_back(contourWithData);                                     // add contour with data object to list of all contours with data
	}

	for (int i = 0; i < allContoursWithData.size(); i++) {                      // for all contours
		if (allContoursWithData[i].checkIfContourIsValid()) {                   // check if valid
			validContoursWithData.push_back(allContoursWithData[i]);            // if so, append to valid contour list
		}
	}
	if (validContoursWithData.empty())
	{
		aCallback(false);
		return;
	}

	// sort contours from left to right
	std::sort(validContoursWithData.begin(), validContoursWithData.end(), ContourWithData::sortByBoundingRectXPosition);


	std::string strFinalString;         // declare final string, this will have the final number sequence by the end of the program

	for (int i = 0; i < validContoursWithData.size(); i++)
	{
		outSymbol.minX = validContoursWithData[i].boundingRect.x;
		outSymbol.minY = validContoursWithData[i].boundingRect.y;
		outSymbol.maxX = outSymbol.minX + validContoursWithData[i].boundingRect.width;
		outSymbol.maxY = outSymbol.minY + validContoursWithData[i].boundingRect.height;

		cv::Mat matROI = inputImageThresh(validContoursWithData[i].boundingRect);          // get ROI image of bounding rect

		cv::Mat matROIResized;
		cv::Size targetSize(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT);
		cv::resize(matROI, matROIResized, targetSize, 0, 0, cv::INTER_LINEAR | cv::INTER_NEAREST);

		cv::Mat matROIFloat;
		matROIResized.convertTo(matROIFloat, CV_32FC1);             // convert Mat to float, necessary for call to find_nearest

		cv::Mat matROIFlattenedFloat = matROIFloat.reshape(1, 1);


		float fltCurrentChar = svm->predict(matROIFlattenedFloat);

		strFinalString = strFinalString + char(int(fltCurrentChar));        // append current char to full string
		outSymbol.name = strFinalString;
	}

	aCallback(true);
}

void ImgRecognition::GenerateDataSVM()
{
	cv::Mat imgTrainingSymbols;
	cv::Mat imgGrayscale;
	cv::Mat imgBlurred;
	cv::Mat imgThresh;
	cv::Mat imgThreshCopy;

	std::vector<int> matClassificationInts; //int labels vector, will be used for creating the labels mat later
	cv::Mat matTrainingImagesAsFlattenedFloats;

	imgTrainingSymbols = cv::imread("Assets/Resources/training_symbols3.png");

	cv::cvtColor(imgTrainingSymbols, imgGrayscale, cv::COLOR_BGR2GRAY); // convert to grayscale

	cv::GaussianBlur(imgGrayscale, imgBlurred, cv::Size(5, 5), 0); // blur

	// invert colors so bg is black and characters are white
	cv::adaptiveThreshold(imgBlurred, imgThresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 11, 2);

	imgThreshCopy = imgThresh.clone();          // make a copy of the thresh image, this in necessary b/c findContours modifies the image

	std::vector<cv::Vec4i> v4iHierarchy;
	std::vector<std::vector<cv::Point> > ptContours;
	cv::findContours(imgThreshCopy, ptContours, v4iHierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	//Loops over the characters in the image and prompts the user to press a key which will assign labels to those areas
	for (int i = 0; i < ptContours.size(); i++)
	{
		if (cv::contourArea(ptContours[i]) > MIN_CONTOUR_AREA) // check contour is big enough to consider
		{
			cv::Rect boundingRect = cv::boundingRect(ptContours[i]);

			cv::Mat matROI = imgThresh(boundingRect);
			cv::imshow("matROI", matROI);
			cv::Mat matROIResized;
			cv::Size targetSize(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT);
			cv::resize(matROI, matROIResized, targetSize, 0, 0, cv::INTER_LINEAR | cv::INTER_NEAREST);

			matClassificationInts.push_back(cv::waitKey(0));

			cv::Mat matImageFloat;
			matROIResized.convertTo(matImageFloat, CV_32FC1);

			cv::Mat matImageFlattenedFloat = matImageFloat.reshape(1, 1);

			matTrainingImagesAsFlattenedFloats.push_back(matImageFlattenedFloat);
		}
	}

	cv::Mat matClassification(matClassificationInts);
	matClassification = matClassification.reshape(1, matClassificationInts.size());

	cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
	svm->setType(cv::ml::SVM::C_SVC);
	svm->setKernel(cv::ml::SVM::LINEAR);
	svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6));

	svm->train(matTrainingImagesAsFlattenedFloats, cv::ml::ROW_SAMPLE, matClassification);

	// Save the trained SVM to a file
	svm->save("svm_model.xml");
}



cv::Mat mat;
Ptr<SVM> svm;
std::function<void(bool aSucceeded)> callback;

void ImgRecognition::GetSymbolSVM(BrushSymbol& anOutSymbol, ID3D11Texture2D* aTexture, int width, int height, const std::function<void(bool aSucceeded)>& aCallback)
{
	D3D11_MAPPED_SUBRESOURCE mapped_resource = {};
	HRESULT result = DX11::Context->Map(aTexture, 0, D3D11_MAP_WRITE, 0, &mapped_resource);
	mat = cv::Mat(height, width, CV_8UC4, mapped_resource.pData, mapped_resource.RowPitch);
	DX11::Context->Unmap(aTexture, 0);
	std::string outString;

	callback = aCallback;

	svm = SVM::load("svm_model.xml");
	Engine::GetInstance()->GetThreadPool()->QueueJob([&] { ReadImageSVM(mat, svm, anOutSymbol, callback); });
}
