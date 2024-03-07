/*
 * RandomNumberGeneration_helper.h
 *
 *  Created on: 16 Feb 2024
 *      Author: Charlie
 *
 *      Referencing:
 *      https://en.cppreference.com/w/cpp/numeric/random
 *      https://codehs.com/documentation/new/java#docs-random
 *      https://codehs.github.io/codehs-java-library/src/Randomizer.java - converted this to c++
 */
#ifndef RANDOMNUMBERGENERATION_HELPER_H
#define RANDOMNUMBERGENERATION_HELPER_H

#include <iostream>
#include <random>

using namespace std;

class RandomNumberGeneration_helper {
private:
	static random_device _randomDevice;
	static mt19937_64 _psuedoRandom_Gen;  // https://en.cppreference.com/w/cpp/numeric/random
	static mt19937_64* _theInstance; // static pointer to hold the instance
public:
	static mt19937_64& getInstance();
	static bool nextBoolean();
	static bool nextBoolean(double probability);
	static int nextInt();
	static int nextInt(int integer_val);
	static int nextInt(int minVal, int maxVal);
	static double nextDouble();
	static double nextDouble(double minVal, double maxVal);


};

#endif /* RANDOMNUMBERGENERATION_HELPER_H */
