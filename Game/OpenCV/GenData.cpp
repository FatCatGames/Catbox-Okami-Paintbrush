#include "Game.pch.h"
#include "GenData.h"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>

#include<iostream>
#include<vector>

// global variables ///////////////////////////////////////////////////////////////////////////////
const int MIN_CONTOUR_AREA = 100;

const int RESIZED_IMAGE_WIDTH = 50;
const int RESIZED_IMAGE_HEIGHT = 30;

///////////////////////////////////////////////////////////////////////////////////////////////////
void GenData::GenerateData()
{
	cv::Mat imgTrainingSymbols;
	cv::Mat imgGrayscale;
	cv::Mat imgBlurred;
	cv::Mat imgThresh;
	cv::Mat imgThreshCopy;

	std::vector<std::vector<cv::Point> > ptContours;
	std::vector<cv::Vec4i> v4iHierarchy;

	cv::Mat matClassificationInts;      // these are our training classifications, note we will have to perform some conversions before writing to file later

	// these are our training images, due to the data types that the KNN object KNearest requires, we have to declare a single Mat,
	// then append to it as though it's a vector, also we will have to perform some conversions before writing to file later
	cv::Mat matTrainingImagesAsFlattenedFloats;

	std::vector<int> intValidSymbols = { 'o', '-', 'b', 'c' };

	imgTrainingSymbols = cv::imread("Assets/Resources/training_symbols3.png");

	if (imgTrainingSymbols.empty())
	{
		printerror("error: image not read from file");
		return;
	}

	cv::cvtColor(imgTrainingSymbols, imgGrayscale, cv::COLOR_BGR2GRAY); // convert to grayscale

	cv::GaussianBlur(imgGrayscale,
		imgBlurred,
		cv::Size(5, 5),                         // smoothing window width and height in pixels
		0);                                     // sigma value, determines how much the image will be blurred, zero makes function choose the sigma value

												// filter image from grayscale to black and white
	cv::adaptiveThreshold(imgBlurred,           // input image
		imgThresh,                              // output image
		255,                                    // make pixels that pass the threshold full white
		cv::ADAPTIVE_THRESH_GAUSSIAN_C,         // use gaussian rather than mean, seems to give better results
		cv::THRESH_BINARY_INV,                  // invert so foreground will be white, background will be black
		11,                                     // size of a pixel neighborhood used to calculate threshold value
		2);                                     // constant subtracted from the mean or weighted mean

   //cv::imshow("imgThresh", imgThresh);         // show threshold image for reference

	imgThreshCopy = imgThresh.clone();          // make a copy of the thresh image, this in necessary b/c findContours modifies the image

	cv::findContours(imgThreshCopy,             // input image, make sure to use a copy since the function will modify this image in the course of finding contours
		ptContours,                             // output contours
		v4iHierarchy,                           // output hierarchy
		cv::RETR_EXTERNAL,                      // retrieve the outermost contours only
		cv::CHAIN_APPROX_SIMPLE);               // compress horizontal, vertical, and diagonal segments and leave only their end points

	for (int i = 0; i < ptContours.size(); i++) {                           // for each contour
		if (cv::contourArea(ptContours[i]) > MIN_CONTOUR_AREA) {                // if contour is big enough to consider
			cv::Rect boundingRect = cv::boundingRect(ptContours[i]);                // get the bounding rect

			cv::rectangle(imgTrainingSymbols, boundingRect, cv::Scalar(0, 0, 255), 2);      // draw red rectangle around each contour as we ask user for input

			cv::Mat matROI = imgThresh(boundingRect);           // get ROI image of bounding rect

			cv::Mat matROIResized;
			cv::resize(matROI, matROIResized, cv::Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));     // resize image, this will be more consistent for recognition and storage

			cv::imshow("matROI", matROI);                               // show ROI image for reference
			cv::imshow("matROIResized", matROIResized);                 // show resized ROI image for reference
			//cv::imshow("imgTrainingNumbers", imgTrainingSymbols);       // show training numbers image, this will now have red rectangles drawn on it

			int intChar = cv::waitKey(0);

			if (std::find(intValidSymbols.begin(), intValidSymbols.end(), intChar) != intValidSymbols.end()) {     // else if the char is in the list of chars we are looking for . . .

				matClassificationInts.push_back(intChar);       // append classification char to integer list of chars

				cv::Mat matImageFloat;                          // now add the training image (some conversion is necessary first) . . .
				matROIResized.convertTo(matImageFloat, CV_32FC1);       // convert Mat to float

				cv::Mat matImageFlattenedFloat = matImageFloat.reshape(1, 1);       // flatten

				matTrainingImagesAsFlattenedFloats.push_back(matImageFlattenedFloat);       // add to Mat as though it was a vector, this is necessary due to the
																							// data types that KNearest.train accepts
			}
		}
	}

	printmsg("training complete");

	// save classifications to file ///////////////////////////////////////////////////////

	cv::FileStorage fsClassifications("Assets/Resources/classifications.xml", cv::FileStorage::WRITE);           // open the classifications file

	if (fsClassifications.isOpened() == false) {                                                        // if the file was not opened successfully
		printerror("error, unable to open training classifications file, exiting program");        // show error message
		return;                                                                                      // and exit program
	}

	fsClassifications << "classifications" << matClassificationInts;        // write classifications into classifications section of classifications file
	fsClassifications.release();                                            // close the classifications file

	// save training images to file ///////////////////////////////////////////////////////

	cv::FileStorage fsTrainingImages("Assets/Resources/images.xml", cv::FileStorage::WRITE);         // open the training images file

	if (fsTrainingImages.isOpened() == false) {                                                 // if the file was not opened successfully
		printerror("error, unable to open training images file, exiting program");         // show error message
		return;                                                                              // and exit program
	}

	fsTrainingImages << "images" << matTrainingImagesAsFlattenedFloats;         // write training images into images section of images file
	fsTrainingImages.release();                                                 // close the training images file
}

///////////////////////////////////////////////////////////////////////////////////////////////////
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


///////////////////////////////////////////////////////////////////////////////////////////////////
bool ReadImage(cv::Mat anImage, BrushSymbol& outSymbol)
{
	std::vector<ContourWithData> allContoursWithData;           // declare empty vectors,
	std::vector<ContourWithData> validContoursWithData;         // we will fill these shortly

																// read in training classifications ///////////////////////////////////////////////////

	cv::Mat matClassificationInts;      // we will read the classification numbers into this variable as though it is a vector

	cv::FileStorage fsClassifications("Assets/Resources/classifications.xml", cv::FileStorage::READ);        // open the classifications file

	if (fsClassifications.isOpened() == false) {                                                    // if the file was not opened successfully
		printerror("error, unable to open training classifications file, exiting program");    // show error message
		return false;                                                                                  // and exit program
	}

	fsClassifications["classifications"] >> matClassificationInts;      // read classifications section into Mat classifications variable
	fsClassifications.release();                                        // close the classifications file

																		// read in training images ////////////////////////////////////////////////////////////

	cv::Mat matTrainingImagesAsFlattenedFloats;         // we will read multiple images into this single image variable as though it is a vector

	cv::FileStorage fsTrainingImages("Assets/Resources/images.xml", cv::FileStorage::READ);          // open the training images file

	if (fsTrainingImages.isOpened() == false) {                                                 // if the file was not opened successfully
		printerror("error, unable to open training images file, exiting program");         // show error message
		return false;                                                                              // and exit program
	}

	fsTrainingImages["images"] >> matTrainingImagesAsFlattenedFloats;           // read images section into Mat training images variable
	fsTrainingImages.release();                                                 // close the traning images file
																				// train //////////////////////////////////////////////////////////////////////////////

	cv::Ptr<cv::ml::KNearest>  kNearest(cv::ml::KNearest::create());            // instantiate the KNN object

																				// finally we get to the call to train, note that both parameters have to be of type Mat (a single Mat)
																				// even though in reality they are multiple images / numbers
	kNearest->train(matTrainingImagesAsFlattenedFloats, cv::ml::ROW_SAMPLE, matClassificationInts);

	// test ///////////////////////////////////////////////////////////////////////////////

	cv::Mat matTestingNumbers = anImage;            // read in the test numbers image

	if (matTestingNumbers.empty()) {                                // if unable to open image
		printerror("error: image not read from file");         // show error message on command line
		return false;                                                  // and exit program
	}

	cv::Mat matGrayscale;           //
	cv::Mat matBlurred;             // declare more image variables
	cv::Mat matThresh;              //
	cv::Mat matThreshCopy;          //

	cv::cvtColor(matTestingNumbers, matGrayscale, cv::COLOR_BGR2GRAY);         // convert to grayscale

																		// blur
	cv::GaussianBlur(matGrayscale,              // input image
		matBlurred,                // output image
		cv::Size(5, 5),            // smoothing window width and height in pixels
		0);                        // sigma value, determines how much the image will be blurred, zero makes function choose the sigma value

								   // filter image from grayscale to black and white
	cv::adaptiveThreshold(matBlurred,                           // input image
		matThresh,                            // output image
		255,                                  // make pixels that pass the threshold full white
		cv::ADAPTIVE_THRESH_GAUSSIAN_C,       // use gaussian rather than mean, seems to give better results
		cv::THRESH_BINARY_INV,                // invert so foreground will be white, background will be black
		11,                                   // size of a pixel neighborhood used to calculate threshold value
		2);                                   // constant subtracted from the mean or weighted mean

	matThreshCopy = matThresh.clone();              // make a copy of the thresh image, this in necessary b/c findContours modifies the image

	std::vector<std::vector<cv::Point> > ptContours;        // declare a vector for the contours
	std::vector<cv::Vec4i> v4iHierarchy;                    // declare a vector for the hierarchy (we won't use this in this program but this may be helpful for reference)

	cv::findContours(matThreshCopy,             // input image, make sure to use a copy since the function will modify this image in the course of finding contours
		ptContours,                             // output contours
		v4iHierarchy,                           // output hierarchy
		cv::RETR_EXTERNAL,                      // retrieve the outermost contours only
		cv::CHAIN_APPROX_SIMPLE);               // compress horizontal, vertical, and diagonal segments and leave only their end points

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
	// sort contours from left to right
	std::sort(validContoursWithData.begin(), validContoursWithData.end(), ContourWithData::sortByBoundingRectXPosition);

	std::string strFinalString;         // declare final string, this will have the final number sequence by the end of the program


	for (int i = 0; i < validContoursWithData.size(); i++) {            // for each contour

																		// draw a green rect around the current char
		cv::rectangle(matTestingNumbers,                            // draw rectangle on original image
			validContoursWithData[i].boundingRect,        // rect to draw
			cv::Scalar(0, 255, 0),                        // green
			2);                                           // thickness

		outSymbol.minX = validContoursWithData[i].boundingRect.x;
		outSymbol.minY = validContoursWithData[i].boundingRect.y;
		outSymbol.maxX = outSymbol.minX + validContoursWithData[i].boundingRect.width;
		outSymbol.maxY = outSymbol.minY + validContoursWithData[i].boundingRect.height;

		cv::Mat matROI = matThresh(validContoursWithData[i].boundingRect);          // get ROI image of bounding rect

		cv::Mat matROIResized;
		cv::resize(matROI, matROIResized, cv::Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));     // resize image, this will be more consistent for recognition and storage

		cv::Mat matROIFloat;
		matROIResized.convertTo(matROIFloat, CV_32FC1);             // convert Mat to float, necessary for call to find_nearest

		cv::Mat matROIFlattenedFloat = matROIFloat.reshape(1, 1);

		cv::Mat matCurrentChar(0, 0, CV_32F);

		kNearest->findNearest(matROIFlattenedFloat, 1, matCurrentChar);     // finally we can call find_nearest !!!

		float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);

		strFinalString = strFinalString + char(int(fltCurrentChar));        // append current char to full string
		outSymbol.name = strFinalString;
	}

	// printmsg("numbers read = " + strFinalString);       // show the full string

	 //cv::imshow("matTestingNumbers", matTestingNumbers);     // show input image with green boxes drawn around found digits
	 //outSymbol = strFinalString;
	return true;
}



BrushSymbol GenData::GetSymbol(ID3D11Texture2D* aTexture, int width, int height)
{
	D3D11_MAPPED_SUBRESOURCE mapped_resource = {};
	HRESULT result = DX11::Context->Map(aTexture, 0, D3D11_MAP_WRITE, 0, &mapped_resource);
	cv::Mat mat(height, width, CV_8UC4, mapped_resource.pData, mapped_resource.RowPitch);
	DX11::Context->Unmap(aTexture, 0);
	std::string outString;
	BrushSymbol symbol;
	ReadImage(mat, symbol);
	return symbol;
}
