/*
Author: Marek Machliñski
Date: 6.12.2017
*/
#pragma once
#include "ONP.h"
namespace DerivativeLibrary
{
	///<summary>
	///Point in arithmetic tree representing number or other expression
	///</summary>
	class Point
	{
		Type contentType;
		Content value;
		Point * right = nullptr;
		Point * left = nullptr;
	public:
		Point();
		Point(const Point & point);
		Point(const Pair & pair);
		Point(Type type, Content content) : contentType(type), value(content) { }
		~Point();

		inline Point * & RightChild() { return right; }
		inline Point * & LeftChild() { return left; }
		inline Type ContentType() const { return contentType; }
		inline Content ContentValue() const { return value; }
		inline double ValueNumber() const { return value.Number; }
		inline Expression ValueExpression() const { return value.Operation; }

		void ChangeContent(const Type & type, const Content & content);
		///<summary>
		///Insert text value into param of content inside
		///</summary>
		void TextValue(string & function);

		Point & operator=(const Point & point);
		Point & operator=(Point && point);
	};
}
