#include <iostream>
#include <chrono>
#include "daal.h"
#include "service.h"


using namespace std;
using namespace std::chrono;
using namespace daal;
using namespace daal::algorithms;
using namespace daal::services;

// Input data set parameters 
const string datasetFileName = "./Data/GPct.txt" ;

void printResults(const low_order_moments::ResultPtr& res);
void reportTime(steady_clock::duration span);


int main(int argc, char* argv[])
{
	checkArguments(argc, argv, 1, &datasetFileName);
	steady_clock::time_point ts, te;
	ts = steady_clock::now();
	/* Initialize FileDataSource<CSVFeatureManager> to retrieve the input data from a .csv file */
	FileDataSource<CSVFeatureManager> dataSource(datasetFileName, DataSource::doAllocateNumericTable, DataSource::doDictionaryFromContext);

	/* Retrieve the data from the input file */
	dataSource.loadDataBlock();

	/* Create an algorithm to compute low order moments using the default method */
	low_order_moments::Batch<> algorithm;

	/* Set input objects for the algorithm */
	algorithm.input.set(low_order_moments::data, dataSource.getNumericTable());

	/* Compute low order moments */
	algorithm.compute();

	/* Get the computed low order moments */
	low_order_moments::ResultPtr res = algorithm.getResult();

	printResults(res);
	te = steady_clock::now();
	reportTime(te - ts);
	return 0;
}

void printResults(const low_order_moments::ResultPtr& res)
{
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
void reportTime(steady_clock::duration span) {
	auto ms = duration_cast<milliseconds>(span);
	std::cout << " Batch - took - " <<
		ms.count() << " milliseconds" << std::endl;
}
