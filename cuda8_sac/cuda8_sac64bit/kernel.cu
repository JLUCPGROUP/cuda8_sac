#pragma once

#include <iostream>
#include <string>
#include "model/XBuilder.h"
#include "model/HBuilder.h"
#include "cuda_sac.cuh"
#undef DOMDocument

using namespace std;
using namespace cudacp;
const string X_PATH = "BMPath.xml";

int main() {
	XBuilder path_builder(X_PATH, XRT_BM_PATH);
	string bm_path = path_builder.GetBMFile();
	cout << bm_path << endl;
	XBuilder builder(bm_path, XRT_BM);
	XModel* xmodel = new XModel();
	builder.GenerateModelFromXml(xmodel);
	HModel* hmodel = new HModel();
	HBuilder hbuilder(xmodel, hmodel);
	//hbuilder.ShowHModel();
	BuidBitModel64bit(hmodel);
	float exe_time = SACGPU();
	DelGPUModel();
	delete hmodel;
	delete xmodel;
	cout << "Execution time = " << exe_time << endl;
	printf("---end---\n");
	return 0;
}

