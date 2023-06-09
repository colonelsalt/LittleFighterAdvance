#pragma once

typedef unsigned int b32;

// Fixed-point type
union fixed
{
	struct
	{
		u8 FractionalPart;
		int WholePart : 24;
	};
	s32 RawValue;

	fixed() : RawValue(0) {}

	fixed(int Int) : FractionalPart(0), WholePart(Int) {}

	fixed(int Whole, int Fraction) : FractionalPart(Fraction), WholePart(Whole) {}

	inline void operator+=(fixed Other)
	{
		RawValue += Other.RawValue;
	}

	inline void operator-=(fixed Other)
	{
		RawValue -= Other.RawValue;
	}

	inline void operator+=(int Other)
	{
		WholePart += Other;
	}

	inline void operator=(int Other)
	{
		WholePart = Other;
		FractionalPart = 0;
	}
};

static const fixed F_HALF = fixed(0, 1 << 7);

inline fixed operator+(fixed A, fixed B)
{
	fixed Result;
	Result.RawValue = A.RawValue + B.RawValue;
	return Result;
}

inline fixed operator-(fixed A, fixed B)
{
	fixed Result;
	Result.RawValue = A.RawValue - B.RawValue;
	return Result;
}

inline fixed operator*(fixed A, fixed B)
{
	fixed Result;
	Result.RawValue = (A.RawValue * B.RawValue) >> 8;
	return Result;
}

inline fixed operator+(fixed A, int Int)
{
	fixed Result = A;
	Result.WholePart += Int;
	return Result;
}

inline fixed operator-(fixed A, int Int)
{
	fixed Result = A;
	Result.WholePart -= Int;
	return Result;
}

inline fixed operator*(fixed A, int Int)
{
	fixed Result = A;
	Result.WholePart *= Int;
	return Result;
}

inline fixed operator*(int Int, const fixed B)
{
	fixed Result = B;
	Result.WholePart *= Int;
	return Result;
}

inline bool operator>(fixed A, fixed B)
{
	return A.RawValue > B.RawValue;
}

inline bool operator>=(fixed A, fixed B)
{
	return A.RawValue >= B.RawValue;
}

inline bool operator<=(fixed A, fixed B)
{
	return A.RawValue <= B.RawValue;
}

inline bool operator>(fixed A, int Int)
{
	if (A.WholePart > Int)
	{
		return true;
	}
	else if (A.WholePart == Int)
	{
		return A.FractionalPart > 0;
	}
	return false;
}

inline fixed operator-(fixed A)
{
	fixed Result = A;
	Result.WholePart *= -1;
	return Result;
}

inline bool operator<(fixed A, fixed B)
{
	return A.RawValue < B.RawValue;
}

inline bool operator<(fixed A, int Int)
{
	return A.WholePart < Int;
}

inline fixed FMin(fixed A, fixed B)
{
	return A < B ? A : B;
}

inline fixed FMax(fixed A, fixed B)
{
	return A > B ? A : B;
}

inline fixed FClamp(fixed Value, fixed Min, fixed Max)
{
	return FMin(FMax(Value, Min), Max);
}

inline fixed FAbs(fixed Value)
{
	fixed Result = Value;
	if (Result.WholePart < 0)
	{
		Result.WholePart *= -1;
	}
	return Result;
}


struct v2
{
	fixed X;
	fixed Y;

	inline void operator+=(v2 Other)
	{
		X += Other.X;
		Y += Other.Y;
	}
};

inline v2 operator+(v2 A, v2 B)
{
	v2 Result;
	Result.X = A.X + B.X;
	Result.Y = A.Y + B.Y;
	return Result;
}

inline v2 operator-(v2 A, v2 B)
{
	v2 Result;
	Result.X = A.X - B.X;
	Result.Y = A.Y - B.Y;
	return Result;
}

inline fixed SqMagnitude(v2 V)
{
	return V.X * V.X + V.Y * V.Y;
}