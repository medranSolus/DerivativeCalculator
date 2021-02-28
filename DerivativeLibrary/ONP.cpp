/*
Author: Marek Machliñski
Date: 6.12.2017
*/
#include "stdafx.h"
#include "ONP.h"
namespace DerivativeLibrary
{
	void ONP::PutFunction(VecPair & out, VecPair & function, char sign, short number)
	{
		if (function.size() == 0)
		{
			Pair pair;
			pair.Value.Operation = Expression(sign);
			pair.Priority = number;
			pair.ValueType = Operation;
			function.push_back(pair);
		}
		else
		{
			if (function.size() > 0)
			{
				Pair & functionPair = function.back();
				if (functionPair.Priority < number)
				{
					Pair pair;
					pair.Value.Operation = Expression(sign);
					pair.Priority = number;
					pair.ValueType = Operation;
					function.push_back(pair);
				}
				else
				{
					while (functionPair.Priority >= number)
					{
						out.push_back(functionPair);
						function.pop_back();
						if (function.size() != 0)
							functionPair = function.back();
						else
							break;
					}
					Pair pair;
					pair.Value.Operation = Expression(sign);
					pair.Priority = number;
					pair.ValueType = Operation;
					function.push_back(pair);
				}
			}
		}
	}

	long double ONP::ONPValue(VecPair & ONP)
	{
		VecPair result;
        for (auto iter = ONP.begin(); iter != ONP.end(); ++iter)
		{
            if (iter->ValueType == Number || iter->ValueType == Trigonometric)
                result.push_back(*iter);
			else if (result.size() > 1)
			{
				Pair temp2 = result.back();
				result.pop_back();
				Pair temp1 = result.back();
				result.pop_back();
				Pair tmp;
				tmp.ValueType = Number;
                switch (iter->Value.Operation)
				{
				case Expression::Plus:
				{
					tmp.Value.Number = (temp1.Value.Number + temp2.Value.Number);
					result.push_back(tmp);
					break;
				}
				case Expression::Minus:
				{
					tmp.Value.Number = temp1.Value.Number - temp2.Value.Number;
					result.push_back(tmp);
					break;
				}
				case Expression::Multiply:
				{
					tmp.Value.Number = temp1.Value.Number * temp2.Value.Number;
					result.push_back(tmp);
					break;
				}
				case Expression::Divide:
				{
					if (!temp2.Value.Number)
                        return OUT_OF_RANGE;
					tmp.Value.Number = temp1.Value.Number / temp2.Value.Number;
					result.push_back(tmp);
					break;
				}
				case Expression::Log:
				{
                    if (temp1.Value.Number <= 0 || temp1.Value.Number == 1 || temp2.Value.Number <= 0)
						return OUT_OF_RANGE;
					tmp.Value.Number = log(temp2.Value.Number) / log(temp1.Value.Number);
					result.push_back(tmp);
					break;
				}
				case Expression::Power:
				{
                    if(temp1.Value.Number < 0 && temp2.Value.Number > 0 && temp2.Value.Number < 1)
                        return OUT_OF_RANGE;
					tmp.Value.Number = pow(temp1.Value.Number, temp2.Value.Number);
					result.push_back(tmp);
					break;
				}
				case Expression::Trig:
				{
					switch (temp1.Value.Operation)
					{
					case Expression::Cos:
					{
						tmp.Value.Number = cos(temp2.Value.Number);
						break;
					}
					case Expression::Sin:
					{
						tmp.Value.Number = sin(temp2.Value.Number);
						break;
					}
					case Expression::Tg:
					{
                        long long T = 2 * temp2.Value.Number / M_PI;
                        if(T)
                            return OUT_OF_RANGE;
						tmp.Value.Number = tan(temp2.Value.Number);
						break;
					}
					case Expression::Ctg:
					{
                        long long T = 2 * temp2.Value.Number / M_PI;
                        if(T)
                            return OUT_OF_RANGE;
						long double ctan = tan(temp2.Value.Number);
						if (!ctan)
                            return OUT_OF_RANGE;
						tmp.Value.Number = 1 / ctan;
						break;
					}
					}
					result.push_back(tmp);
					break;
				}
				}
			}
        }
		ONP.clear();
		return result.front().Value.Number;
	}

	VecPair ONP::ToONP(string txt)
	{
		VecPair ONP;
		VecPair functions;
		int I = 0;
		double value = 0.0;
		bool log = false, minus = false;
		size_t size = txt.size();
        while (I < size)
		{
			if (txt.at(I) == 'x')
			{
				Pair pair;
				pair.ValueType = X;
				pair.Value.X = 'x';
				pair.Priority = 5;
				ONP.push_back(pair);
			}
			else if (txt.at(I) == 'e')
			{
				Pair pair;
				pair.ValueType = E;
				pair.Value.Number = M_E;
				pair.Priority = 5;
				ONP.push_back(pair);
			}
			else if ((txt.at(I) == 'p' || txt.at(I) == 'P') && (txt.at(I + 1) == 'i' || txt.at(I + 1) == 'I'))
			{
				Pair pair;
				pair.ValueType = Pi;
				pair.Value.Number = M_PI;
				pair.Priority = 5;
				ONP.push_back(pair);
			}
			else if (txt.at(I) == '-')
			{
				if (I == 0 || txt.at(I - 1) == '(' || (txt.at(I - 1) == ' ' && txt.at(I - 2) == '('))
					txt.insert(I + 1, "1*");
				else
					txt.insert(I + 1, "+1*");
				minus = true;
				size = txt.size();
			}
			else if (isdigit(txt.at(I)))
			{
				int i = -1;
				string number = "";
				while (I < size && isalnum(txt.at(I)))
				{
					++i;
					number += txt.at(I);
					++I;
				}
				if (I < size && txt.at(I) == '.')
				{
					++I;
					int j = 0;
					while (isalnum(txt.at(I)))
					{
						value += (txt.at(I) - 48) / pow(10, ++j);
						++I;
					}
				}
				--I;
				for (int j = 0; i >= 0; --i, ++j)
					value += (number.at(i) - 48) * pow(10, j);
				if (minus)
				{
					value *= -1;
					minus = false;
				}
				Pair pair;
				pair.Value.Number = value;
				pair.Priority = 5;
				pair.ValueType = Number;
				ONP.push_back(pair);
				value = 0;
			}
			else
			{
				char temp = txt.at(I);
				switch (temp)
				{
				case '(':
				{
					Pair pair;
					pair.Value.Operation = Expression(temp);
					pair.Priority = 0;
					pair.ValueType = Operation;
					functions.push_back(pair);
					break;
				}
				case ')':
				{
					for (;;)
					{
						Pair pair;
						if (functions.size() > 0)
							pair = functions.back();
						else
							break;
						if (pair.Value.Operation != Expression::LeftBracket)
						{
							ONP.push_back(pair);
							functions.pop_back();
						}
						else
						{
							functions.pop_back();
							break;
						}
					}
					break;
				}
				case '-':
				case '+':
				{
					PutFunction(ONP, functions, temp, 1);
					break;
				}
				case '/':
				case '*':
				{
					PutFunction(ONP, functions, temp, 2);
					break;
				}
				case '^':
				{
					PutFunction(ONP, functions, temp, 3);
					break;
				}
				case 'v':
				case 'V':
				{
					Pair pair;
					pair.ValueType = Number;
					pair.Value.Number = 0.5;
					ONP.push_back(pair);
					PutFunction(ONP, functions, 'v', 3);
					break;
				}
				case 'l':
				{
					if (txt.at(I + 1 == 'n'))
					{
						Pair pair;
						pair.ValueType = E;
						pair.Value.Number = M_E;
						pair.Priority = 5;
						ONP.push_back(pair);
						PutFunction(ONP, functions, 'l', 3);
						++I;
					}
					else if (log)
					{
						PutFunction(ONP, functions, 'l', 3);
						log = false;
					}
					else
					{
						unsigned int i = 0, pos = 3;
						if (txt.at(I + pos) == ' ')
							++pos;
						do
						{
							if (txt.at(I + pos) == '(')
								++i;
							else if (txt.at(I + pos) == ')')
								--i;
							++pos;
						} while (i);
						txt.at(I + pos) = 'l';
						I += 2;
						log = true;
					}
					break;
				}
				case 's':
				{
					Pair pair;
					pair.ValueType = Trigonometric;
					pair.Priority = 5;
					pair.Value.Operation = Expression::Sin;
					ONP.push_back(pair);
					PutFunction(ONP, functions, 'T', 3);
					I += 3;
					break;
				}
				case 't':
				{
					Pair pair;
					pair.ValueType = Trigonometric;
					pair.Priority = 5;
					pair.Value.Operation = Expression::Tg;
					ONP.push_back(pair);
					PutFunction(ONP, functions, 'T', 3);
					I += 2;
					break;
				}
				case 'c':
				{
					Pair pair;
					pair.ValueType = Trigonometric;
					pair.Priority = 5;
					if (txt.at(I + 1) == 't')
						pair.Value.Operation = Expression::Ctg;
					else
						pair.Value.Operation = Expression::Cos;
					ONP.push_back(pair);
					PutFunction(ONP, functions, 'T', 3);
					I += 3;
					break;
				}
				}
			}
			++I;
        }
		for_each(functions.rbegin(), functions.rend(), [&ONP](Pair & x)
		{
			ONP.push_back(x);
		});
		functions.clear();
		return ONP;
	}

	long double ONP::ValueFromONP(string txt)
	{
		VecPair ONP;
		long double value = 0.0;
		int I = 0;
		size_t size = txt.size();
		bool minus = false;
		do
		{
			if (isspace(txt.at(I)))
			{
				++I;
				continue;
			}
			else if (txt.at(I) == 'e')
			{
				Pair pair;
				pair.ValueType = Number;
				pair.Value.Number = M_E;
				ONP.push_back(pair);
			}
			else if (txt.at(I) == 'P' || txt.at(I) == 'p')
			{
				Pair pair;
				pair.ValueType = Number;
				pair.Value.Number = M_PI;
				ONP.push_back(pair);
				++I;
			}
			else if (txt.at(I) == '-' && isdigit(txt.at(I + 1)))
				minus = true;
			else if (isdigit(txt.at(I)))
			{
				int i = -1;
				string number = "";
				while (I < size && isalnum(txt.at(I)))
				{
					++i;
					number += txt.at(I);
					++I;
				}
				if (I < size && txt.at(I) == '.')
				{
					++I;
					int j = 0;
					while (isalnum(txt.at(I)))
					{
						value += (txt.at(I) - 48) / pow(10, ++j);
						++I;
					}
				}
				--I;
				for (int j = 0; i >= 0; --i, ++j)
					value += (number.at(i) - 48) * pow(10, j);
				if (minus)
				{
					value *= -1;
					minus = false;
				}
				Pair pair;
				pair.Value.Number = value;
				pair.Priority = 5;
				pair.ValueType = Number;
				ONP.push_back(pair);
				value = 0;
			}
            else if(txt.at(I) == 's')
            {
                Pair pair;
                pair.ValueType = Trigonometric;
                pair.Value.Operation = Expression::Sin;
                ONP.push_back(pair);
            }
            else if(txt.at(I) == 't')
            {
                Pair pair;
                pair.ValueType = Trigonometric;
                pair.Value.Operation = Expression::Tg;
                ONP.push_back(pair);
            }
            else if(txt.at(I) == 'c')
            {
                Pair pair;
                pair.ValueType = Trigonometric;
                pair.Value.Operation = Expression::Cos;
                ONP.push_back(pair);
            }
            else if(txt.at(I) == 'C')
            {
                Pair pair;
                pair.ValueType = Trigonometric;
                pair.Value.Operation = Expression::Ctg;
                ONP.push_back(pair);
            }
            else if(txt.at(I) == 'T')
            {
                Pair pair;
                pair.Value.Operation = Expression::Trig;
                pair.ValueType = Operation;
                ONP.push_back(pair);
            }
			else if (!isalpha(txt.at(I)) && !isalnum(txt.at(I)))
			{
				char operation = txt.at(I++);
				Pair pair;
				pair.Value.Operation = Expression(operation);
				pair.ValueType = Operation;
				ONP.push_back(pair);
			}
			++I;
		} while (I < size);
		return ONPValue(ONP);
	}

	string ONP::NumberToString(long double x)
	{
		string text = to_string(x);
		if (text.find('.') != string::npos)
		{
			for (size_t I = text.size(), i = 0; ;)
			{
				if (text.at(--I) == '0')
					++i;
				else
				{
					if (text.at(I) != '.')
						++I;
					else
						++i;
					text.erase(I, i);
					break;
				}
			}
		}
		return text;
	}
}
