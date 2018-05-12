#pragma once

#include <iostream>
#include <vector>
#include <stack>
#include <cassert>
#include <fstream>

#include "table.hpp"
#include "lex.hpp"
#include "parser.hpp"
#include "utilfuncs.hpp"

using namespace std;

struct RpnContext {
	stack<int> st;
	int eip;
};

class RpnOp {
public:
	virtual void calc(RpnContext& context) const = 0;
};

class RpnBinOp: public RpnOp {
public:
	virtual void calc(RpnContext& context) const override;
protected:
	virtual void doCalc(RpnContext& context) const = 0;
};

class RpnPutOp: public RpnOp {
	int value;
public:
	virtual void calc(RpnContext& context) const override;
};

class RpnAddOp: public RpnBinOp {
public:
	virtual void doCalc(RpnContext& context) const override;
};

class RpnSubOp: public RpnBinOp {
public:
	virtual void doCalc(RpnContext& context) const override;
};

class RpnMulOp: public RpnBinOp {
public:
	virtual void doCalc(RpnContext& context) const override;
};

class RpnDivOp: public RpnBinOp {
public:
	virtual void doCalc(RpnContext& context) const override;
};

class RpnModOp: public RpnBinOp {
public:
	virtual void doCalc(RpnContext& context) const override;
};

class RpnGoOp: public RpnOp {
public:
	virtual void calc(RpnContext& context) const override;
};

class RpnFgoOp: public RpnOp {
public:
	virtual void calc(RpnContext& context) const override;
};

class RpnLabelOp: public RpnOp {
public:
	virtual void calc(RpnContext& context) const override;
};

class RpnAddressOp: public RpnOp {
public:
	virtual void calc(RpnContext& context) const override;
};
