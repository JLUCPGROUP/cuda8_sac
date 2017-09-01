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
	*\brief ��ʾHModel
	*/
	void ShowHModel();
private:
	void assign(int* lh, const int* rh, const int len);

	void getIntTuple(int idx, int *t, const XCon* c);

	/**
	*\brief �ж�Ԫ���Ƿ����
	*/
	bool isEqual(const int* lh, const int* rh, const int len);

	/**
	*\brief ����HTab
	*/
	//void makeHTab(HTab* t, const XCon *c, const XRel* r, const int sup_size);

	/**
	*\brief ����HDomMap
	*/
	void generateHDomMap();

	/**
	*\brief ����HVars
	*/
	void generateHVars();

	/**
	*\brief ����HTab
	*/
	void generateHTab();

	/**
	*\brief ��Լ��תHTab
	*/
	void EXTConToTab(const XCon *c);

	/**
	*\brief ת��׼��Լ��
	*/
	void modifyTuple(HTab* t, std::vector<DomType>& type);

};

}


