#include "model/HBuilder.h"
#include <deque>
using namespace std;
namespace cudacp {
class IntVal;
enum SolutionNum;
enum SearchMethod;

const u32 MASK1[32] = {
	0x80000000, 0x40000000, 0x20000000, 0x10000000,
	0x08000000, 0x04000000, 0x02000000, 0x01000000,
	0x00800000, 0x00400000, 0x00200000, 0x00100000,
	0x00080000, 0x00040000, 0x00020000, 0x00010000,
	0x00008000, 0x00004000, 0x00002000, 0x00001000,
	0x00000800, 0x00000400, 0x00000200, 0x00000100,
	0x00000080, 0x00000040, 0x00000020, 0x00000010,
	0x00000008, 0x00000004, 0x00000002, 0x00000001
};

const u32 MASK0[32] = {
	0x7FFFFFFF, 0xBFFFFFFF, 0xDFFFFFFF, 0xEFFFFFFF,
	0xF7FFFFFF, 0xFBFFFFFF, 0xFDFFFFFF, 0xFEFFFFFF,
	0xFF7FFFFF, 0xFFBFFFFF, 0xFFDFFFFF, 0xFFEFFFFF,
	0xFFF7FFFF, 0xFFFBFFFF, 0xFFFDFFFF, 0xFFFEFFFF,
	0xFFFF7FFF, 0xFFFFBFFF, 0xFFFFDFFF, 0xFFFFEFFF,
	0xFFFFF7FF, 0xFFFFFBFF, 0xFFFFFDFF, 0xFFFFFEFF,
	0xFFFFFF7F, 0xFFFFFFBF, 0xFFFFFFDF, 0xFFFFFFEF,
	0xFFFFFFF7, 0xFFFFFFFB, 0xFFFFFFFD, 0xFFFFFFFE
};

namespace NodeType {
const IntVal ROOTNODE = IntVal(-1, 0, true);
};

//解数量
enum SolutionNum {
	SN_ONE,
	SN_ALL
};

//搜索方案
enum SearchMethod {
	SM_MAC,
	SM_DFS,
	SM_BAB,
	SM_BIDFS
};

//搜索状态
enum SearchState {
	S_BRANCH
	, S_FAILED
	, S_SOLVED
	//,	S_BACKTRACK
};

//变量启发式
enum VarHeuristic {
	VRH_LEX,
	VRH_DOM,
	VRH_VWDEG
};

//值启发式
enum ValHeuristic {
	VLH_MIN,
	VLH_VWDEG
};

//统计搜索状态
struct SearchStatistics {
	int num_sol = 0;
	int nodes = 0;
	int times = 0;
	int n_deep = 0;
};

//左起第一个1的位置
inline u32 _32f1(u32 v) {
	static const char msb_256_table[256] =
	{
		0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
		4, 4, 4, 4, 4, 4, 4, 4,4, 4, 4, 4,4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	};
	int result = 0;

	if (v > 0xFFFF) {
		v >>= 16;
		result += 16;
	}
	if (v > 0xFF) {
		v >>= 8;
		result += 8;
	}

	return 31 - (result + msb_256_table[v]);

};

class BitModel {
public:
	BitModel() {};
	void initial(HModel *m, u32* bitDom, u32* bitSubDom);
	virtual ~BitModel() {};
	vector<vector<vector<u32>>> bsd;
	vector<u32> bd;
	int mds;
	int vs_size;
private:
	//HModel *m_;

	inline int getBSDIdx(const int x, const int a, const int y) const;
};

//变量值取值或删值
class IntVal {
public:
	int v;
	int a;
	bool aop = true;
	IntVal() {};
	IntVal(const int v, const int a, const bool aop = true) :v(v), a(a), aop(aop) {};
	//IntVal(const uint2 v_a, const bool aop = true) :v(v_a.x), a(v_a.y), aop(aop) {};

	const IntVal& operator=(const IntVal& rhs) {
		v = rhs.v;
		a = rhs.a;
		aop = rhs.aop;
		return *this;
	};

	//翻转赋值符号
	void flop() {
		aop = !aop;
	};

	inline bool operator==(const IntVal& rhs) {
		return (this == &rhs) || (v == rhs.v && a == rhs.a && aop == rhs.aop);
	};

	inline bool operator!=(const IntVal& rhs) {
		return !((this == &rhs) || (v == rhs.v && a == rhs.a && aop == rhs.aop));
	};

	friend std::ostream& operator<< (std::ostream &os, IntVal &v_val);
	//{
	//	const std::string s = (v_val.aop == AO_ASSIGN) ? " = " : " != ";
	//	os << "(" << v_val.v << s << v_val.a << ")";
	//	return os;
	//};

	~IntVal() {};
};

//赋值队列
class AssignedStack {
public:
	AssignedStack() {};

	void initial(HModel *m);
	~AssignedStack() {};
	void push(IntVal& v_a);
	const IntVal pop();
	IntVal top() const;
	int size() const;
	int capacity() const;
	bool full() const;
	bool empty() const;
	IntVal operator[](const int i) const;
	IntVal at(const int i) const;
	void clear();
	bool assiged(const int v) const;
	friend std::ostream& operator<< (std::ostream &os, AssignedStack &I);
	friend std::ostream& operator<< (std::ostream &os, AssignedStack* I);

protected:
	HModel *m_;
	std::vector<IntVal> vals_;
	std::vector<bool> asnd_;
	int top_ = 0;
	int max_size_;
};

class network {
public:
	network() {};
	void initial(HModel *hm, u32* bitDom, u32* bitSubDom);
	//考虑一次实例化后赋两个值的巧合
	SearchState push_back(const IntVal& val);
	SearchState reduce_dom(const IntVal& val);
	SearchState remove_value(const IntVal& val);
	vector<u32>& nt_back();
	IntVal val_back();
	void pop_back();
	//bool empty();
	int size() const;
	IntVal selectIntVal(const VarHeuristic vrh, const ValHeuristic vlh = VLH_MIN);
	int select_var(const VarHeuristic vrh);
	int select_val(const int var, const ValHeuristic vlh);
	void sychronize(IntVal& val);
	~network() {};
private:
	HModel* m_;
	//bit model
	BitModel bm_;
	//计算删值后网络bitDom的中间变量
	vector<u32> r_;
	//决策栈
	deque<IntVal> D_;
	//赋值栈
	AssignedStack *I_;
	//网络栈
	deque<vector<u32>> s_;
	int vs_size_;
	int mds_;
};

class NetworkStack {
public:
	NetworkStack() {};
	void initial(HModel *hm, u32* bitDom, u32* bitSubDom);
	//考虑一次实例化后赋两个值的巧合
	SearchState push_back(const IntVal& val);
	SearchState reduce_dom(const IntVal& val);
	SearchState remove_value(const IntVal& val);
	vector<u32>& nt_back();
	void pop_back();
	//bool empty();
	int size() const;
	IntVal selectIntVal(const VarHeuristic vrh, const ValHeuristic vlh = VLH_MIN);
	int select_var(const VarHeuristic vrh);
	int select_val(const int var, const ValHeuristic vlh);
	~NetworkStack() {};
private:
	HModel* m_;
	//bit model
	BitModel bm_;
	//计算删值后网络bitDom的中间变量
	vector<u32> r_;
	////决策栈
	//vector<IntVal> D_;
	//赋值栈
	AssignedStack *I_;
	//网络栈
	vector<vector<u32>> s_;
	int vs_size_;
	int mds_;
	int top_ = 0;
};

class CPUSolver {
public:
	AssignedStack I;
	CPUSolver(HModel *m, u32* bitDom, u32* bitSubDom);
	~CPUSolver() {};

	SearchStatistics MAC();

private:
	HModel* m_;
	network n_;
};
}