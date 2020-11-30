#include <iostream>
#include "daal.h"
#include "service.h"

using namespace std;
using namespace daal;
using namespace daal::algorithms::linear_regression;

// Input data set parameters 
string trainedModelFileName = "./regression_test_data.txt";
string testDatasetFileName = "./test_cases.csv";

const size_t nFeatures = 10; // Number of features in training and testing data sets 
const size_t nDependentVariables = 1;  // Number of dependent variables that correspond to each observation 

ModelPtr buildModel();
void testModel(ModelPtr&);

int main(int argc, char* argv[])
{
	checkArguments(argc, argv, 2, &trainedModelFileName, &testDatasetFileName);

	ModelPtr builtModel = buildModel();
	testModel(builtModel);

	return 0;
}

ModelPtr buildModel()
{
	// Initialize FileDataSource<CSVFeatureManager> to retrieve the test data from a .csv file 
	FileDataSource<CSVFeatureManager> modelSource(trainedModelFileName, DataSource::doAllocateNumericTable, DataSource::doDictionaryFromContext);

	// Create Numeric Table for beta coefficients 
	NumericTablePtr beta(new HomogenNumericTable<>(nFeatures + 1, 0, NumericTable::doNotAllocate));
	// Get beta from trained model 
	modelSource.loadDataBlock(beta.get());

	// Retrive pointer to the begining of beta 
	BlockDescriptor<> blockResult;
	beta->getBlockOfRows(0, beta->getNumberOfRows(), readOnly, blockResult);
	// Define the size of beta 
	size_t numberOfBetas = (beta->getNumberOfRows()) * (beta->getNumberOfColumns());

	// Initialize iterators for beta array with itrecepts 
	float* first = blockResult.getBlockPtr();
	float* last = first + numberOfBetas;

	// Create model builder with true intercept flag 
	ModelBuilder<> modelBuilder(nFeatures, nDependentVariables);

	// Set beta 
	modelBuilder.setBeta(first, last);
	beta->releaseBlockOfRows(blockResult);

	printNumericTable(modelBuilder.getModel()->getBeta(), "Linear Regression coefficients of built model:");

	return modelBuilder.getModel();
}

void testModel(ModelPtr& inputModel)
{
	// Initialize FileDataSource<CSVFeatureManager> to retrieve the test data from a .csv file 
	FileDataSource<CSVFeatureManager> testDataSource(testDatasetFileName, DataSource::doAllocateNumericTable, DataSource::doDictionaryFromContext);

	// Create Numeric Tables for testing data and ground truth values 
	NumericTablePtr testData(new HomogenNumericTable<>(nFeatures, 0, NumericTable::doNotAllocate));
	NumericTablePtr testGroundTruth(new HomogenNumericTable<>(nDependentVariables, 0, NumericTable::doNotAllocate));
	NumericTablePtr mergedData(new MergedNumericTable(testData, testGroundTruth));

	// Load the data from the data file 
	testDataSource.loadDataBlock(mergedData.get());

	// Create an algorithm object to predict values of multiple linear regression 
	prediction::Batch<> algorithm;

	// Pass a testing data set and the trained model to the algorithm 
	algorithm.input.set(prediction::data, testData);
	algorithm.input.set(prediction::model, inputModel);

	// Predict values of multiple linear regression 
	algorithm.compute();

	// Retrieve the algorithm results 
	NumericTablePtr prediction = algorithm.getResult()->get(prediction::prediction);
	printNumericTable(prediction, "Linear Regression prediction results:");
	printNumericTable(testGroundTruth, "Ground truth:");
}
