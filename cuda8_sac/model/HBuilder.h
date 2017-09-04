#pragma once
#include <map>
#include <functional>
#include <vector>
#include <array>
#include <functional>
#include <iostream>
#include "model/XModel.h"
#include "model/HModel.h"

namespace cudacp {



class HDomMap {
public:
	int id;
	std::map<int, int>m;
	DomType type = DT_STD;
	HDomMap() {};
	void MakeMap(XDom* d);
	~HDomMap() {};
};

class API_DECLSPEC HBuilder {
public:
	std::vector<HDomMap> domms;
	XModel* xm;
	HModel* hm;
	//std::function<int(int*)> func;
	std::map<PredicateOperator, std::function<int(int*)>> ops = {
		{ PO_ADD ,op_add},
		{ PO_SUB ,op_sub},
		{ PO_MUL ,op_mul},
		{ PO_DIV,op_div},
		{ PO_MOD,op_mod },
		{ PO_EQ, op_eq },
		{ PO_NE ,op_ne },
		{ PO_LT,op_lt },
		{ PO_LE ,op_le },
		{ PO_GT,op_gt },
		{ PO_GE,op_ge },
		{ PO_ABS , op_abs },
		{ PO_AND ,op_and },
		{ PO_OR ,op_or },
	};
	HBuilder(XModel* xmodel, HModel* hmodel);
	~HBuilder() {};

	/**
	*\brief 显示HModel
	*/
	void ShowHModel();
private:
	void assign(int* lh, const int* rh, const int len);

	void getIntTuple(int idx, int *t, const XCon* c);
	void getIntTuple(int idx, std::vector<int>& t, const XCon * c);
	/**
	*\brief 判断元组是否相等
	*/
	bool isEqual(const int* lh, const int* rh, const int len);

	/**
	*\brief 生成HTab
	*/
	//void makeHTab(HTab* t, const XCon *c, const XRel* r, const int sup_size);

	/**
	*\brief 生成HDomMap
	*/
	void generateHDomMap();

	/**
	*\brief 生成HVars
	*/
	void generateHVars();

	/**
	*\brief 生成HTab
	*/
	void generateHTab();

	/**
	*\brief 表约束转HTab
	*/
	void EXTConToTab(XCon *c);

	void INTConToTab(XCon *c);

	/**
	*\brief 转标准表约束
	*/
	void modifyTuple(HTab* t, std::vector<DomType>& type);

	void makeHTab(HTab* t, const int id, const int arity, const int size, std::vector<std::vector<int>>& tuples, int* scope) {};
	void makeHTab(HTab * t, std::vector<std::vector<int>>& tuples);
	/**
	*\brief 获取参数
	*是变量ID返回true
	*不是变量返回false 并返回参数
	*/
	bool getPara(int par_in, int *par);

	/**
	*\brief 计算结果
	*/
	int calulate(const PStack *pss, int *params);

	int getResult(PredicateOperator po, int *x, int *pars_top);

	static int op_or(int *x) {
		return x[0] || x[1];
	}

	static int op_and(int *x) {
		return x[0] && x[1];
	}

	static int op_eq(int *x) {
		return x[0] == x[1];
	}

	static int op_abs(int *x) {
		return fabsf(x[0]);
	}

	static int op_sub(int *x) {
		return x[0] - x[1];
	}

	static int op_div(int *x) {
		return x[0] / x[1];
	}

	static int op_mod(int *x) {
		return x[0] % x[1];
	}

	static int op_ne(int *x) {
		return x[0] != x[1];
	}

	static int op_add(int *x) {
		return x[0] + x[1];
	}

	static int op_mul(int *x) {
		return x[0] + x[1];
	}

	static int op_lt(int *x) {
		return x[0] < x[1];
	}

	static int op_le(int *x) {
		return x[0] <= x[1];
	}

	static int op_gt(int *x) {
		return x[0] > x[1];
	}

	static int op_ge(int *x) {
		return x[0] >= x[1];
	}
};

}


