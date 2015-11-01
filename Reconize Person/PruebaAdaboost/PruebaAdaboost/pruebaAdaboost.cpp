#include "opencv2/imgproc/imgproc.hpp" 
#include "opencv2/highgui/highgui.hpp" 
#include <stdlib.h> 
#include <stdio.h> 
#include <iostream> // use std
#include <vector>  // std::vector
#include "opencv2/opencv.hpp" 
#include < opencv/highgui.h > 
#include <opencv2/legacy/compat.hpp> 
#include <opencv2/ml/ml.hpp>

//using namespace std;
int main(){
	//Step 1. Prepare the files.
	
	//Step 2. Opening the file
	//1. Declare a structure to keep the data
	CvMLData cvml;
	//2. Read the file
	cvml.read_csv("samples.csv");
	//3. Indicate which column is the response
	cvml.set_response_idx(0);

	//Step 3. Splitting the samples
	//1. Select 11 for the training
	CvTrainTestSplit cvtts(11, true);
	//2. Assign the division to the data
	cvml.set_train_test_split(&cvtts);
	printf("\nsplit_part_mode %d\n", cvtts.train_sample_part_mode);

	//Step 4. The training process
	//1. Declare the classifier
	CvBoost boost;
	//2. Train it with 1 or n features
	boost.train(&cvml, CvBoostParams(CvBoost::REAL, 1, 0, 1, false, 0), false);

	//Step 5.
	// 1. Declare a couple of vectors to save the predictions of each sample
	std::vector <float> train_responses, test_responses;
	// 2. Calculate the training error
	float fl1 = boost.calc_error(&cvml,CV_TRAIN_ERROR,&train_responses);
	// 3. Calculate the test error
	float fl2 = boost.calc_error(&cvml,CV_TEST_ERROR,&test_responses);

	//Print error train
	//ERROR TRAIN -> es el error en el proceso de formación y es el número de muestras de entrenamiento 
	//que se clasifican de forma incorrecta (llamados falsos positivos + falsos negativos - http://en.wikipedia.org/wiki/Confusion_matrix).
	printf("train %.7f", fl1);

	printf("\ntest %.7f", fl2);
	
	
	
	//Step 6. Save your classifier!!
	// Save the trained classifier
	boost.save("./trained_boost.xml", "boost");

	//Step 7. Compiling the whole code

	return EXIT_SUCCESS;

}