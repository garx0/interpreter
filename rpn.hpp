#pragma once

#include <iostream>
#include <vector>
#include <stack>
#include <cassert>

#include "table.hpp"
#include "lex.hpp"

using namespace std;

struct RpnContext {
	stack<int> st;
	int eip;
};

class RpnOp {
public:
	virtual void calc(RpnContext& context) const = 0;
	friend ostream& operator<<(ostream& stream, const RpnOp& op);
protected:
	virtual void print(ostream& stream) const = 0;
};

ostream& operator<<(ostream& stream, const RpnOp& op);

class RpnBinOp: public RpnOp {
public:
	virtual void calc(RpnContext& context) const override;
protected:
	virtual void doCalc(RpnContext& context) const = 0;
};

class RpnPut: public RpnOp {
	LexT type;
	int value;
public:
	RpnPut(LexT a_type = LexT::LEX_NULL, int a_value = 0):
		type(a_type), value(a_value) {}
	virtual void calc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnSemicolon: public RpnOp {
public:
	virtual void calc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnAdd: public RpnBinOp {
public:
	virtual void doCalc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnSub: public RpnBinOp {
public:
	virtual void doCalc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnMul: public RpnBinOp {
public:
	virtual void doCalc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnDiv: public RpnBinOp {
public:
	virtual void doCalc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnMod: public RpnBinOp {
public:
	virtual void doCalc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnAnd: public RpnBinOp {
public:
	virtual void doCalc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnOr: public RpnBinOp {
public:
	virtual void doCalc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnNot: public RpnOp {
public:
	virtual void calc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnEq: public RpnBinOp {
public:
	virtual void doCalc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnNe: public RpnBinOp {
public:
	virtual void doCalc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnLe: public RpnBinOp {
public:
	virtual void doCalc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnGe: public RpnBinOp {
public:
	virtual void doCalc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnLt: public RpnBinOp {
public:
	virtual void doCalc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnGt: public RpnBinOp {
public:
	virtual void doCalc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnAssign: public RpnBinOp {
public:
	virtual void doCalc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnGo: public RpnOp {
public:
	virtual void calc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnFgo: public RpnOp {
public:
	virtual void calc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnLabel: public RpnOp {
	int address;
public:
	RpnLabel(int a_address = 0): address(a_address) {}
	virtual void calc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnAddress: public RpnOp {
public:
	virtual void calc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnArgc: public RpnOp {
//используется для функции write
	int num;
public:
	RpnArgc(int a_num = 0): num(a_num) {}
	virtual void calc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnRead: public RpnOp {
public:
	virtual void calc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnWrite: public RpnOp {
public:
	virtual void calc(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};
