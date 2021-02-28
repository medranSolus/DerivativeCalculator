/*
Author: Marek Machliñski
Date: 6.12.2017
*/
#pragma once
#include <utility>
#include "Point.h"
#ifdef DERIVATIVE_LIBRARY
#define DERIVATIVE_API __declspec(dllexport)
#else
#define DERIVATIVE_API __declspec(dllimport)
#endif
namespace DerivativeLibrary
{
	using std::move;
	using std::ostream;

	///<summary>
	///Returns a derivative of function given by param
	///</summary>
    DERIVATIVE_API string getDerivative(const string & txt);
	///<summary>
	///Returns value of derivative in point
	///</summary>
	///<param name="txt">Mathematical function</param>
	///<param name="x">Point on graph</param>
    DERIVATIVE_API long double getDerivativeValue(const string & txt, const long double & x);
	///<summary>
	///Returns value of function in point
	///</summary>
	///<param name="txt">Mathematical function</param>
	///<param name="x">Point on graph</param>
	DERIVATIVE_API long double getFunctionValue(const string & txt, const long double & x);

	class Derivative
	{
		bool isUnassigned = true;
		Point * mainNodeRoot = nullptr;

		void InsertPoint(Point * & node, const Pair & pair);
		bool Contains(Point * node, const Pair & pair);
		void CopyNode(Point * & node, Point * & newNode);
		void CreateDerivative(Point * & node, Point * & newNode);
		void Simplify(Point * & node);
		short OperationPriority(const Expression & expression) const;
		void ToInfix(Point * point, string & txt) const;
		void HowManyOperations(Point * node, int & count);
	public:
		Derivative(const string & txt, bool isDerivative);
		~Derivative() { delete mainNodeRoot; }

		long double GetValue(long double x);
		string ToString();
	};
}
