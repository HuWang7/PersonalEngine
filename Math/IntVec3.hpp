#pragma once
struct IntVec3
{
public:
	int x = 0;
	int y = 0;
	int z = 0;

public:
	//construction /destruction
	~IntVec3() {};
	IntVec3() {};
	IntVec3(const IntVec3& copyFrom);
	explicit IntVec3(int initialX, int initialY, int initialZ);

	//Accessors (const)
	float			GetLength() const;
	int				GetTaxicabLength() const;
	int				GetLengthSquared() const;


	//Mutators (non- const)

	void SetFromText(const char* text);
	// Operators (const)
	bool		operator==(const IntVec3& compare) const;		// vec2 == vec2
	bool		operator!=(const IntVec3& compare) const;		// vec2 != vec2
	bool		operator<(const IntVec3& comparewith) const;
	const IntVec3	operator+(const IntVec3& vecToAdd) const;		// vec2 + vec2
	const IntVec3	operator-(const IntVec3& vecToSubtract) const;	// vec2 - vec2
	const IntVec3	operator-() const;								// -vec2, i.e. "unary negation"
	const IntVec3	operator*(int uniformScale) const;			// vec2 * float
	const IntVec3	operator*(const IntVec3& vecToMultiply) const;	// vec2 * vec2

	// Operators (self-mutating / non-const)
	void		operator+=(const IntVec3& vecToAdd);				// vec2 += vec2
	void		operator-=(const IntVec3& vecToSubtract);		// vec2 -= vec2
	void		operator*=(const int uniformScale);			// vec2 *= float
	void		operator=(const IntVec3& copyFrom);				// vec2 = vec2


};