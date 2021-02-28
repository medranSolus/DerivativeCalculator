/*
Author: Marek Machliñski
Date: 6.12.2017
*/
#include "stdafx.h"
#include "Point.h"
namespace DerivativeLibrary
{
	Point::Point()
	{
		value.Number = 0;
		contentType = Number;
	}

	Point::Point(const Point & point)
	{
		contentType = point.contentType;
		value = point.value;
	}

	Point::Point(const Pair & pair)
	{
		contentType = pair.ValueType;
		value = pair.Value;
	}

	Point::~Point()
	{
		if (right != nullptr)
			right->~Point();
		if (left != nullptr)
			left->~Point();
	}

	void Point::ChangeContent(const Type & type, const Content & content)
	{
		contentType = type;
		value = content;
	}

	void Point::TextValue(string & function)
	{
		if (left != nullptr)
			left->TextValue(function);
		if (right != nullptr)
			right->TextValue(function);
		switch (contentType)
		{
		case Trigonometric:
		case Operation:
		{
			function += ' ';
			function += value.Operation;
			break;
		}
		case Number:
		{
			string txt = ONP::NumberToString(value.Number);
			function += " " + txt;
			break;
		}
		case X:
		{
			function += " x";
			break;
		}
		case E:
		{
			function += " e";
			break;
		}
		case Pi:
		{
			function += " pi";
			break;
		}
		}
	}

	Point & Point::operator=(const Point & point)
	{
		this->operator=(Point(point));
		return *this;
	}

	Point & Point::operator=(Point && point)
	{
		contentType = point.contentType;
		value = point.value;
		left = point.left;
		right = point.right;
		point.left = point.right = nullptr;
		return *this;
	}
}
