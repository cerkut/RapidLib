#include <vector>
#include <iostream>
#include <cassert>
#include <random>
#include <algorithm>
#include "regression.h"
#include "classification.h"
#include "seriesClassification.h"
#include "rapidStream.h"

int main(int argc, const char * argv[]) {

    rapidStream<double> rapidProcess;
    double bayes = 0.;
    for (int i = 0; i < 100; ++i) {
        bayes = rapidProcess.bayesFilter(i/100.);
    }
    assert(bayes > 0.99);

    //vanAllenTesting
    seriesClassification testDTW;
    std::vector<trainingSeries> testVector;
    trainingSeries tempSeriesTest;
    
    for (int i = 0; i < 5; ++i) {
        tempSeriesTest.input.push_back({ 0.1, 0.1, 0.1 });
    }
    tempSeriesTest.label = "zzz";
    testVector.push_back(tempSeriesTest);
    tempSeriesTest.label = "yyy";
    testVector.push_back(tempSeriesTest);
    
    testDTW.train(testVector);
    std::cout << testDTW.run(tempSeriesTest.input) << std::endl;
    
    //////////////////////////////////////////////////////////////////////////////simple multilayer test
    
    //This takes forever, I don't always run it
    //#define MULTILAYER 1
#ifdef MULTILAYER
    regression myNN2;
    
    std::vector<trainingExample> trainingSet1;
    trainingExample  tempExample1;
    tempExample1.input = { 1.0, 1.0, 1.0 };
    tempExample1.output = { 10.0 };
    trainingSet1.push_back(tempExample1);
    tempExample1.input = { 2.0, 2.0, 2.0 };
    tempExample1.output = { 1.3 };
    trainingSet1.push_back(tempExample1);
    myNN2.setNumHiddenLayers(2);
    assert(myNN2.getNumHiddenLayers()[0] == 2);
    
    myNN2.setNumEpochs(50000);
    myNN2.train(trainingSet1);
    
    std::vector<double> inputVec1 = { 2.0, 2.0, 2.0 };
    std::cout << myNN2.run(inputVec1)[0] <<std::endl;
    
    myNN2.reset();
    trainingSet1.clear();
    tempExample1.input = {0., 0. };
    tempExample1.output = { 0.0 };
    trainingSet1.push_back(tempExample1);
    tempExample1.input = {0., 1. };
    tempExample1.output = { 1.0 };
    trainingSet1.push_back(tempExample1);
    tempExample1.input = {1., 0. };
    tempExample1.output = { 1.0 };
    trainingSet1.push_back(tempExample1);
    tempExample1.input = {1., 1. };
    tempExample1.output = { 2.0 };
    trainingSet1.push_back(tempExample1);
    myNN2.setNumHiddenLayers(2);
    assert(myNN2.getNumHiddenLayers()[0] == 2);
    
    myNN2.setNumEpochs(500000);
    myNN2.train(trainingSet1);
    
    inputVec1 = { 0.9, 0.7 };
    std::cout << myNN2.run(inputVec1)[0] <<std::endl;
#endif
    
    ////////////////////////////////////////////////////////////////////////////////
    
    regression myNN;
    classification myKnn;
    //classification mySVM(classification::svm);
    
    std::vector<trainingExample> trainingSet;
    trainingExample  tempExample;
    tempExample.input = { 0.2, 0.7 };
    tempExample.output = { 3.0 };
    trainingSet.push_back(tempExample);
    
    tempExample.input = { 2.0, 44.2 };
    tempExample.output = { 20.14 };
    trainingSet.push_back(tempExample);
    
    myNN.train(trainingSet);
    //    std::cout << myNN.getJSON() << std::endl;
    std::string filepath = "/var/tmp/modelSetDescription.json";
    myNN.writeJSON(filepath);
    
    
    regression myNNfromString;
    myNNfromString.putJSON(myNN.getJSON());
    
    regression myNNfromFile;
    myNNfromFile.readJSON(filepath);
    std::vector<double> inputVec = { 2.0, 44.2 };
    
    
    std::cout << "before: " << myNN.run(inputVec)[0] << std::endl;
    //std::cout << "from string: " << myNNfromString.run(inputVec)[0] << std::endl;
    //std::cout << "from file: " << myNNfromFile.run(inputVec)[0] << std::endl;
    
    assert(myNN.run(inputVec)[0] == 20.14);
    //assert(myNN.run(inputVec)[0] == myNNfromString.run(inputVec)[0]);
    //assert(myNN.run(inputVec)[0] == myNNfromFile.run(inputVec)[0]);
    
    //Testing exceptions for regression
    std::vector<double> emptyVec = {};
    try {
        myNN.run(emptyVec);
    }
    catch (const std::length_error &e){
        assert(e.what() == std::string("bad input size: 0"));
    }
    
    std::vector<double> wrongSizeVector = {1, 1, 1, 1, 1, 1};
    try {
        myNN.run(wrongSizeVector);
    }
    catch (const std::length_error &e){
        assert(e.what() == std::string("bad input size: 6"));
    }
    
    regression badNN;
    std::vector<trainingExample> badSet;
    trainingExample  badExample;
    badExample.input = { 0.1, 0.2 };
    badExample.output = { 3.0 };
    badSet.push_back(badExample);
    
    badExample.input = { 1.0, 2.0, 3.0 };
    badExample.output = { 4.0 };
    badSet.push_back(badExample);
    
    try {
        badNN.train(badSet);
    }
    catch (const std::length_error &e) {
        assert(e.what() == std::string("unequal feature vectors in input."));
    }
    
    badSet.clear();
    badExample.input = { 0.1, 0.2 };
    badExample.output = { 3.0 };
    badSet.push_back(badExample);
    
    badExample.input = { 1.0, 2.0 };
    badExample.output = { 4.0, 5.0 };
    badSet.push_back(badExample);
    
    try {
        badNN.train(badSet);
    }
    catch(const std::length_error &e) {
        assert(e.what() == std::string("unequal output vectors."));
    }
    
    ///////////////////////////
    
    myKnn.train(trainingSet);
    //mySVM.train(trainingSet);
    
    //   std::cout << myKnn.getJSON() << std::endl;
    std::string filepath2 = "/var/tmp/modelSetDescription_knn.json";
    myKnn.writeJSON(filepath2);
    
    classification myKnnFromString(classification::knn);
    myKnnFromString.putJSON(myKnn.getJSON());
    
    classification myKnnFromFile;
    myKnnFromFile.readJSON(filepath2);
    
    std::cout << "knn before: " << myKnn.run(inputVec)[0] << std::endl;
    //std::cout << "svm: " << mySVM.run(inputVec)[0] << std::endl;
    //std::cout << "knn from string: " << myKnnFromString.run(inputVec)[0] << std::endl;
    //std::cout << "knn from file: " << myKnnFromFile.run(inputVec)[0] << std::endl;
    
    assert(myKnn.run(inputVec)[0] == 20);
    //assert(myKnn.run(inputVec)[0] == myKnnFromString.run(inputVec)[0]);
    //assert(myKnn.run(inputVec)[0] == myKnnFromFile.run(inputVec)[0]);
    
    try {
        myKnn.run(emptyVec);
    }
    catch (const std::length_error &e){
        std::cout << "error: " << e.what() << std::endl;
        assert(e.what() == std::string("bad input size: 0"));
    }
    try {
        myKnn.run(wrongSizeVector);
    }
    catch (const std::length_error &e){
        std::cout << "error: " << e.what() << std::endl;
        assert(e.what() == std::string("bad input size: 6"));
    }
    
    assert(myKnn.getK()[0] == 1);
    myKnn.setK(0, 2);
    assert(myKnn.getK()[0] == 2);
    
    //    regression<float> bigVector;
    std::vector<trainingExampleTemplate<float> > trainingSet2;
    trainingExampleTemplate<float> tempExample2;
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(-0.5,0.5);
    int vecLength = 64;
    for (int j = 0; j < vecLength; ++j) {
        tempExample2.input.clear();
        tempExample2.output.clear();
        for (int i = 0; i < vecLength; ++i) {
            tempExample2.input.push_back(distribution(generator));
        }
        tempExample2.output = { distribution(generator) };
        trainingSet2.push_back(tempExample2);
    }
    //    bigVector.train(trainingSet2);
    std::vector<float> inputVec2;
    for (int i=0; i < vecLength; ++i) {
        inputVec2.push_back(distribution(generator));
    }
    //    assert (isfinite(bigVector.run(inputVec2)[0]));
    
    
    /////////
    
    /*
     classification mySVM2(classification::svm);
     
     std::vector<trainingExample> trainingSet3;
     trainingExample tempExample3;
     
     tempExample3.input = { 0., 0. };
     tempExample3.output = { 0. };
     trainingSet3.push_back(tempExample3);
     
     tempExample3.input = { 1., 0. };
     tempExample3.output = { 1. };
     trainingSet3.push_back(tempExample3);
     
     tempExample3.input = { 1., 8. };
     tempExample3.output = { 5. };
     trainingSet3.push_back(tempExample3);
     */
    
    
    /*
     tempExample3.input = { 1., 1. };
     tempExample3.output = { 2. };
     trainingSet3.push_back(tempExample3);
     */
    
    //mySVM2.train(trainingSet3);
    
    std::vector<double>inputVec4 = { 1., 0. };
    //std::cout << "svm: " << mySVM2.run(inputVec4)[0] << std::endl;
    
    std::vector<double> inputVec3 = { 0., 0. };
    //std::cout << "svm2: " << mySVM2.run(inputVec3)[0] << std::endl;
    
    
    
    //////////////////////////////////////////////////////////////////////// DTW
    
    //Test series
    std::vector<std::vector<double>> seriesOne;
    seriesOne.push_back( { 1., 5.} );
    seriesOne.push_back( { 2., 4.} );
    seriesOne.push_back( { 3., 3.} );
    seriesOne.push_back( { 4., 2.} );
    seriesOne.push_back( { 5., 1.} );
    
    std::vector<std::vector<double>> seriesTwo;
    seriesTwo.push_back( { 1., 4. } );
    seriesTwo.push_back( { 2., -3. } );
    seriesTwo.push_back( { 1., 5. } );
    seriesTwo.push_back( { -2., 1. } );
    
    
    //Testing with labels
    seriesClassification myDTW;
    std::vector<trainingSeries> seriesVector;
    trainingSeries tempSeries;
    
    tempSeries.input.push_back( { 1., 5.} );
    tempSeries.input.push_back( { 2., 4.} );
    tempSeries.input.push_back( { 3., 3.} );
    tempSeries.input.push_back( { 4., 2.} );
    tempSeries.input.push_back( { 5., 1.} );
    tempSeries.label = "first series";
    seriesVector.push_back(tempSeries);
    
    tempSeries = {};
    tempSeries.input.push_back( { 1., 4.} );
    tempSeries.input.push_back( { 2., -3.} );
    tempSeries.input.push_back( { 1., 5.} );
    tempSeries.input.push_back( { -2., 1.} );
    tempSeries.label = "second series";
    seriesVector.push_back(tempSeries);
    
    myDTW.train(seriesVector);
    assert(myDTW.run(seriesOne) == "first series");
    assert(myDTW.run(seriesTwo) == "second series");
    //std::cout << myDTW.getCosts()[0] << std::endl;
    //std::cout << myDTW.getCosts()[1] << std::endl;
    
    //testing match against single label
    assert(myDTW.run(seriesOne, "second series") == 19.325403217417502);
    
    //Training set stats
    assert(myDTW.getMaxLength() == 5);
    assert(myDTW.getMinLength() == 4);
    assert(myDTW.getMaxLength("first series") == 5);
    assert(myDTW.getMinLength("first series") == 5);
    assert(myDTW.getMaxLength("second series") == 4);
    assert(myDTW.getMinLength("second series") == 4);
    
    //costs inside of a series
    tempSeries = {};
    tempSeries.input.push_back( { 1., 5.1} );
    tempSeries.input.push_back( { 2., 4.1} );
    tempSeries.input.push_back( { 3., 3.1} );
    tempSeries.input.push_back( { 4., 2.1} );
    tempSeries.input.push_back( { 5., 1.1} );
    tempSeries.label = "first series";
    seriesVector.push_back(tempSeries);
    
    tempSeries = {};
    tempSeries.input.push_back( { 1.3, 5.1} );
    tempSeries.input.push_back( { 2.3, 4.1} );
    tempSeries.input.push_back( { 3.3, 3.1} );
    tempSeries.input.push_back( { 4.3, 2.1} );
    tempSeries.input.push_back( { 5.3, 1.1} );
    tempSeries.label = "first series";
    seriesVector.push_back(tempSeries);
    
    myDTW.train(seriesVector);
    std::cout << "mincost " << myDTW.calculateCosts("first series").min << std::endl;
    std::cout << "maxcost " << myDTW.calculateCosts("first series").max << std::endl;
    
    /*
     fastDTW fastDtw;
     std::cout << "fast one-two cost " << fastDtw.getCost(seriesOne, seriesTwo, 1) << std::endl;
     std::cout << "fast two-one cost " << fastDtw.getCost(seriesTwo, seriesOne, 1) << std::endl;
     
     dtw slowDTW;
     std::cout << "slow one-two cost " << slowDTW.getCost(seriesOne, seriesTwo) << std::endl;
     */
    
    //Long dtw test
    int testSize = 800;
    seriesVector.clear();
    std::vector<std::vector<double> > inputSeries;
    tempSeries.input.clear();
    for (int i = 0; i < testSize; ++i) {
        tempSeries.input.push_back( { (double)i, (double)i } );
    }
    tempSeries.label ="long up";
    seriesVector.push_back(tempSeries);
    
    tempSeries.input.clear();
    for (int i = 0; i < testSize; ++i) {
        tempSeries.input.push_back( { (double)i, double(testSize - i) } );
    }
    tempSeries.label ="long down";
    seriesVector.push_back(tempSeries);
    
    myDTW.train(seriesVector);
    inputSeries = tempSeries.input;
    assert(myDTW.run(inputSeries) == "long down");
    //std::cout << myDTW.getCosts()[0] << std::endl;
    //std::cout << myDTW.getCosts()[1] << std::endl;
    
    
    ////////////////////////////////////////////////////////////////////////
    
    
    return 0;
}
