#pragma once

#include <iostream>
#include <string>
#include <time.h>
#include <sstream>
#include <fstream>
#include <windows.h>
#include "model/XBuilder.h"
#include "model/HBuilder.h"
#include "cuda_sac.cuh"
#include "model/Timer.h"
#include "CPUSolver.h"

#undef DOMDocument
#define LOGFILE
using namespace std;
using namespace cudacp;
const string X_PATH = "BMPath.xml";

int main() {

	DWORD t1, t2;
	XBuilder path_builder(X_PATH, XRT_BM_PATH);
	string bm_path = path_builder.GetBMFile();
	const string bmp_root = "E:/Projects/benchmarks/";
	//const string bmp_folder[4] = {
	//	"tightness0.1/rand-2-40-8-753-100-",
	//	"tightness0.2/rand-2-40-11-414-200-",
	//	"tightness0.35/rand-2-40-16-250-350-",
	//	"tightness0.5/rand-2-40-25-180-500-" };
	const string bmp_folder[1] = { "frb30-15/frb30-15-" };
	const string bmp_ext = "_ext.xml";

	for (size_t i = 0; i < 1; i++) {
#ifdef LOGFILE
		ofstream lofi;
		const string bm_res = bmp_root + "res/" + bmp_folder[i].substr(0, bmp_folder[0].find("/")) + "bi.txt";
		lofi.open(bm_res, ios::out | ios::app);
		cout << bm_res << endl;
		if (!lofi.is_open())
			return 0;
#endif
		for (size_t j = 1; j < 6; j++) {
			const string num = tostr<int>(j);
			const string bm_path = bmp_root + bmp_folder[i] + num + bmp_ext;
			cout << bm_path << endl;

#ifdef LOGFILE
			lofi << bm_path << endl;
#endif
			XBuilder builder(bm_path, XRT_BM);
			XModel* xmodel = new XModel();
			builder.GenerateModelFromXml(xmodel);
			HModel* hmodel = new HModel();
			HBuilder hbuilder(xmodel, hmodel);
			float build_time = BuidBitModel32bit(hmodel);
			float exe_time = SACGPU();
			float back_time = CopyBitSubDom();

			CPUSolver s(hmodel, h_bitDom, h_bitSubDom);
			SearchStatistics statistics;
			t1 = GetTickCount();
			statistics = s.MAC();
			t2 = GetTickCount();
			const string  slv_str = (statistics.num_sol > 0) ? "SAT!!" : "UNSAT";
			DelGPUModel();
			delete hmodel;
			delete xmodel;

			cout << "----------------modeling--------------------" << endl;
			cout << "Build time = " << build_time << endl;
			cout << "Execution time = " << exe_time << endl;
			cout << "Copy back time = " << back_time << endl;
			cout << "----------------solving--------------------" << endl;
			cout << slv_str << "|| Solve time = " << (t2 - t1) << "|| nodes = " << statistics.nodes << endl;
			cout << "----------------sleeping--------------------" << endl;
#ifdef LOGFILE
			lofi << "----------------modeling--------------------" << endl;
			lofi << "Build time = " << build_time << endl;
			lofi << "Execution time = " << exe_time << endl;
			lofi << "Copy back time = " << back_time << endl;
			lofi << "----------------solving--------------------" << endl;
			lofi << slv_str << "|| Solve time = " << (t2 - t1) << "|| nodes = " << statistics.nodes << endl;
			lofi << "----------------sleeping--------------------" << endl;
#endif
			Sleep(1000);
		}
#ifdef LOGFILE
		lofi.close();
	}
#endif
	//}
	//}
	return 0;
}

