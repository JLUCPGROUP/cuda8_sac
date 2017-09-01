#pragma once
#include <map>
#include <functional>
#include <vector>
#include "XModel.h"
#include "HModel.h"

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

class HBuilder {
public:
	std::vector<HDomMap> domms;
	XModel* xm;
	HModel* hm;

	HBuilder(XModel* xmodel, HModel* hmodel);
	~HBuilder() {};

	/**
	*\brief 显示HModel
	*/
	void ShowHModel();
private:
	void assign(int* lh, const int* rh, const int len);

	void getIntTuple(int idx, int *t, const XCon* c);

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
	void EXTConToTab(const XCon *c);

	/**
	*\brief 转标准表约束
	*/
	void modifyTuple(HTab* t, std::vector<DomType>& type);

};

}


