#pragma once

#include <iostream>
#include <vector>
#include <stack>
#include <cassert>

#include "table.hpp"
#include "lex.hpp"
//~ #include "parser.hpp"

using namespace std;

enum RpnT {
	RPN_NULL,		//0
	INT,			//1
	BOOLEAN,		//2
	STRING,			//3
	VAR_INT,		//4
	VAR_BOOLEAN,	//5
	VAR_STRING,		//6
	ADDRESS,		//7
	LABEL,			//8
	ARGC			//9
};

bool rpnEqTypes(RpnT type1, RpnT type2);
bool isVarType(RpnT type);

struct RpnOperand {
	RpnT type = RpnT::RPN_NULL;
	int value;
	string str;
	RpnOperand() = default;
	RpnOperand(RpnT a_type, int a_value):
		type(a_type), value(a_value) {}
	RpnOperand(RpnT a_type, string a_str):
		type(a_type), str(a_str)
	{
		assert(rpnEqTypes(a_type, RpnT::STRING)); //DEBUG
	}
};

struct RpnContext {
	stack<RpnOperand> st;
	int eip = 0;
	Table<Ident> tid;
	Table<string> tstr;
	
	RpnContext(Table<Ident> a_tid, Table<string> a_tstr) 
	{
		tid = a_tid;
		tstr = a_tstr;
	}
};

RpnT rpnConvertType(const Lex& lex, const RpnContext& context);
RpnT rpnConvertType(const Lex& lex);

class RpnOp {
public:
	virtual void execute(RpnContext& context) const = 0;
	friend ostream& operator<<(ostream& stream, const RpnOp& op);
protected:
	virtual void print(ostream& stream) const = 0;
};

ostream& operator<<(ostream& stream, const RpnOp& op);

class RpnBinOp: public RpnOp {
public:
	virtual void execute(RpnContext& context) const override;
protected:
	virtual RpnOperand calc(const RpnOperand& op1, 
		const RpnOperand& op2, RpnContext& context) const = 0;
};

class RpnPut: public RpnOp {
	RpnT type;
	int value;
public:
	RpnPut(RpnT a_type = RpnT::RPN_NULL, int a_value = 0):
		type(a_type), value(a_value) {}
	RpnPut(Lex lex):
		type(rpnConvertType(lex)), value(lex.getValue()) {}
	RpnPut(Lex lex, int a_value):
		type(rpnConvertType(lex)), value(a_value) {}
	virtual void execute(RpnContext& context) const override;
	RpnT getType() const {return type;}
	int getValue() const {return value;}
protected:
	virtual void print(ostream& stream) const override;
};

class RpnSemicolon: public RpnOp {
public:
	virtual void execute(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnAdd: public RpnBinOp {
protected:
	virtual RpnOperand calc(const RpnOperand& op1, 
		const RpnOperand& op2, RpnContext& context) const override;
	virtual void print(ostream& stream) const override;
};

class RpnSub: public RpnBinOp {
protected:
	virtual RpnOperand calc(const RpnOperand& op1, 
		const RpnOperand& op2, RpnContext& context) const override;
	virtual void print(ostream& stream) const override;
};

class RpnMul: public RpnBinOp {
protected:
	virtual RpnOperand calc(const RpnOperand& op1, 
		const RpnOperand& op2, RpnContext& context) const override;
	virtual void print(ostream& stream) const override;
};

class RpnDiv: public RpnBinOp {
protected:
	virtual RpnOperand calc(const RpnOperand& op1, 
		const RpnOperand& op2, RpnContext& context) const override;
	virtual void print(ostream& stream) const override;
};

class RpnMod: public RpnBinOp {
protected:
	virtual RpnOperand calc(const RpnOperand& op1, 
		const RpnOperand& op2, RpnContext& context) const override;
	virtual void print(ostream& stream) const override;
};

class RpnLogOp: public RpnBinOp {
protected:
	virtual RpnOperand calc(const RpnOperand& op1, 
		const RpnOperand& op2, RpnContext& context) const override;
	virtual int doCalc(int a, int b) const = 0;
};

class RpnCmpOp: public RpnBinOp {
protected:
	virtual RpnOperand calc(const RpnOperand& op1, 
		const RpnOperand& op2, RpnContext& context) const override;
	virtual int doCalc(int a, int b) const = 0;
};

class RpnAnd: public RpnLogOp {
protected:
	virtual int doCalc(int a, int b) const override;
	virtual void print(ostream& stream) const override;
};

class RpnOr: public RpnLogOp {
protected:
	virtual int doCalc(int a, int b) const override;
	virtual void print(ostream& stream) const override;
};

class RpnNot: public RpnOp {
public:
	virtual void execute(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnEq: public RpnCmpOp {
protected:
	virtual int doCalc(int a, int b) const override;
	virtual void print(ostream& stream) const override;
};

class RpnNe: public RpnCmpOp {
protected:
	virtual int doCalc(int a, int b) const override;
	virtual void print(ostream& stream) const override;
};

class RpnLe: public RpnCmpOp {
protected:
	virtual int doCalc(int a, int b) const override;
	virtual void print(ostream& stream) const override;
};

class RpnGe: public RpnCmpOp {
protected:
	virtual int doCalc(int a, int b) const override;
	virtual void print(ostream& stream) const override;
};

class RpnLt: public RpnCmpOp {
protected:
	virtual int doCalc(int a, int b) const override;
	virtual void print(ostream& stream) const override;
};

class RpnGt: public RpnCmpOp {
protected:
	virtual int doCalc(int a, int b) const override;
	virtual void print(ostream& stream) const override;
};

class RpnAssign: public RpnBinOp {
protected:
	virtual RpnOperand calc(const RpnOperand& op1, 
		const RpnOperand& op2, RpnContext& context) const override;
	virtual void print(ostream& stream) const override;
};

class RpnJump: public RpnOp {
public:
	virtual void execute(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnJumpTrue: public RpnBinOp {
protected:
	virtual RpnOperand calc(const RpnOperand& op1, 
		const RpnOperand& op2, RpnContext& context) const override;
	virtual void print(ostream& stream) const override;
};

class RpnJumpFalse: public RpnBinOp {
protected:
	virtual RpnOperand calc(const RpnOperand& op1, 
		const RpnOperand& op2, RpnContext& context) const override;
	virtual void print(ostream& stream) const override;
};

class RpnLabel: public RpnOp {
	int value;
public:
	RpnLabel(int a_value = 0): value(a_value) {}
	virtual void execute(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnAddress: public RpnOp {
	int value;
public:
	RpnAddress(int a_value = 0): value(a_value) {}
	virtual void execute(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnArgc: public RpnOp {
//используется для функции write
	int value;
public:
	RpnArgc(int a_value = 0): value(a_value) {}
	virtual void execute(RpnContext& context) const override;
	int getValue() const {return value;}
protected:
	virtual void print(ostream& stream) const override;
};

class RpnRead: public RpnOp {
public:
	virtual void execute(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

class RpnWrite: public RpnOp {
public:
	virtual void execute(RpnContext& context) const override;
protected:
	virtual void print(ostream& stream) const override;
};

void idToAddr(RpnOp*& abstrPtr);
// заменяет rpn-операцию put(значение идентификатора) на
// операцию-адрес идентификатора
// (*abstrPtr) должен быть типа RpnPut
// (*abstrPtr) должен быть порожден при помощи new

class RpnProgram {
	vector<RpnOp*> rpn;
	RpnContext context;
public:
	RpnProgram(const vector<RpnOp*>& a_rpn,
		const Table<Ident>& a_tid, const Table<string>& a_tstr);
	//~ RpnProgram(const Parser& parser,
		//~ const Table<Ident>& a_tid, const Table<string>& a_tstr);
	void execute();
};
	
