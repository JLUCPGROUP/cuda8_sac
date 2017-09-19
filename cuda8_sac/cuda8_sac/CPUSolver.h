//#pragma once
//#include "cuda_sac.cuh"
//#include "model/HModel.h"
//#include <vector>
//#include <iostream>
//#include <tuple>
//#include <stack>
//#include <deque>
//
//using namespace std;
//
//namespace cudacp {
//
//enum SolutionNum {
//	SN_ONE,
//	SN_ALL
//};
//
//enum NodeType {
//	NT_ROOT,
//	NT_LEVEL1,
//	NT_DEADEND,
//	NT_LEAF,
//	NT_OUTLASTBRONODE,
//	NT_EXPLORE
//};
//
//enum SearchState {
//	S_SOLVED,
//	S_FAILED,
//	S_BRANCH,
//	S_BACKTRACK
//};
//
//enum AssignedOperator {
//	AO_EXCLUDE = 0,
//	AO_ASSIGN = 1
//};
//
//class IntVal {
//public:
//	int v;
//	int a;
//	AssignedOperator aop = AO_ASSIGN;
//	IntVal() {};
//	IntVal(const int v, const int a, const AssignedOperator aop = AO_ASSIGN) :v(v), a(a), aop(aop) {};
//	IntVal(const uint2 v_a, const AssignedOperator aop = AO_ASSIGN) :v(v_a.x), a(v_a.y), aop(aop) {};
//
//	const IntVal& operator=(const IntVal& rhs) {
//		v = rhs.v;
//		a = rhs.a;
//		aop = rhs.aop;
//		return *this;
//	};
//
//	inline bool operator==(const IntVal& rhs) {
//		return (this == &rhs) || (v == rhs.v && a == rhs.a && aop == rhs.aop);
//	}
//
//	inline bool operator!=(const IntVal& rhs) {
//		return !((this == &rhs) || (v == rhs.v && a == rhs.a && aop == rhs.aop));
//	}
//
//	//IntVal& next() {
//	//	return IntVal(v, a + 1);
//	//}
//	//IntVal& firstChild() {
//	//	return IntVal(v + 1, 0);
//	//}
//
//	~IntVal() {};
//
//	friend std::ostream& operator<< (std::ostream &os, IntVal &v_val) {
//		os << "(" << v_val.v << (aop ? " = " : " != ") << v_val.a << ")";
//		return os;
//	};
//};
//
//namespace SearchNode {
//const IntVal RootNode = IntVal(-1, 0);
//const IntVal Deadend = IntVal(-2, -1);
//const IntVal NullNode = IntVal(-2, -2);
//const IntVal OutLastBroNode = IntVal(-2, -3);
//const IntVal OutLastNode = IntVal(-2, -4);
//};
//
//class AssignedStack {
//public:
//	AssignedStack() {};
//
//	void initial(HModel *m) {
//		m_ = m;
//		vals_.resize(m_->property.vs_size);
//	};
//
//	virtual ~AssignedStack() {};
//
//	void push(IntVal& v_a) {
//		vals_[top_] = v_a;
//		assigned[top_] = true;
//		++top_;
//	};
//
//	IntVal pop() {
//		--top_;
//		assigned[top_] = true;
//		return vals_[top_];
//	};
//
//	IntVal top() const {
//		return vals_[top_];
//	};
//
//	int size() const { return top_; }
//	int capacity() const { return size_; }
//	bool full() const { return top_ == size_; }
//	bool empty() const { return top_ == 0; }
//	IntVal operator[](const int i) const { return vals_[i]; };
//	IntVal at(const int i) const { return vals_[i]; };
//
//	friend std::ostream& operator<< (std::ostream &os, AssignedStack &I) {
//		for (int i = 0; i < I.size(); ++i)
//			os << "(" << I[i] << ")\t";
//		return os;
//	};
//
//	friend std::ostream& operator<< (std::ostream &os, AssignedStack* I) {
//		for (int i = 0; i < I->size(); ++i)
//			os << "(" << I->at(i) << ")\t";
//		return os;
//	};
//
//private:
//	HModel *m_;
//	std::vector<IntVal> vals_;
//	std::vector<bool> assigned;
//	int* a_;
//	int top_ = 0;
//	int size_;
//};
//
////class AssignedCache {
////public:
////	AssignedCache();
////	~AssignedCache();
////
////private:
////	std::tuple<IntVal, bool>();
////	IntVal val;
////	int MAXDEEP;
////	bool
////
////};
//
////class NonBinary_Stack {
////public:
////	NonBinary_Stack() {};
////
////	void initial(HModel* m) {
////		m_ = m;
////		vs_size_ = m->property.vs_size;
////		mds_ = m->property.max_dom_size;
////		b_.resize(vs_size_, UINT32_MAX);
////		r_.resize(vs_size_, 0);
////		s_.resize((vs_size_ + 2), std::vector<u32>(vs_size_, UINT_MAX));
////	}
////
////	//考虑一次实例化后赋两个值的巧合
////	SearchState push(std::vector<u32>& a) {
////		//++top_;
////		//所有变量都要做一次按位运算
////		for (size_t i = 0; i < vs_size_; i++) {
////			s_[top_ + 1][i] = s_[top_][i] & a[i];
////			//有变量域变空
////			if (s_[top_ + 1][i] == 0)
////				return S_FAILED;
////		}
////
////		++top_;
////		//所有变量域均不为空
////		if (top_ == vs_size_)
////			return S_SOLVED;
////		else
////			return S_BRANCH;
////	};
////
////	//考虑一次实例化后赋两个值的巧合
////	SearchState push(IntVal& val) {
////		//若当前网络已经删除将要赋值的val
////		if (s_[top_][val.v] & bsd_[val.v][val.a][val.v])
////			return S_FAILED;
////
////		//所有变量都要做一次按位运算
////		for (size_t i = 0; i < vs_size_; i++) {
////			s_[top_ + 1][i] = s_[top_][i] & bsd_[val.v][val.a][i];
////			//有变量域变空
////			if (s_[top_ + 1][i] == 0)
////				return S_FAILED;
////		}
////
////		//AC成功
////		++top_;
////		//所有变量域均不为空
////		if (top_ == vs_size_)
////			return S_SOLVED;
////		else
////			return S_BRANCH;
////	};
////
////	std::vector<u32>& top() {
////		return s_[top_];
////	};
////
////	~NonBinary_Stack() {};
////private:
////	//top的最大值是vs_size+1;
////	int top_ = 0;
////	std::vector<std::vector<u32>> s_;
////	HModel* m_;
////	int vs_size_;
////	int mds_;
////	std::vector<u32> b_;
////	std::vector<u32> r_;
////	std::vector<std::vector<std::vector<u32>>> bsd_;
////};
////
//
//
//////搜索8层记录一次
////class Binary_Stack {
////public:
////	Binary_Stack() {};
////
////	void initial(HModel* m) {
////		m_ = m;
////		vs_size_ = m->property.vs_size;
////		mds_ = m->property.max_dom_size;
////		b_.resize(vs_size_, UINT32_MAX);
////		r_.resize(vs_size_, 0);
////		s_.resize((vs_size_ + 2), std::vector<u32>(vs_size_, UINT_MAX));
////		assigned_cache_.resize(MAXCACHESIZE);
////	}
////
////	//考虑一次实例化后赋两个值的巧合
////	SearchState push(std::vector<u32>& a) {
////		//++top_;
////		//所有变量都要做一次按位运算
////		for (size_t i = 0; i < vs_size_; i++) {
////			s_[top_ + 1][i] = s_[top_][i] & a[i];
////			//有变量域变空
////			if (s_[top_ + 1][i] == 0)
////				return S_FAILED;
////		}
////
////		++top_;
////		//所有变量域均不为空
////		if (top_ == vs_size_)
////			return S_SOLVED;
////		else
////			return S_BRANCH;
////	};
////
////	//考虑一次实例化后赋两个值的巧合
////	SearchState push(IntVal& val) {
////		//若当前网络已经删除将要赋值的val
////		if (s_[top_][val.v] & bsd_[val.v][val.a][val.v])
////			return S_FAILED;
////
////		//所有变量都要做一次按位运算
////		for (size_t i = 0; i < vs_size_; i++) {
////			s_[top_ + 1][i] = s_[top_][i] & bsd_[val.v][val.a][i];
////			//有变量域变空
////			if (s_[top_ + 1][i] == 0)
////				return S_FAILED;
////		}
////
////		//AC成功
////		++top_;
////		//所有变量域均不为空
////		if (top_ == vs_size_)
////			return S_SOLVED;
////		else
////			return S_BRANCH;
////	};
////
////	SearchState MakeRVD(IntVal& val, std::vector<std::vector<std::vector<u32>>> bsd) {
////		r_.assign(vs_size_, 0);
////		//先删该点val
////		s_[top_][val.v] &= U32_MASK0[val.a];
////		//遍历该变量中所的有值
////		for (size_t i = 0; i < m_->vars[i]->size; ++i)
////			//若变量值已不存在，跳过
////			if (s_[top_][val.v] & U32_MASK1[i])
////				//生成subDom(x != a)
////				for (size_t j = 0; j < vs_size_; ++j)
////					r_[j] |= bsd[val.v][val.a][j];
////
////		return push(r_);
////	};
////
////	std::vector<u32>& top() {
////		return s_[top_];
////	};
////
////	~Binary_Stack() {};
////	void cache_push(Int) {
////	};
////private:
////	//top的最大值是vs_size+1;
////	int delt_top_ = 0;
////	int top_ = 0;
////	//主网络栈8层一个记录
////	std::vector<std::vector<u32>> s_;
////	std::vector<std::tuple<IntVal, bool>> assigned_cache_;
////	//缓冲网络栈只记录最新的8层
////	std::vector<std::vector<std::vector<u32>>> cache_;
////	HModel* m_;
////	int vs_size_;
////	int mds_;
////	std::vector<u32> b_;
////	std::vector<u32> r_;
////	std::vector<std::vector<std::vector<u32>>> bsd_;
////};
//
//enum SearchMethod {
//	SM_DFS,
//	SM_BAB,
//	SM_BIDFS
//};
//
//////struct {
////	int time;
////	int nodes;
////};
//
////class Solver {
////public:
////	std::vector<std::vector<std::vector<u32>>> bitSubDom;
////	std::vector<IntVal> I;
////	BD_Stack bds;
////	HModel* model;
////	int level = -1;
////	int num_solutions = 0;
////	int num_nodes = 0;
////
////	Solver(HModel* m) :
////		model(m) {
////		const int vs_size = model->property.vs_size;
////		const int mds = model->property.max_dom_size;
////		bitSubDom.resize(vs_size, std::vector<std::vector<u32>>(mds, std::vector<u32>(vs_size, 0)));
////		//拷贝数据结构
////		for (size_t i = 0; i < vs_size; i++) {
////			for (size_t j = 0; j < mds; j++) {
////				for (size_t k = 0; k < vs_size; k++) {
////					const int idx = GetBitSubDomIndexHost(i, j, k);
////					bitSubDom[i][j][k] = h_bitSubDom[idx];
////				}
////			}
////		}
////		rvd.resize(vs_size, 0);
////		bds.initial(m);
////		I.reserve(vs_size + 1);
////	}
////
////	bool Solve(SearchMethod sm, SolutionNum sn) {
////
////		bool solved;
////
////		switch (sm) {
////		case cudacp::SM_DFS:
////			solved = preOrderSearch(sn);
////			break;
////		case cudacp::SM_BAB:
////			break;
////		case cudacp::SM_BIDFS:
////
////			break;
////		default:
////			break;
////		}
////
////		if (solved)
////			return true;
////		else
////			return false;
////	}
////
////	~Solver() {
////	}
////
////private:
////	std::vector<u32> rvd;
////	bool hasChild(IntVal& v_a) {
////		if ((v_a.v + 1) < model->property.vs_size)
////			return true;
////		else
////			return false;
////	}
////
////	bool hasNext(IntVal& v_a) {
////		if ((v_a.a + 1) < model->vars[v_a.v]->size)
////			return true;
////		else
////			return false;
////	}
////
////	IntVal getChild(IntVal& v_a) {
////		if (hasChild(v_a))
////			return v_a.firstChild();
////		else
////			return SearchNode::OutLastBroNode;
////	}
////
////	IntVal getNext(IntVal& v_a) {
////		if (hasNext(v_a))
////			return v_a.next();
////		else return
////			SearchNode::OutLastBroNode;
////	}
////
////	bool preOrderSearch(SolutionNum sn) {
////		SearchState state = S_BRANCH;
////		IntVal val;
////		IntVal pre = SearchNode::RootNode;
////		bool finished = false;
////		do {
////			switch (state) {
////			case cudacp::S_SOLVED:
////				finished = true;
////				break;
////			case cudacp::S_FAILED:
////				pre = I.back();
////				I.pop_back();
////
////				break;
////			case cudacp::S_BRANCH:
////				val = selectValue(pre);
////				I.push_back(val);
////				state = bds.push(bitSubDom[val.v][val.a]);
////				pre = val;
////				++num_nodes;
////				break;
////			case cudacp::S_BACKTRACK:
////				break;
////			default:
////				break;
////			}
////		} while (!finished);
////		//IntVal curr = SearchNode::RootNode;
////		//SearchState state = S_BRANCH;
////		//I.push_back(curr);
////		//curr = selectChildValue(curr);
////
////		////root结点不能弹出， 赋值栈不能弹空 长度最小为1
////		//while (!I.empty()) {
////		//	std::cout << I.size() << std::endl;
////		//	switch (state) {
////		//	case cudacp::S_SOLVED:
////		//		if (sn == SN_ONE) {
////		//			I.push_back(curr);
////		//			printAssigned();
////		//			return true;
////		//		}
////		//		else {
////		//			I.push_back(curr);
////		//			++num_solutions;
////		//			printAssigned();
////		//			state = S_BACKTRACK;
////		//		}
////		//		break;
////		//	case cudacp::S_FAILED:
////		//		curr = selectNextValue(curr);
////		//		state = curr != SearchNode::OutLastBroNode ? S_BRANCH : S_BACKTRACK;
////		//		break;
////		//	case cudacp::S_BRANCH:
////		//		state = bds.push(bitSubDom[curr.v][curr.a]);
////		//		++num_nodes;
////		//		if (state == S_BRANCH) {
////		//			I.push_back(curr);
////		//			curr = selectChildValue(curr);
////		//		}
////		//		break;
////		//	case cudacp::S_BACKTRACK:
////		//		curr = I.back();
////		//		I.pop_back();
////		//		if (curr != SearchNode::RootNode) {
////		//			bds.pop();
////		//			curr = selectNextValue(curr);
////		//			state = curr != SearchNode::OutLastBroNode ? S_BRANCH : S_BACKTRACK;
////		//		}
////		//		else {
////		//			return true;
////		//		}
////		//		break;
////		//	default:
////		//		break;
////		//	}
////		//}
////		//return false;
////
////		//while (state == S_BRANCH) {
////		//IntVal curr = SearchNode::RootNode;
////		//SearchState state = S_BRANCH;
////		//I.push_back(curr);
////		//curr = selectChildValue(curr);
////
////		//while (state == S_BRANCH) {
////		//	state = bds.push(bitSubDom[curr.v][curr.a]);
////		//	++num_nodes;
////		//	if (state == S_BRANCH) {
////		//		I.push_back(curr);
////		//		curr = selectChildValue(curr);
////		//	}
////		//}
////
////		//if (state == S_FAILED) {
////		//	curr = selectNextValue(curr);
////		//	state = curr != SearchNode::OutLastBroNode ? S_BRANCH : S_BACKTRACK;
////		//}
////		//else if (state == S_BACKTRACK) {
////		//	curr = I.back();
////		//	I.pop_back();
////		//	if (curr != SearchNode::RootNode) {
////		//		bds.pop();
////		//		curr = selectNextValue(curr);
////		//		state = curr != SearchNode::OutLastBroNode ? S_BRANCH : S_BACKTRACK;
////		//	}
////		//}
////		//else if (state == S_SOLVED) {
////		//	if (sn == SN_ONE) {
////		//		I.push_back(curr);
////		//		printAssigned();
////		//		return true;
////		//	}
////		//	else {
////		//		I.push_back(curr);
////		//		printAssigned();
////		//		state = S_BACKTRACK;
////		//	}
////		//}
////	}
////
////	bool binarySearch(SolutionNum sn) {
////		bool consistent = false;
////		bool finished = false;
////		IntVal curr;
////		SearchState state;
////
////		while (!finished) {
////			curr = selectChildValue(curr);
////			I.push_back(curr);
////			state = bds.push(bitSubDom[curr.v][curr.a]);
////			consistent = (state == S_BRANCH) ? true : false;
////			if (consistent && (I.size() == model->property.vs_size)) {
////				finished = true;
////			}
////
////			while (!consistent && !I.empty()) {
////
////			}
////		}
////
////		//root结点不能弹出， 赋值栈不能弹空 长度最小为1
////		while (!I.empty()) {
////			switch (state) {
////			case cudacp::S_SOLVED:
////				if (sn == SN_ONE) {
////					I.push_back(curr);
////					//printAssigned();
////					return true;
////				}
////				else {
////					I.push_back(curr);
////					++num_solutions;
////					//printAssigned();
////					state = S_BACKTRACK;
////				}
////				break;
////			case cudacp::S_FAILED:
////				curr = selectNextValue(curr);
////				state = curr != SearchNode::OutLastBroNode ? S_BRANCH : S_BACKTRACK;
////				break;
////			case cudacp::S_BRANCH:
////				state = bds.push(bitSubDom[curr.v][curr.a]);
////				++num_nodes;
////				if (state == S_BRANCH) {
////					I.push_back(curr);
////					curr = selectChildValue(curr);
////				}
////				break;
////			case cudacp::S_BACKTRACK:
////				curr = I.back();
////				I.pop_back();
////				if (curr != SearchNode::RootNode) {
////					bds.pop();
////					curr = selectNextValue(curr);
////					state = curr != SearchNode::OutLastBroNode ? S_BRANCH : S_BACKTRACK;
////				}
////				else
////					return true;
////				break;
////			default:
////				break;
////			}
////		}
////		return false;
////	}
////
////
////	void printAssigned() {
////		for (auto v_a : I)
////			std::cout << v_a;
////		std::cout << std::endl;
////	}
////
////	NodeType checkNodeType(IntVal& v_a) {
////		IntVal back = I.back();
////		if (back == SearchNode::RootNode)
////			return NT_LEVEL1;
////		if (v_a == SearchNode::OutLastBroNode)
////			return NT_OUTLASTBRONODE;
////		else if (bitSubDom[back.v][back.a][v_a.v] & bitSubDom[v_a.v][v_a.a][v_a.v] == 0)
////			return NT_DEADEND;
////		return NT_EXPLORE;
////	}
////
////	//新的赋值与现网络相容
////	IntVal selectChildValue(IntVal& v_a) {
////		IntVal val = getChild(v_a);
////		while (checkNodeType(val) == NT_DEADEND)
////			val = getNext(val);
////		return val;
////	}
////
////	IntVal selectNextValue(IntVal& v_a) {
////		IntVal val = getNext(v_a);
////		NodeType type = checkNodeType(val);
////
////		do {
////			switch (type) {
////			case NT_LEVEL1:
////				type = NT_EXPLORE;
////				break;
////			case NT_DEADEND:
////				val = getNext(val);
////				type = checkNodeType(val);
////				break;
////			case NT_OUTLASTBRONODE:
////				return SearchNode::OutLastBroNode;
////				break;
////			case NT_EXPLORE:
////				break;
////			default:
////				break;
////			}
////		} while (type != NT_EXPLORE);
////		return val;
////	};
////
////	void removeValue(IntVal& v_a) {
////		rvd.assign(model->property.vs_size, 0);
////		bds.top()[v_a.v] &= U32_MASK0[v_a.a];
////		for (size_t i = 0; i < model->vars[v_a.v]->size; ++i)
////			if (bds.top()[v_a.v] & U32_MASK1[i])
////				for (size_t j = 0; j < model->property.vs_size; ++j)
////					rvd[j] |= bitSubDom[v_a.v][v_a.a][j];
////	};
////};
//class CPUSolver {
//public:
//	CPUSolver();
//	virtual ~CPUSolver();
//};
//}
//
