/*
Author: Marek Machliñski
Date: 6.12.2017
*/
#include "stdafx.h"
#include "Derivative.h"
namespace DerivativeLibrary
{
	Derivative::Derivative(const string & txt, bool isDerivative)
	{
		VecPair onpExpression = move(ONP::ToONP(txt));
		Point * node = nullptr;
		Pair x;
		for (size_t I = onpExpression.size(); I > 0;)
		{
			x = onpExpression.at(--I);
			isUnassigned = true;
			InsertPoint(node, x);
		}
		onpExpression.clear();
		if (isDerivative)
		{
			CreateDerivative(node, mainNodeRoot);
			delete node;
		}
		else
		{
			mainNodeRoot = node;
			node = nullptr;
		}
		int I = 0;
		HowManyOperations(mainNodeRoot, I);
		for (I = I * 0.5 + 1; I > 0; --I)
			Simplify(mainNodeRoot);
	}

	void Derivative::InsertPoint(Point * & node, const Pair & pair)
	{
		if (node == nullptr)
		{
			node = new Point(pair);
			isUnassigned = false;
		}
		if (isUnassigned && (node->RightChild() == nullptr || node->RightChild()->ContentType() == Operation))
			InsertPoint(node->RightChild(), pair);
		if (isUnassigned && (node->LeftChild() == nullptr || node->LeftChild()->ContentType() == Operation))
			InsertPoint(node->LeftChild(), pair);
		return;
	}

	bool Derivative::Contains(Point * node, const Pair & pair)
	{
		if (node != nullptr)
		{
			if (pair.ValueType == X && node->ContentType() == X)
				return true;
			if (pair.ValueType == Number && node->ContentType() == Number)
				if (pair.Value.Number == node->ValueNumber())
					return true;
			if (pair.ValueType == Operation && node->ContentType() == Operation)
				if (pair.Value.Operation == node->ValueExpression())
					return true;
			if (node->RightChild() != nullptr)
				if (Contains(node->RightChild(), pair))
					return true;
			if (node->LeftChild() != nullptr)
				if (Contains(node->LeftChild(), pair))
					return true;
		}
		return false;
	}

	void Derivative::CopyNode(Point * & node, Point * & newNode)
	{
		if (node == nullptr)
			return;
		else
		{
			Pair pair;
			pair.ValueType = node->ContentType();
			pair.Value = node->ContentValue();
			isUnassigned = true;
			InsertPoint(newNode, pair);
			CopyNode(node->RightChild(), newNode->RightChild());
			CopyNode(node->LeftChild(), newNode->LeftChild());
		}
	}

	void Derivative::CreateDerivative(Point * & node, Point * & newNode)
	{
		if (node == nullptr)
			return;
		Pair pair;
		switch (node->ContentType())
		{
		case E:
		case Pi:
		case Number:
		{
			pair.ValueType = Number;
			pair.Value.Number = 0;
			isUnassigned = true;
			InsertPoint(newNode, pair);
			break;
		}
		case X:
		{
			pair.ValueType = Number;
			pair.Value.Number = 1;
			isUnassigned = true;
			InsertPoint(newNode, pair);
			break;
		}
		case Operation:
		{
			pair.ValueType = Operation;
			switch (node->ValueExpression())
			{
			case Expression::Plus:
			{
				pair.ValueType = X;
				bool left = Contains(node->LeftChild(), pair);
				bool right = Contains(node->RightChild(), pair);
				if (left && right)
				{
					pair.ValueType = Operation;
					pair.Value.Operation = Expression::Plus;
					isUnassigned = true;
					InsertPoint(newNode, pair);
					CreateDerivative(node->RightChild(), newNode->RightChild());
					CreateDerivative(node->LeftChild(), newNode->LeftChild());
				}
				else if (left)
					CreateDerivative(node->LeftChild(), newNode);
				else if (right)
					CreateDerivative(node->RightChild(), newNode);
				else
				{
					pair.ValueType = Number;
					pair.Value.Number = 0;
					isUnassigned = true;
					InsertPoint(newNode, pair);
				}
				break;
			}
			case Expression::Minus:
			{
				pair.ValueType = X;
				bool left = Contains(node->LeftChild(), pair);
				bool right = Contains(node->RightChild(), pair);
				if (left && right)
				{
					pair.ValueType = Operation;
					pair.Value.Operation = Expression::Minus;
					isUnassigned = true;
					InsertPoint(newNode, pair);
					CreateDerivative(node->RightChild(), newNode->RightChild());
					CreateDerivative(node->LeftChild(), newNode->LeftChild());
				}
				else if (left)
					CreateDerivative(node->LeftChild(), newNode);
				else if (right)
				{
					pair.ValueType = Operation;
					pair.Value.Operation = Expression::Multiply;
					isUnassigned = true;
					InsertPoint(newNode, pair);
					pair.ValueType = Number;
					pair.Value.Number = -1;
					isUnassigned = true;
					InsertPoint(newNode->LeftChild(), pair);
					CreateDerivative(node->RightChild(), newNode->RightChild());
				}
				else
				{
					pair.ValueType = Number;
					pair.Value.Number = 0;
					isUnassigned = true;
					InsertPoint(newNode, pair);
				}
				break;
			}
			case Expression::Multiply:
			{
				pair.ValueType = X;
				bool left = Contains(node->LeftChild(), pair);
				bool right = Contains(node->RightChild(), pair);
				if (left && right)
				{
					pair.ValueType = Operation;
					pair.Value.Operation = Expression::Plus;
					isUnassigned = true;
					InsertPoint(newNode, pair);
					pair.Value.Operation = Expression::Multiply;
					isUnassigned = true;
					InsertPoint(newNode->RightChild(), pair);
					isUnassigned = true;
					InsertPoint(newNode->LeftChild(), pair);
					CopyNode(node->LeftChild(), newNode->RightChild()->LeftChild());
					CopyNode(node->RightChild(), newNode->LeftChild()->RightChild());
					CreateDerivative(node->RightChild(), newNode->RightChild()->RightChild());
					CreateDerivative(node->LeftChild(), newNode->LeftChild()->LeftChild());
				}
				else if (left)
				{
					pair.ValueType = Operation;
					pair.Value.Operation = Expression::Multiply;
					isUnassigned = true;
					InsertPoint(newNode, pair);
					CopyNode(node->RightChild(), newNode->RightChild());
					CreateDerivative(node->LeftChild(), newNode->LeftChild());
				}
				else if (right)
				{
					pair.ValueType = Operation;
					pair.Value.Operation = Expression::Multiply;
					isUnassigned = true;
					InsertPoint(newNode, pair);
					CopyNode(node->LeftChild(), newNode->LeftChild());
					CreateDerivative(node->RightChild(), newNode->RightChild());
				}
				else
				{
					pair.ValueType = Number;
					pair.Value.Number = 0;
					isUnassigned = true;
					InsertPoint(newNode, pair);
				}
				break;
			}
			case Expression::Divide:
			{
				pair.Value.Operation = Expression::Divide;
				isUnassigned = true;
				InsertPoint(newNode, pair);
				pair.Value.Operation = Expression::Power;
				isUnassigned = true;
				InsertPoint(newNode->RightChild(), pair);
				pair.ValueType = Number;
				pair.Value.Number = 2;
				isUnassigned = true;
				InsertPoint(newNode->RightChild()->RightChild(), pair);
				CopyNode(node->RightChild(), newNode->RightChild()->LeftChild());
				pair.ValueType = Operation;
				pair.Value.Operation = Expression::Minus;
				isUnassigned = true;
				InsertPoint(newNode->LeftChild(), pair);
				pair.Value.Operation = Expression::Multiply;
				isUnassigned = true;
				InsertPoint(newNode->LeftChild()->RightChild(), pair);
				isUnassigned = true;
				InsertPoint(newNode->LeftChild()->LeftChild(), pair);
				CopyNode(node->RightChild(), newNode->LeftChild()->LeftChild()->RightChild());
				CreateDerivative(node->RightChild(), newNode->LeftChild()->RightChild()->RightChild());
				CopyNode(node->LeftChild(), newNode->LeftChild()->RightChild()->LeftChild());
				CreateDerivative(node->LeftChild(), newNode->LeftChild()->LeftChild()->LeftChild());
				break;
			}
			case Expression::Power:
			{
				pair.ValueType = X;
				bool left = Contains(node->LeftChild(), pair);
				bool right = Contains(node->RightChild(), pair);
				if (left && right)
				{
					pair.ValueType = Operation;
					pair.Value.Operation = Expression::Multiply;
					isUnassigned = true;
					InsertPoint(newNode, pair);
					pair.Value.Operation = Expression::Power;
					isUnassigned = true;
					InsertPoint(newNode->LeftChild(), pair);
					CopyNode(node->LeftChild(), newNode->LeftChild()->LeftChild());
					CopyNode(node->RightChild(), newNode->LeftChild()->RightChild());
					pair.Value.Operation = Expression::Plus;
					isUnassigned = true;
					InsertPoint(newNode->RightChild(), pair);
					pair.Value.Operation = Expression::Multiply;
					isUnassigned = true;
					InsertPoint(newNode->RightChild()->LeftChild(), pair);
					pair.Value.Operation = Expression::Log;
					isUnassigned = true;
					InsertPoint(newNode->RightChild()->LeftChild()->RightChild(), pair);
					pair.ValueType = Number;
					pair.Value.Number = 10;
					isUnassigned = true;
					InsertPoint(newNode->RightChild()->LeftChild()->RightChild()->LeftChild(), pair);
					CopyNode(node->LeftChild(), newNode->RightChild()->LeftChild()->RightChild()->RightChild());
					pair.ValueType = Operation;
					pair.Value.Operation = Expression::Divide;
					isUnassigned = true;
					InsertPoint(newNode->RightChild()->RightChild(), pair);
					pair.Value.Operation = Expression::Multiply;
					isUnassigned = true;
					InsertPoint(newNode->RightChild()->RightChild()->LeftChild(), pair);
					CopyNode(node->LeftChild(), newNode->RightChild()->RightChild()->RightChild());
					CopyNode(node->RightChild(), newNode->RightChild()->RightChild()->LeftChild()->RightChild());
					CreateDerivative(node->RightChild(), newNode->RightChild()->RightChild()->LeftChild()->LeftChild());
					CreateDerivative(node->LeftChild(), newNode->RightChild()->LeftChild()->LeftChild());
				}
				else if (left)
				{
					pair.ValueType = Operation;
					pair.Value.Operation = Expression::Multiply;
					isUnassigned = true;
					InsertPoint(newNode, pair);
					CopyNode(node->RightChild(), newNode->LeftChild());
					pair.Value.Operation = Expression::Power;
					isUnassigned = true;
					InsertPoint(newNode->RightChild(), pair);
					CopyNode(node->LeftChild(), newNode->RightChild()->LeftChild());
					pair.Value.Operation = Expression::Minus;
					isUnassigned = true;
					InsertPoint(newNode->RightChild()->RightChild(), pair);
					pair.ValueType = Number;
					pair.Value.Number = 1;
					isUnassigned = true;
					InsertPoint(newNode->RightChild()->RightChild()->RightChild(), pair);
					CopyNode(node->RightChild(), newNode->RightChild()->RightChild()->LeftChild());
				}
				else if (right)
				{
					if (node->LeftChild()->ContentType() == E && node->RightChild()->ContentType() == X)
						CopyNode(node, newNode);
					else
					{
						pair.ValueType = Operation;
						pair.Value.Operation = Expression::Multiply;
						isUnassigned = true;
						InsertPoint(newNode, pair);
						pair.Value.Operation = Expression::Power;
						isUnassigned = true;
						InsertPoint(newNode->LeftChild(), pair);
						CopyNode(node->LeftChild(), newNode->LeftChild()->LeftChild());
						CopyNode(node->RightChild(), newNode->LeftChild()->RightChild());
						pair.Value.Operation = Expression::Log;
						isUnassigned = true;
						InsertPoint(newNode->RightChild(), pair);
						pair.ValueType = E;
						pair.Value.Number = M_E;
						isUnassigned = true;
						InsertPoint(newNode->RightChild()->LeftChild(), pair);
						CopyNode(node->LeftChild(), newNode->RightChild()->RightChild());
					}
				}
				else
				{
					pair.ValueType = Number;
					pair.Value.Number = 0;
					isUnassigned = true;
					InsertPoint(newNode, pair);
				}
				break;
			}
			case Expression::Log:
			{
				pair.ValueType = X;
				bool left, right;
				left = Contains(node->LeftChild(), pair);
				right = Contains(node->RightChild(), pair);
				if (left && right)
				{
					pair.ValueType = Operation;
					pair.Value.Operation = Expression::Divide;
					isUnassigned = true;
					InsertPoint(newNode, pair);
					pair.Value.Operation = Expression::Power;
					isUnassigned = true;
					InsertPoint(newNode->RightChild(), pair);
					pair.ValueType = Number;
					pair.Value.Number = 2;
					isUnassigned = true;
					InsertPoint(newNode->RightChild()->RightChild(), pair);
					pair.ValueType = Operation;
					pair.Value.Operation = Expression::Log;
					isUnassigned = true;
					InsertPoint(newNode->RightChild()->LeftChild(), pair);
					pair.ValueType = Number;
					pair.Value.Number = 10;
					isUnassigned = true;
					InsertPoint(newNode->RightChild()->LeftChild()->LeftChild(), pair);
					CopyNode(node->RightChild(), newNode->RightChild()->LeftChild()->RightChild());
					pair.ValueType = Operation;
					pair.Value.Operation = Expression::Minus;
					isUnassigned = true;
					InsertPoint(newNode->LeftChild(), pair);
					pair.Value.Operation = Divide;
					isUnassigned = true;
					InsertPoint(newNode->LeftChild()->RightChild(), pair);
					isUnassigned = true;
					InsertPoint(newNode->LeftChild()->LeftChild(), pair);
					pair.ValueType = X;
					isUnassigned = true;
					InsertPoint(newNode->LeftChild()->RightChild()->RightChild(), pair);
					CopyNode(node->RightChild(), newNode->LeftChild()->LeftChild()->RightChild());
					pair.ValueType = Operation;
					pair.Value.Operation = Expression::Multiply;
					isUnassigned = true;
					InsertPoint(newNode->LeftChild()->LeftChild()->LeftChild(), pair);
					pair.Value.Operation = Expression::Log;
					isUnassigned = true;
					InsertPoint(newNode->LeftChild()->RightChild()->LeftChild(), pair);
					isUnassigned = true;
					InsertPoint(newNode->LeftChild()->LeftChild()->LeftChild()->RightChild(), pair);
					pair.ValueType = Number;
					pair.Value.Number = 10;
					isUnassigned = true;
					InsertPoint(newNode->LeftChild()->RightChild()->LeftChild()->LeftChild(), pair);
					isUnassigned = true;
					InsertPoint(newNode->LeftChild()->LeftChild()->LeftChild()->RightChild()->LeftChild(), pair);
					CopyNode(node->RightChild(), newNode->LeftChild()->RightChild()->LeftChild()->RightChild());
					CopyNode(node->LeftChild(), newNode->LeftChild()->LeftChild()->LeftChild()->RightChild()->RightChild());
					CreateDerivative(node->RightChild(), newNode->LeftChild()->LeftChild()->LeftChild()->LeftChild());
				}
				else if (right)
				{
					if (node->LeftChild()->ContentType() == E && node->RightChild()->ContentType() == X)
					{
						pair.ValueType = Operation;
						pair.Value.Operation = Expression::Divide;
						isUnassigned = true;
						InsertPoint(newNode, pair);
						pair.ValueType = Number;
						pair.Value.Number = 1;
						isUnassigned = true;
						InsertPoint(newNode->LeftChild(), pair);
						pair.ValueType = X;
						pair.Value.X = 'x';
						isUnassigned = true;
						InsertPoint(newNode->RightChild(), pair);
					}
					else
					{
						pair.ValueType = Operation;
						pair.Value.Operation = Expression::Divide;
						isUnassigned = true;
						InsertPoint(newNode, pair);
						pair.Value.Operation = Expression::Multiply;
						isUnassigned = true;
						InsertPoint(newNode->RightChild(), pair);
						pair.Value.Operation = Expression::Log;
						isUnassigned = true;
						InsertPoint(newNode->RightChild()->RightChild(), pair);
						pair.ValueType = Number;
						pair.Value.Number = 10;
						isUnassigned = true;
						InsertPoint(newNode->RightChild()->RightChild()->LeftChild(), pair);
						CopyNode(node->LeftChild(), newNode->RightChild()->RightChild()->RightChild());
						CopyNode(node->RightChild(), newNode->RightChild()->LeftChild());
						CreateDerivative(node->RightChild(), newNode->LeftChild());
					}
				}
				else if (left)
				{
					pair.ValueType = Operation;
					pair.Value.Operation = Expression::Divide;
					isUnassigned = true;
					InsertPoint(newNode, pair);
					pair.Value.Operation = Expression::Multiply;
					isUnassigned = true;
					InsertPoint(newNode->RightChild(), pair);
					isUnassigned = true;
					InsertPoint(newNode->LeftChild(), pair);
					isUnassigned = true;
					InsertPoint(newNode->LeftChild()->LeftChild(), pair);
					pair.Value.Operation = Expression::Power;
					isUnassigned = true;
					InsertPoint(newNode->RightChild()->RightChild(), pair);
					pair.Value.Operation = Expression::Log;
					isUnassigned = true;
					InsertPoint(newNode->RightChild()->RightChild()->LeftChild(), pair);
					isUnassigned = true;
					InsertPoint(newNode->LeftChild()->LeftChild()->RightChild(), pair);
					pair.ValueType = Number;
					pair.Value.Number = -1;
					isUnassigned = true;
					InsertPoint(newNode->LeftChild()->LeftChild()->LeftChild(), pair);
					pair.Value.Number = 2;
					isUnassigned = true;
					InsertPoint(newNode->RightChild()->RightChild()->RightChild(), pair);
					pair.Value.Number = 10;
					isUnassigned = true;
					InsertPoint(newNode->RightChild()->RightChild()->LeftChild()->LeftChild(), pair);
					isUnassigned = true;
					InsertPoint(newNode->LeftChild()->LeftChild()->RightChild()->LeftChild(), pair);
					CopyNode(node->RightChild(), newNode->LeftChild()->LeftChild()->RightChild()->RightChild());
					CopyNode(node->LeftChild(), newNode->RightChild()->LeftChild());
					CopyNode(node->LeftChild(), newNode->RightChild()->RightChild()->LeftChild()->RightChild());
					CreateDerivative(node->LeftChild(), newNode->LeftChild()->RightChild());
				}
				else
				{
					pair.ValueType = Number;
					pair.Value.Number = 0;
					isUnassigned = true;
					InsertPoint(newNode, pair);
				}
				break;
			}
			case Expression::RootSquare:
			{
				pair.Value.Operation = Expression::Divide;
				isUnassigned = true;
				InsertPoint(newNode, pair);
				pair.Value.Operation = Expression::Power;
				isUnassigned = true;
				InsertPoint(newNode->RightChild(), pair);
				pair.Value.Operation = Expression::Multiply;
				isUnassigned = true;
				InsertPoint(newNode->LeftChild(), pair);
				pair.ValueType = Number;
				pair.Value.Number = 0.5;
				isUnassigned = true;
				InsertPoint(newNode->RightChild()->RightChild(), pair);
				isUnassigned = true;
				InsertPoint(newNode->LeftChild()->LeftChild(), pair);
				CopyNode(node->RightChild(), newNode->RightChild()->LeftChild());
				CreateDerivative(node->RightChild(), newNode->LeftChild()->RightChild());
				break;
			}
			case Expression::Trig:
			{
				pair.ValueType = X;
				if (Contains(node->RightChild(), pair))
				{
					switch (node->LeftChild()->ValueExpression())
					{
					case Expression::Sin:
					{
						pair.ValueType = Operation;
						pair.Value.Operation = Multiply;
						isUnassigned = true;
						InsertPoint(newNode, pair);
						pair.Value.Operation = Trig;
						isUnassigned = true;
						InsertPoint(newNode->RightChild(), pair);
						pair.ValueType = Trigonometric;
						pair.Value.Operation = Expression::Cos;
						isUnassigned = true;
						InsertPoint(newNode->RightChild()->LeftChild(), pair);
						CopyNode(node->RightChild(), newNode->RightChild()->RightChild());
						CreateDerivative(node->RightChild(), newNode->LeftChild());
						break;
					}
					case Expression::Cos:
					{
						pair.ValueType = Operation;
						pair.Value.Operation = Multiply;
						isUnassigned = true;
						InsertPoint(newNode, pair);
						isUnassigned = true;
						InsertPoint(newNode->LeftChild(), pair);
						pair.Value.Operation = Expression::Trig;
						isUnassigned = true;
						InsertPoint(newNode->RightChild(), pair);
						pair.ValueType = Number;
						pair.Value.Number = -1;
						isUnassigned = true;
						InsertPoint(newNode->LeftChild()->LeftChild(), pair);
						pair.ValueType = Trigonometric;
						pair.Value.Operation = Expression::Sin;
						isUnassigned = true;
						InsertPoint(newNode->RightChild()->LeftChild(), pair);
						CopyNode(node->RightChild(), newNode->RightChild()->RightChild());
						CreateDerivative(node->RightChild(), newNode->LeftChild()->RightChild());
						break;
					}
					case Expression::Tg:
					{
						pair.ValueType = Operation;
						pair.Value.Operation = Expression::Divide;
						isUnassigned = true;
						InsertPoint(newNode, pair);
						pair.Value.Operation = Expression::Power;
						isUnassigned = true;
						InsertPoint(newNode->RightChild(), pair);
						pair.Value.Operation = Expression::Trig;
						isUnassigned = true;
						InsertPoint(newNode->RightChild()->LeftChild(), pair);
						pair.ValueType = Trigonometric;
						pair.Value.Operation = Expression::Cos;
						isUnassigned = true;
						InsertPoint(newNode->RightChild()->LeftChild()->LeftChild(), pair);
						pair.ValueType = Number;
						pair.Value.Number = 2;
						isUnassigned = true;
						InsertPoint(newNode->RightChild()->RightChild(), pair);
						CopyNode(node->RightChild(), newNode->RightChild()->LeftChild()->RightChild());
						CreateDerivative(node->RightChild(), newNode->LeftChild());
						break;
					}
					case Expression::Ctg:
					{
						pair.ValueType = Operation;
						pair.Value.Operation = Expression::Divide;
						isUnassigned = true;
						InsertPoint(newNode, pair);
						pair.Value.Operation = Expression::Multiply;
						isUnassigned = true;
						InsertPoint(newNode->LeftChild(), pair);
						pair.Value.Operation = Expression::Power;
						isUnassigned = true;
						InsertPoint(newNode->RightChild(), pair);
						pair.Value.Operation = Expression::Trig;
						isUnassigned = true;
						InsertPoint(newNode->RightChild()->LeftChild(), pair);
						pair.ValueType = Trigonometric;
						pair.Value.Operation = Expression::Sin;
						isUnassigned = true;
						InsertPoint(newNode->RightChild()->LeftChild()->LeftChild(), pair);
						pair.ValueType = Number;
						pair.Value.Number = -1;
						isUnassigned = true;
						InsertPoint(newNode->LeftChild()->LeftChild(), pair);
						pair.Value.Number = 2;
						isUnassigned = true;
						InsertPoint(newNode->RightChild()->RightChild(), pair);
						CopyNode(node->RightChild(), newNode->RightChild()->LeftChild()->RightChild());
						CreateDerivative(node->RightChild(), newNode->LeftChild()->RightChild());
						break;
					}
					}
				}
				else
				{
					pair.ValueType = Number;
					pair.Value.Number = 0;
					isUnassigned = true;
					InsertPoint(newNode, pair);
				}
				break;
			}
			}
			break;
		}
		}
	}

	void Derivative::Simplify(Point * & node)
	{
		if (node != nullptr)
		{
			if (node->LeftChild() != nullptr && node->RightChild() != nullptr)
			{
				Content value;
				Type left = node->LeftChild()->ContentType(), right = node->RightChild()->ContentType();
				if (left == Operation)
					Simplify(node->LeftChild());
				if (right == Operation)
					Simplify(node->RightChild());
				left = node->LeftChild()->ContentType();
				right = node->RightChild()->ContentType();
				if (left == Number)
				{
					if (node->LeftChild()->ValueNumber() == 0)
					{
						switch (node->ValueExpression())
						{
						case Expression::Divide:
						case Expression::Multiply:
						{
							delete node;
							value.Number = 0;
							node = new Point(Number, value);
							return;
						}
						case Expression::Plus:
						{
							Point * tmp = nullptr;
							CopyNode(node->RightChild(), tmp);
							delete node;
							node = move(tmp);
							return;
						}
						case Expression::Minus:
						{
							if (node->RightChild()->ContentType() == Number)
							{
								value.Number = -1 * node->RightChild()->ValueNumber();
								delete node;
								node = new Point(Number, value);
							}
							else
							{
								value.Operation = Expression::Multiply;
								node->ChangeContent(Operation, value);
								value.Number = -1;
								node->LeftChild()->ChangeContent(Number, value);
							}
							return;
						}
						case Expression::Power:
						{
							if (node->RightChild()->ContentType() == Number && node->RightChild()->ValueNumber() == 0)
								break;
							else
							{
								value.Number = 0;
								delete node;
								node = new Point(Number, value);
								return;
							}
						}
						}
					}
					else if (node->LeftChild()->ValueNumber() == 1)
					{
						switch (node->ValueExpression())
						{
						case Expression::Multiply:
						{
							Point * point = nullptr;
							CopyNode(node->RightChild(), point);
							delete node;
							node = move(point);
							return;
						}
						case Expression::Power:
						{
							value.Number = 1;
							delete node;
							node = new Point(Number, value);
							return;
						}
						}
					}
				}
				if (right == Number)
				{
					if (node->RightChild()->ValueNumber() == 0)
					{
						switch (node->ContentType())
						{
						case Expression::Plus:
						case Expression::Minus:
						{
							Point * point = nullptr;
							CopyNode(node->LeftChild(), point);
							delete node;
							node = move(point);
							return;
						}
						case Expression::Multiply:
						{
							delete node;
							value.Number = 0;
							node = new Point(Number, value);
							return;
						}
						case Expression::Power:
						{
							delete node;
							value.Number = 1;
							node = new Point(Number, value);
							return;
						}
						}
					}
					else if (node->RightChild()->ValueNumber() == 1)
					{
						switch (node->ValueExpression())
						{
						case Expression::Multiply:
						case Expression::Power:
						case Expression::Divide:
						{
							Point * point = nullptr;
							CopyNode(node->LeftChild(), point);
							delete node;
							node = move(point);
							return;
						}
						}
					}
				}
				if (node->ContentType() == Operation && node->ValueExpression() == Expression::Multiply)
				{
					if (node->LeftChild()->ContentType() == Number && node->RightChild()->ContentType() == Operation && node->RightChild()->ValueExpression() == Expression::Multiply)
					{
						if (node->RightChild()->LeftChild()->ContentType() == Number)
						{
							value.Number = node->LeftChild()->ValueNumber() * node->RightChild()->LeftChild()->ValueNumber();
							node->RightChild()->LeftChild()->ChangeContent(Number, value);
							Point * point = nullptr;
							CopyNode(node->RightChild(), point);
							delete node;
							node = move(point);
							Simplify(node);
						}
						else if (node->RightChild()->RightChild()->ContentType() == Number)
						{
							value.Number = node->LeftChild()->ValueNumber() * node->RightChild()->RightChild()->ValueNumber();
							node->RightChild()->RightChild()->ChangeContent(Number, value);
							Point * point = nullptr;
							CopyNode(node->RightChild(), point);
							delete node;
							node = move(point);
							Simplify(node);
						}
					}
					else if (node->RightChild()->ContentType() == Number && node->LeftChild()->ContentType() == Operation && node->LeftChild()->ValueExpression() == Expression::Multiply)
					{
						if (node->LeftChild()->LeftChild()->ContentType() == Number)
						{
							value.Number = node->RightChild()->ValueNumber() * node->LeftChild()->LeftChild()->ValueNumber();
							node->LeftChild()->LeftChild()->ChangeContent(Number, value);
							Point * point = nullptr;
							CopyNode(node->LeftChild(), point);
							delete node;
							node = move(point);
							Simplify(node);
						}
						else if (node->LeftChild()->RightChild()->ContentType() == Number)
						{
							value.Number = node->RightChild()->ValueNumber() * node->LeftChild()->RightChild()->ValueNumber();
							node->LeftChild()->RightChild()->ChangeContent(Number, value);
							Point * point = nullptr;
							CopyNode(node->LeftChild(), point);
							delete node;
							node = move(point);
							Simplify(node);
						}
					}
					else if (node->LeftChild()->ContentType() == X && node->RightChild()->ContentType() == Operation && node->RightChild()->ValueExpression() == Expression::Power)
					{
						if (node->RightChild()->LeftChild()->ContentType() == X && node->RightChild()->RightChild()->ContentType() == Number)
						{
							value.Number = node->RightChild()->RightChild()->ValueNumber();
							node->RightChild()->RightChild()->ChangeContent(Number, value);
							Point * point = nullptr;
							CopyNode(node->RightChild(), point);
							delete node;
							node = move(point);
							Simplify(node);
						}
					}
					else if (node->RightChild()->ContentType() == X && node->LeftChild()->ContentType() == Operation && node->LeftChild()->ValueExpression() == Expression::Power)
					{
						if (node->LeftChild()->LeftChild()->ContentType() == X && node->LeftChild()->RightChild()->ContentType() == Number)
						{
							value.Number = node->LeftChild()->RightChild()->ValueNumber() + 1;
							node->LeftChild()->RightChild()->ChangeContent(Number, value);
							Point * point = nullptr;
							CopyNode(node->LeftChild(), point);
							delete node;
							node = move(point);
							Simplify(node);
						}
					}
				}
				if (left == Number && right == Number)
				{
					switch (node->ValueExpression())
					{
					case Expression::Plus:
					{
						value.Number = node->LeftChild()->ValueNumber() + node->RightChild()->ValueNumber();
						break;
					}
					case Expression::Minus:
					{
						value.Number = node->LeftChild()->ValueNumber() - node->RightChild()->ValueNumber();
						break;
					}
					case Expression::Multiply:
					{
						value.Number = node->LeftChild()->ValueNumber() * node->RightChild()->ValueNumber();
						break;
					}
					case Expression::Divide:
					{
						value.Number = node->LeftChild()->ValueNumber() / node->RightChild()->ValueNumber();
						break;
					}
					case Expression::Log:
					{
						value.Number = log(node->RightChild()->ValueNumber()) / log(node->LeftChild()->ValueNumber());
						break;
					}
					case Expression::Power:
					{
						value.Number = pow(node->LeftChild()->ValueNumber(), node->RightChild()->ValueNumber());
						break;
					}
					}
					delete node;
					node = new Point(Number, value);
				}
				else if (left == Trigonometric && right == Number)
				{
					switch (node->LeftChild()->ValueExpression())
					{
					case Expression::Sin:
					{
						value.Number = sin(node->RightChild()->ValueNumber());
						break;
					}
					case Expression::Cos:
					{
						value.Number = cos(node->RightChild()->ValueNumber());
						break;
					}
					case Expression::Tg:
					{
						value.Number = tan(node->RightChild()->ValueNumber());
						break;
					}
					case Expression::Ctg:
					{
						value.Number = 1 / tan(node->RightChild()->ValueNumber());
						break;
					}
					}
					delete node;
					node = new Point(Number, value);
				}
				else if ((left == X && right == Number && node->RightChild()->ValueNumber() == 1 && (node->ValueExpression() == Expression::Multiply || node->ValueExpression() == Expression::Power)) || (right == X && left == Number && node->ValueExpression() == Expression::Multiply && node->LeftChild()->ValueNumber() == 1))
				{
					delete node;
					value.X = 'x';
					node = new Point(X, value);
				}
				else if (right == X && left == X && node->ValueExpression() == Expression::Multiply)
				{
					delete node;
					value.Operation = Expression::Power;
					node = new Point(Operation, value);
					value.X = 'x';
					node->LeftChild() = new Point(X, value);
					value.Number = 2;
					node->RightChild() = new Point(Number, value);
				}
				
			}
		}
		return;
	}

	short Derivative::OperationPriority(const Expression & expression) const
	{
		switch (expression)
		{
		case Expression::Minus:
		case Expression::Plus:
			return 1;
		case Expression::Multiply:
		case Expression::Divide:
			return 2;
		case Expression::Power:
		case Expression::Log:
		case Expression::Trig:
			return 3;
		}
	}

	void Derivative::ToInfix(Point * point, string & txt) const
	{
		if (point != nullptr)
		{
			if (point->ContentType() == Number)
                txt += ONP::NumberToString(point->ValueNumber()) + " ";
			else if (point->ContentType() == E)
				txt += "e ";
			else if (point->ContentType() == Pi)
				txt += "PI ";
			else if (point->ContentType() == X)
				txt += "x ";
			else if (point->LeftChild() != nullptr && point->RightChild() != nullptr)
			{
				if (point->ContentType() == Operation && point->ValueExpression() == Expression::Trig)
				{
					switch (point->LeftChild()->ValueExpression())
					{
					case Expression::Cos:
					{
						txt += "cos (";
						break;
					}
					case Expression::Ctg:
					{
						txt += "ctg (";
						break;
					}
					case Expression::Sin:
					{
						txt += "sin (";
						break;
					}
					case Expression::Tg:
					{
						txt += "tg (";
						break;
					}
					}
					if (point->RightChild()->ContentType() == Number)
						txt += ONP::NumberToString(point->RightChild()->ValueNumber()) + ") ";
					else if ((point->RightChild()->ContentType() == X))
						txt += "x) ";
					else
					{
						ToInfix(point->RightChild(), txt);
						txt.pop_back();
						txt += ") ";
					}
				}
				else if (point->ContentType() == Operation && point->ValueExpression() == Expression::Log)
				{
					if (point->LeftChild()->ContentType() == E)
                        txt += "ln(";
					else
					{
						txt += "log(";
						if (point->LeftChild()->ContentType() == Number)
						{
							txt += ONP::NumberToString(point->LeftChild()->ValueNumber());
							txt += ") (";
						}
						else if (point->LeftChild()->ContentType() == X)
							txt += "x) (";
						else
						{
							ToInfix(point->LeftChild(), txt);
							txt.pop_back();
							txt += ") (";
						}
					}
					ToInfix(point->RightChild(), txt);
					txt.pop_back();
					txt += ") ";
				}
				else
				{
					if (point->LeftChild()->ContentType() == Number)
						txt += ONP::NumberToString(point->LeftChild()->ValueNumber()) + " ";
					else if (point->LeftChild()->ContentType() == X)
						txt += "x ";
					else
					{
						if (OperationPriority(point->LeftChild()->ValueExpression()) < OperationPriority(point->ValueExpression()))
						{
							txt += "(";
							ToInfix(point->LeftChild(), txt);
							txt.pop_back();
							txt += ") ";
						}
						else
							ToInfix(point->LeftChild(), txt);
					}
					txt += point->ValueExpression();
					txt += " ";
					if (point->RightChild()->ContentType() == Number)
						txt += ONP::NumberToString(point->RightChild()->ValueNumber()) + " ";
					else if (point->RightChild()->ContentType() == X)
						txt += "x ";
					else
					{
						if (OperationPriority(point->RightChild()->ValueExpression()) <= OperationPriority(point->ValueExpression()))
						{
							txt += "(";
							ToInfix(point->RightChild(), txt);
							txt.pop_back();
							txt += ") ";
						}
						else
							ToInfix(point->RightChild(), txt);
					}
				}
			}
		}
	}

	void Derivative::HowManyOperations(Point * node, int & count)
	{
		if (node != nullptr)
		{
			if (node->ContentType() == Operation)
			{
				++count;
				HowManyOperations(node->RightChild(), count);
				HowManyOperations(node->LeftChild(), count);
			}
		}
	}

	long double Derivative::GetValue(long double x)
	{
		Pair pair;
		pair.ValueType = X;
		if (Contains(mainNodeRoot, pair))
		{
			string txt = "", tmp = ONP::NumberToString(x);
			mainNodeRoot->TextValue(txt);
			size_t position = txt.find('x');
			while (position != string::npos)
			{
				txt.erase(position, 1);
				txt.insert(position, tmp);
				position = txt.find('x');
			}
			return ONP::ValueFromONP(txt);
		}
		else
		{
			Simplify(mainNodeRoot);
			return mainNodeRoot->ValueNumber();
		}
	}

	string Derivative::ToString()
	{
		string txt = "f'(x)=";
		ToInfix(mainNodeRoot, txt);
		return txt;
	}

    string getDerivative(const string & txt)
	{
		Derivative * der = new Derivative(txt, true);
		return der->ToString();
	}

    long double getDerivativeValue(const string & txt, const long double & x)
	{
		Derivative * der = new Derivative(txt, true);
		return der->GetValue(x);
	}

	long double getFunctionValue(const string & txt, const long double & x)
	{
		Derivative * der = new Derivative(txt, false);
		return der->GetValue(x);
	}
}
