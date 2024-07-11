#pragma once
class RandomNumberGenerator
{
public:
	int RollRandomIntLessThan( int maxNotInclusive );
	int RollRandomIntInRange( int minInclusive, int maxInclusive );
	float RollRandomFloatZeroToOne();
	float RollRandomFloatInRange( float minInclusive, float maxInclusive );

	unsigned int  m_seed = 0;     // we will use this later on...

private:
	 int          m_position = 0; //...when we replace rand() with noise
};



