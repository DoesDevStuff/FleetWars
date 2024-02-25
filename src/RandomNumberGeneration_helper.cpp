/*
 * RandomNumberGeneration_helper.cpp
 *
 *  Created on: 16 Feb 2024
 *      Author: Charlie
 */

#include "headers/RandomNumberGeneration_helper.h"
using namespace std;
random_device RandomNumberGeneration_helper::_randomDevice;
mt19937_64 RandomNumberGeneration_helper::_psuedoRandom_Gen(_randomDevice());
mt19937_64* RandomNumberGeneration_helper::_theInstance = nullptr; // initialise the static pointer to null

mt19937_64& RandomNumberGeneration_helper::getInstance()
{
	if(!_theInstance){
		_theInstance = &_psuedoRandom_Gen;
	}
	return *_theInstance;
}

/*
 * Here we will generate a random integer and then check if it even or odd
 * and return the bool associated to it even = true, odd = false
 *
 * call getInstance() and since it returns a reference to our psuedoRandom_Gen
 * we will also call mt19937_64 object and thus invoke random generation
*/
bool RandomNumberGeneration_helper::nextBoolean()
{
	return getInstance()() % 2 == 0; // if the remainder is 0, then the number is even else it is odd
}

bool RandomNumberGeneration_helper::nextBoolean(double probability)
{
	uniform_real_distribution<double> distribution(0.0, 1.0);
	return ( distribution( getInstance() ) < probability);
}

int RandomNumberGeneration_helper::nextInt(){
	return getInstance()();
}

int RandomNumberGeneration_helper::nextInt(int integer_val)
{
	uniform_int_distribution<int> distribution(0, integer_val - 1);
	return distribution( getInstance() );
}

int RandomNumberGeneration_helper::nextInt(int minVal, int maxVal)
{
	uniform_int_distribution<int> distribution(minVal, maxVal);
	return distribution( getInstance() );
}

double RandomNumberGeneration_helper::nextDouble(){
	uniform_real_distribution<double> distribution(0.0, 1.0);
	return distribution( getInstance() );
}

double RandomNumberGeneration_helper::nextDouble(double minVal, double maxVal){
	uniform_real_distribution<double> distribution(minVal, maxVal);
	return ( minVal + ( (maxVal - minVal) * nextDouble() ) );
}
