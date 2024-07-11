#include "RandomNumberGenerator.hpp"
#include <cstdlib> // For rand() and RAND_MAX

RandomNumberGenerator* g_rng;

int RandomNumberGenerator::RollRandomIntLessThan(int maxNotInclusive)
{
	return rand() % maxNotInclusive; // Returns a random number between 0 (inclusive) and maxNotInclusive (exclusive).
}

int RandomNumberGenerator::RollRandomIntInRange(int minInclusive, int maxInclusive)
{
	return minInclusive + rand() % (maxInclusive - minInclusive + 1); // Returns a random number between minInclusive and maxInclusive.
}

float RandomNumberGenerator::RollRandomFloatZeroToOne()
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX); // Returns a float value between 0.0 and 1.0
}

float RandomNumberGenerator::RollRandomFloatInRange(float minInclusive, float maxInclusive)
{
	float zeroToOne = RollRandomFloatZeroToOne();
	return minInclusive + (zeroToOne * (maxInclusive - minInclusive)); // Scale and shift to desired range
}



