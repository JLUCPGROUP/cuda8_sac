#pragma once

#define _EXPORTING
#ifdef _EXPORTING
#define API_DECLSPEC __declspec(dllexport)
#else
#define API_DECLSPEC __declspec(dllimport)
#endif

namespace cudacp {
enum DomType {
	//从零开始连续整数
	DT_STD = 1,
	//离散值
	DT_DP = 0
};

struct HProp {
	int max_dom_size;
	int vs_size;
	int ts_size;
};

class HVar {
public:
	int id;
	int size;
	int *vals;

	HVar(const int id, const int size);
	~HVar();
};

class HTab {
public:
	int id;
	int arity;
	int size;
	int** tuples;
	int* scope;
	HTab() {};
	HTab(const int id, const int arity, const int size, int* scp);
	HTab(const int id, const int arity, const int size, int** tps, int* scp);
	~HTab();
};


class API_DECLSPEC HModel {
public:
	HProp property;
	HVar** vars;
	HTab** tabs;
	HModel() {};
	~HModel();
};






}