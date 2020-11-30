// LowOrder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "daal.h"
#include "service.h"


using namespace std;
using namespace daal;
using namespace daal::algorithms;
using namespace daal::services;

/* Input data set parameters */
const size_t nBlocks = 1;

const string datasetFileNames[] = {  "./Data/GPct.txt" };

low_order_moments::PartialResultPtr partialResult[nBlocks];
low_order_moments::ResultPtr result;

void computestep1Local(size_t i);
void computeOnMasterNode();
void printResults(const low_order_moments::ResultPtr& res);

int main(int argc, char* argv[])
{
	checkArguments(argc, argv, 1, &datasetFileNames[0]);

	for (size_t i = 0; i < nBlocks; i++)
	{
		computestep1Local(i);
	}

	computeOnMasterNode();

	printResults(result);

	return 0;
}

void computestep1Local(size_t block)
{
	// Services Class - Helps extract data from a csv file
	FileDataSource<CSVFeatureManager> dataSource(datasetFileNames[block], DataSource::doAllocateNumericTable, DataSource::doDictionaryFromContext);

	// Retrieve the data from the input file 
	dataSource.loadDataBlock();

	// Create algorithm for distributred data
	low_order_moments::Distributed<step1Local> algorithm;

	// Set input objects for the algorithm 
	algorithm.input.set(low_order_moments::data, dataSource.getNumericTable());

	// Compute partial low order moments estimates on nodes 
	algorithm.compute();

	// Get the computed partial estimates 
	partialResult[block] = algorithm.getPartialResult();
}

void computeOnMasterNode()
{
	// Create an algorithm for the master node using the default method 
	low_order_moments::Distributed<step2Master> algorithm;

	// Set input objects based on the number of partial blocks
	for (size_t i = 0; i < nBlocks; i++)
	{
		algorithm.input.add(low_order_moments::partialResults, partialResult[i]);
	}

	// Compute on local nodes 
	algorithm.compute();

	// Finalize the result 
	algorithm.finalizeCompute();

	// Results stored
	result = algorithm.getResult();
}

void printResults(const low_order_moments::ResultPtr& res)
{
	//Prints the contents of the Numeric Table
	printNumericTable(res->get(low_order_moments::minimum), "Minimum:");
	printNumericTable(res->get(low_order_moments::maximum), "Maximum:");
	printNumericTable(res->get(low_order_moments::sum), "Sum:");
	printNumericTable(res->get(low_order_moments::sumSquares), "Sum of squares:");
	printNumericTable(res->get(low_order_moments::sumSquaresCentered), "Sum of squared difference from the means:");
	printNumericTable(res->get(low_order_moments::mean), "Mean:");
	printNumericTable(res->get(low_order_moments::secondOrderRawMoment), "Second order raw moment:");
	printNumericTable(res->get(low_order_moments::variance), "Variance:");
	printNumericTable(res->get(low_order_moments::standardDeviation), "Standard deviation:");
	printNumericTable(res->get(low_order_moments::variation), "Variation:");
}
