#include "model/HModel.h"

namespace cudacp {

HVar::HVar(const int id, const int size)
	:id(id), size(size) {
	vals = new int[size];
	for (size_t i = 0; i < size; i++)
		vals[i] = i;
}

HVar::~HVar() {
	delete[] vals;
	vals = nullptr;
}

HTab::HTab(const int id, const int arity, const int size, int * scp)
	:id(id), arity(arity), size(size) {
	scope = new int[arity];
	for (size_t i = 0; i < arity; ++i)
		scope[i] = scp[i];

	tuples = new int*[size];
	for (size_t i = 0; i < size; ++i)
		tuples[i] = new int[arity]();
}

HTab::HTab(const int id, const int arity, const int size, int ** tps, int * scp)
	: id(id), arity(arity), size(size) {
	scope = new int[arity];
	for (size_t i = 0; i < arity; ++i)
		scope[i] = scp[i];

	tuples = new int*[size];
	for (size_t i = 0; i < size; ++i) {
		tuples[i] = new int[arity]();
		for (size_t j = 0; j < arity; ++j)
			tuples[i][j] = tps[i][j];
	}
}

HTab::~HTab() {
	delete[] scope;
	for (int i = 0; i < size; ++i)
		delete[] tuples[i];
	delete[] tuples;
}



HModel::~HModel() {
	for (size_t i = 0; i < property.vs_size; i++)
		delete vars[i];
	delete[] vars;

	for (size_t i = 0; i < property.ts_size; i++)
		delete tabs[i];
	delete[] tabs;
}

}
