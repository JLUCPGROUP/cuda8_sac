#pragma once

#include <iostream>
#include <string>
#include <time.h>
#include <windows.h>
#include "model/XBuilder.h"
#include "model/HBuilder.h"
#include "cuda_sac.cuh"
#include "model/Timer.h"

#undef DOMDocument

using namespace std;
using namespace cudacp;
const string X_PATH = "BMPath.xml";

int main() {

	DWORD t1, t2;
	clock_t  begin, end;
	time_t start, end2;
	XBuilder path_builder(X_PATH, XRT_BM_PATH);
	string bm_path = path_builder.GetBMFile();
	cout << bm_path << endl;
	XBuilder builder(bm_path, XRT_BM);
	XModel* xmodel = new XModel();
	builder.GenerateModelFromXml(xmodel);
	HModel* hmodel = new HModel();
	HBuilder hbuilder(xmodel, hmodel);
	//hbuilder.ShowHModel();
	std::cout << "----------------modeling--------------------" << std::endl;
	float build_time = BuidBitModel32bit(hmodel);
	printf("Build time = %f\n", build_time);
	float exe_time = SACGPU();
	printf("Execution time = %f\n", exe_time);
	float back_time = CopyBitSubDom();
	printf("Copy back time = %f\n", back_time);
	std::cout << "----------------modeling--------------------" << std::endl;
	Solver s(hmodel);
	//Timer t;
	//begin = clock();
	std::cout << "----------------solving---------------------" << std::endl;
	t1 = GetTickCount();
	float sol_time = s.Solve(SM_DFS, SN_ONE);
	t2 = GetTickCount();
	//end = clock();
	std::cout << "----------------solving---------------------" << std::endl;
	//int64_t dur = t.elapsed();
	printf("Solve time = %f\n", (t2 - t1)*1.0);
	printf("nodes = %d\n", s.num_nodes);
	printf("|solutions| = %d\n", s.num_solutions);
	DelGPUModel();
	delete hmodel;
	delete xmodel;
	printf("---end---\n");
	return 0;
}

