/*
Author: Marek Machliñski
Date: 6.12.2017
*/
#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>
#include <cctype>
#include <limits>
namespace DerivativeLibrary
{
	using std::string;
	using std::to_string;
	using std::for_each;

	enum Expression { Plus = '+', Minus = '-', Multiply = '*', Divide = '/', Power = '^', RootSquare = 'v', Sin = 's', Cos = 'c', Tg = 't', Ctg = 'C', LeftBracket = '(', RightBracet = ')', Log = 'l', Trig = 'T' };
	enum Type { Operation, Number, X, E, Pi, Trigonometric };

    const long double M_E = exp(1.0L);
    const long double M_PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286L;
	const long double OUT_OF_RANGE = -1 * std::numeric_limits<long double>::max() + 1;

	union Content
	{
		long double Number;
		Expression Operation;
		char X = 'x';
	};

	struct Pair
	{
		Content Value;
		short Priority;
		Type ValueType;

		Pair & operator=(const Pair & pair)
		{
			Value = pair.Value;
			Priority = pair.Priority;
			ValueType = pair.ValueType;
			return *this;
		}
	};

	typedef std::vector<Pair> VecPair;

    class ONP
	{
		static void PutFunction(VecPair & out, VecPair & fc, char sign, short number);
		static long double ONPValue(VecPair & ONP);
	public:
		///<summary>
		///Conversion of text with mathematical expression into ONP
		///</summary>
		///<param name="txt">Text with mathematical expression</param>
		///<returns>Container with next values of ONP expression</returns>
		static VecPair ToONP(string txt);

		///<summary>
		///Gets value from ONP expression
		///</summary>
		///<param name="txt">Text with ONP expression</param>
		static long double ValueFromONP(string txt);

		///<summary>
		///Conversion of number into text
		///</summary>
		static string NumberToString(long double x);
	};
}
