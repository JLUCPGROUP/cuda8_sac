#include "HBuilder.h"

namespace cudacp {
void HDomMap::MakeMap(XDom * d) {
	id = d->id;
	for (int i = 0; i < d->size; ++i) {
		m[d->values[i]] = i;
		if (d->values[i] != i)
			type = DT_DP;
	}
}

HBuilder::HBuilder(XModel * xmodel, HModel * hmodel)
	:xm(xmodel), hm(hmodel) {
	domms.resize(xm->feature.ds_size);

	hm->property.vs_size = xm->feature.vs_size;
	hm->property.ts_size = xm->feature.cs_size;
	hm->property.max_dom_size = xm->feature.max_dom_size;
	hm->vars = new HVar*[hm->property.vs_size];
	hm->tabs = new HTab*[hm->property.ts_size];

	generateHDomMap();
	generateHVars();
	generateHTab();
}

void HBuilder::ShowHModel() {
	//显示变量
	printf("Show variables:\n");
	for (size_t i = 0; i < hm->property.vs_size; i++) {
		const HVar* v = hm->vars[i];
		printf("variable id = %d: ", i);
		for (size_t j = 0; j < v->size; j++) {
			printf("%d ", v->vals[j]);
		}
		printf("\n");
	}

	printf("Show constraints:\n");
	for (size_t i = 0; i < hm->property.ts_size; i++) {
		const HTab* t = hm->tabs[i];
		printf("table_id = %d, arity = %d, size = %d, scope = { ", t->id, t->arity, t->size);
		for (size_t j = 0; j < t->arity; j++) {
			printf("%d ", t->scope[j]);
		}
		printf("}: ");

		for (size_t i = 0; i < t->size; i++) {
			for (size_t j = 0; j < t->arity; j++) {
				printf("%d ", t->tuples[i][j]);
			}
			printf("|");
		}
		printf("\n");
	}
}

void HBuilder::assign(int * lh, const int * rh, const int len) {
	for (size_t i = 0; i < len; i++)
		lh[i] = rh[i];
}

void HBuilder::getIntTuple(int idx, int * t, const XCon * c) {
	for (int i = (c->arity - 1); i >= 0; --i) {
		XDom* d = xm->doms[xm->vars[c->scope[i]]->dom_id];
		t[i] = d->values[(idx % (d->size))];
		idx /= (d->size);
	}
}

void HBuilder::getIntTuple(int idx, std::vector<int>& t, const XCon * c) {
	for (int i = (c->arity - 1); i >= 0; --i) {
		XDom* d = xm->doms[xm->vars[c->scope[i]]->dom_id];
		t[i] = d->values[(idx % (d->size))];
		idx /= (d->size);
	}
}

bool HBuilder::isEqual(const int * lh, const int * rh, const int len) {
	for (int i = 0; i < len; ++i)
		if (lh[i] != rh[i])
			return false;

	return true;
}

void HBuilder::generateHDomMap() {
	for (size_t i = 0; i < xm->feature.ds_size; ++i)
		domms[i].MakeMap(xm->doms[i]);
}

void HBuilder::generateHVars() {
	for (size_t i = 0; i < xm->feature.vs_size; ++i)
		hm->vars[i] = new HVar(i, xm->doms[xm->vars[i]->dom_id]->size);
}

void HBuilder::generateHTab() {
	//若为约束EXT
	for (int i = 0; i < xm->feature.cs_size; ++i) {
		XCon* c = xm->cons[i];

		if (c->type == EXT)
			EXTConToTab(c);
		else
			INTConToTab(c);
	}
}

void HBuilder::EXTConToTab(XCon * c) {
	const XRel* r = xm->rels[c->rel_id];
	const int i = c->id;
	//std::map<int, DomType> mtype;
	std::vector<DomType> type(c->arity);
	bool IsAllStd = true;

	//中间变量元组
	for (size_t j = 0; j < c->arity; ++j) {
		const XVar* v = xm->vars[c->scope[j]];
		type[j] = domms[v->dom_id].type;
		IsAllStd &= domms[v->dom_id].type;
	}
	//正表
	if (r->sem == SEM_SUPPORT) {
		hm->tabs[i] = new HTab(i, c->arity, r->size, r->tuples, c->scope);
	}
	//负表
	else {

		int* tpl = new int[c->arity];
		//获取总个数
		int num_all_tpls = 1;
		for (size_t j = 0; j < c->arity; j++) {
			const XDom *d = xm->doms[xm->vars[c->scope[j]]->dom_id];
			num_all_tpls *= d->size;
		}
		//正表元组个数 = 总元组-负元组
		int num_sup_tpls = num_all_tpls - r->size;

		//生成临时元组集合
		int** ts = new int*[num_sup_tpls];
		for (int j = 0; j < num_sup_tpls; ++j)
			ts[j] = new int[c->arity];

		//通过index获取tuple，一维索引转多维元组
		int k = 0, m = 0;
		for (int j = 0; j < num_all_tpls; ++j) {
			getIntTuple(j, tpl, c);
			//支持
			if (m < r->size) {
				if (!isEqual(r->tuples[m], tpl, c->arity)) {
					assign(ts[k], tpl, c->arity);
					++k;
				}
				else
					++m;
			}
			else {
				assign(ts[k], tpl, c->arity);
				++k;
			}
		}

		hm->tabs[i] = new HTab(i, c->arity, num_sup_tpls, ts, c->scope);

		for (int j = 0; j < num_sup_tpls; ++j)
			delete[] ts[j];
		delete[] ts;

		delete[] tpl;
	}

	//非标准论域转标准论域
	if (IsAllStd == false)
		modifyTuple(hm->tabs[i], type);
}

void HBuilder::INTConToTab(XCon * c) {
	XINTCon *xic = static_cast<XINTCon*>(c);
	const XPre* p = xm->pres[xic->rel_id];
	std::vector<std::vector<int>> ts;
	std::vector<DomType> type(xic->arity);
	bool IsAllStd = true;

	//中间变量元组
	for (size_t j = 0; j < xic->arity; ++j) {
		const XVar* v = xm->vars[xic->scope[j]];
		type[j] = domms[v->dom_id].type;
		IsAllStd &= domms[v->dom_id].type;
	}

	int ps[20];
	//std::vector<int> tp(xic->arity);
	//int* tpl = new int[c->arity];
	std::vector<int> tpl(c->arity);

	int num_all_tpls = 1;
	for (size_t j = 0; j < xic->arity; j++) {
		const XDom *d = xm->doms[xm->vars[c->scope[j]]->dom_id];
		num_all_tpls *= d->size;
	}

	const int num_pars = xic->num_paras;
	for (size_t i = 0; i < num_all_tpls; i++) {
		getIntTuple(i, tpl, c);

		int par;
		for (size_t j = 0; j < xic->num_paras; j++) {
			int par_const = xic->paras[j];
			if (getPara(par_const, &par)) {
				for (size_t k = 0; k < xic->arity; k++)
					if (par == xic->scope[k])
						ps[j] = tpl[k];
			}
			else {
				ps[j] = par_const;
			}
		}

		const int result = calulate(&(p->prop_stack), ps);

		if (result == 1)
			ts.push_back(tpl);
	}
	hm->tabs[xic->id] = new HTab(xic->id, xic->arity, ts.size(), xic->scope);
	makeHTab(hm->tabs[xic->id], ts);

	//非标准论域转标准论域
	if (IsAllStd == false)
		modifyTuple(hm->tabs[xic->id], type);
}

void HBuilder::modifyTuple(HTab * t, std::vector<DomType>& type) {
	for (size_t i = 0; i < t->size; ++i) {
		for (size_t j = 0; j < t->arity; ++j) {
			const DomType tp = type[j];
			//若为非标准论域则标准化
			if (tp == DT_DP) {
				const XVar* v = xm->vars[t->scope[j]];
				const int s = t->tuples[i][j];
				const int res = domms[v->dom_id].m[s];
				t->tuples[i][j] = res;
			}
		}
	}
}

void HBuilder::makeHTab(HTab * t, std::vector<std::vector<int>>& tuples) {
	for (size_t i = 0; i < t->size; i++)
		for (size_t j = 0; j < t->arity; j++)
			t->tuples[i][j] = tuples[i][j];
}



bool HBuilder::getPara(int par_in, int * par) {
	bool IsVarId;

	if ((par_in - MAX_VALUE) >= 0) {
		*par = par_in - MAX_VALUE;
		IsVarId = true;
	}
	else {
		*par = par_in;
		IsVarId = false;
	}

	return IsVarId;
}

int HBuilder::calulate(const PStack * pss, int * params) {
	int res_stack[10];
	int res_top = -1;
	int op;
	int param;

	for (u32 i = 0; i < pss->num_prs; ++i) {
		op = pss->ps[i];

		if (op > MAX_OPT) {
			param = params[op];
			res_stack[++res_top] = param;
		}
		else 
			getResult((PredicateOperator)op, res_stack, &res_top);
	}

	return res_stack[0];
}

int HBuilder::getResult(PredicateOperator po, int * x, int * pars_top) {
	int result = -1;
	if (po == PO_ABS)
		result = ops[po](x + *pars_top);
	else
		result = ops[po](x + (--(*pars_top)));

	x[*pars_top] = result;
	return result;
}
}



