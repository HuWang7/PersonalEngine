#pragma once
//----------------------------------------------------------------------------------------
struct IntVec2
{
public: // Note: this is one of the few cases where we break both the "m_" naming rule and the avoid-public-members rule
	int x = 0;
	int y = 0;

public:
	// Construction/Destruction

	static const IntVec2 ZERO;

	~IntVec2() {}                                                // destructor (do nothing)
	IntVec2() {}                                                 // default constructor (do nothing)
	IntVec2( const IntVec2& copyFrom );                          // copy constructor (from another IntVec2)
	explicit IntVec2( int initialX, int inttialY );              // explicit constructor (from x, y)

	// Accessors (const methods)
	float				GetLength() const;
	int					GetTaxicabLength() const;
	int					GetLengthSquared() const;
	float				GetOrientationRadians() const;
	float				GetOrientationDegrees() const;
	IntVec2 const		GetRotated90Degrees() const;
	IntVec2 const		GetRotatedMinus90Degrees() const;

	// Mutators (non-const methods)
	void Rotate90Degrees();
	void RotateMinus90Degrees();

	//Operators (self-mutating / non-const)

	bool			operator==( const IntVec2& compare ) const;
	void			operator=( const IntVec2& copyFrom );
	bool			operator<(const IntVec2& comparewith) const;
	bool			operator!=(const IntVec2& compare) const;		
	const IntVec2	operator+(const IntVec2& vecToAdd) const;		
	const IntVec2	operator-(const IntVec2& vecToSubtract) const;	
	const IntVec2	operator-() const;								
	const IntVec2	operator*(int uniformScale) const;			
	const IntVec2	operator*(const IntVec2& vecToMultiply) const;	
	const IntVec2	operator/(int inverseScale) const;			

	// Operators (self-mutating / non-const)
	void			operator+=(const IntVec2& vecToAdd);				
	void			operator-=(const IntVec2& vecToSubtract);		
	void			operator*=(const int uniformScale);			
	void			operator/=(const int uniformDivisor);		
	void SetFromText(const char* text);
};

