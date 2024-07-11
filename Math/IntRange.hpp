#pragma once

class IntRange {
public:
	// Member Variables
	int m_min;
	int m_max;

	// Constructors
	IntRange();  // Default Constructor
	explicit IntRange(int min, int max);  // Explicit Constructor

										  // Basic Methods
	bool operator==(const IntRange& other) const;
	bool operator!=(const IntRange& other) const;
	bool IsOnRange(int value) const;
	bool IsOverlappingWith(const IntRange& other) const;

	// Named Static Const Objects
	static const IntRange ZERO;
	static const IntRange ONE;
	static const IntRange ZERO_TO_ONE;
};
