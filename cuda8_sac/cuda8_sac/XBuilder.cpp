#include "XBuilder.h"

namespace cudacp {

XBuilder::XBuilder(std::string file_name, XmlReaderType type)
	: file_name_(file_name), type_(type) {
	if (initial()) {
		switch (type_) {
		case XRT_BM_PATH:
			break;
		case XRT_BM:
			break;
		default:
			break;
		}
	}
}

void XBuilder::GenerateModelFromXml(XModel * i_model) {
	model_ = i_model;
	getFeature();
	generateDomains();
	generateVariables();
	if (model_->feature.rs_size > 0)
		generateRelations();
	if (model_->feature.ps_size > 0)
		generatePredicates();
	generateConstraints();
}

std::string XBuilder::GetBMFile() const {
	std::string bm_file = "";

	if (true) {
		DOMNodeList *bmfile_list = root_->getElementsByTagName(XMLString::transcode("BMFile"));
		DOMNode *bmfile = bmfile_list->item(0);
		char* bm_name = XMLString::transcode(bmfile->getFirstChild()->getNodeValue());
		bm_file = bm_name;
		XMLString::release(&bm_name);
	}

	return bm_file;
}

XBuilder::~XBuilder() {
	if (root_) {
		delete (parser_);
		parser_ = NULL;
		XMLPlatformUtils::Terminate();
	}
}

void XBuilder::getFeature() {
	model_->feature.max_arity = getMaxArity();
	model_->feature.ds_size = getDomsNum();
	model_->feature.vs_size = getVarsNum();
	model_->feature.rs_size = getRelsNum();
	model_->feature.ps_size = getPresNum();
	model_->feature.cs_size = getConsNum();
}

bool XBuilder::initial() {
	if (this->file_name_ == "")
		return false;

	try {
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException & toCatch) {
		// Do your failure processing here
		return false;
	}
	// Do your actual work with Xerces-C++ here.
	parser_ = new XercesDOMParser();
	parser_->setValidationScheme(XercesDOMParser::Val_Always);
	parser_->setDoNamespaces(true);

	parser_->parse(file_name_.c_str());
	//std::cout << file_name << std::endl;
	document_ = parser_->getDocument();
	root_ = document_->getDocumentElement();

	if (!root_) {
		delete (parser_);
		parser_ = NULL;
		return false;
	}
	return true;
}


int XBuilder::getDomsNum() {
	DOMNode *doms_nodes = root_->getElementsByTagName(XMLString::transcode("domains"))->item(0);
	int num_doms = XMLString::parseInt(doms_nodes->getAttributes()->getNamedItem(XMLString::transcode("nbDomains"))->getTextContent());
	return num_doms;
}

int XBuilder::getVarsNum() {
	DOMNode *vars_node = root_->getElementsByTagName(XMLString::transcode("variables"))->item(0);
	int num_vars = XMLString::parseInt(vars_node->getAttributes()->getNamedItem(XMLString::transcode("nbVariables"))->getTextContent());
	return num_vars;
}

int XBuilder::getRelsNum() {
	int num_rels = 0;
	DOMNode* rels_node = root_->getElementsByTagName(XMLString::transcode("relations"))->item(0);

	if (rels_node != nullptr)
		num_rels = XMLString::parseInt(rels_node->getAttributes()->getNamedItem(XMLString::transcode("nbRelations"))->getTextContent());

	return num_rels;
}

int XBuilder::getPresNum() {
	int num_pres = 0;
	DOMNode* pres_node = root_->getElementsByTagName(XMLString::transcode("predicates"))->item(0);

	if (pres_node != nullptr)
		num_pres = XMLString::parseInt(pres_node->getAttributes()->getNamedItem(XMLString::transcode("nbPredicates"))->getTextContent());

	return num_pres;
}

int XBuilder::getConsNum() {
	DOMNode* cons_node = root_->getElementsByTagName(XMLString::transcode("constraints"))->item(0);
	int num_cons = XMLString::parseInt(cons_node->getAttributes()->getNamedItem(XMLString::transcode("nbConstraints"))->getTextContent());
	return num_cons;
}

int XBuilder::getMaxArity() {
	DOMNode *node = root_->getElementsByTagName(XMLString::transcode("presentation"))->item(0);
	int max_arity = XMLString::parseInt(node->getAttributes()->getNamedItem(XMLString::transcode("maxConstraintArity"))->getTextContent());
	return max_arity;
}

void XBuilder::generateDomains() {
	int max_d_s = 0, size;
	DOMNodeList *nodes = root_->getElementsByTagName(XMLString::transcode("domain"));
	char * values;
	DOMNode * node;
	model_->doms = new XDom*[model_->feature.ds_size];
	//xds.resize(model_->feature.ds_size);

	for (int i = 0; i < model_->feature.ds_size; ++i) {
		node = nodes->item(i);
		size = XMLString::parseInt(node->getAttributes()->getNamedItem(XMLString::transcode("nbValues"))->getTextContent());
		max_d_s = MAX(max_d_s, size);
		values = XMLString::transcode(node->getFirstChild()->getNodeValue());
		model_->doms[i] = new XDom(i, size, values);
		//xds[i].MakeMap(model_->doms[i]);
	}

	model_->feature.max_dom_size = max_d_s;
}

void XBuilder::generateVariables() {
	DOMNode* var_node;
	DOMNodeList* var_nodes = root_->getElementsByTagName(XMLString::transcode("variable"));
	int dom_id;
	char* dom_id_str;
	model_->vars = new XVar*[model_->feature.vs_size];

	for (int i = 0; i < model_->feature.vs_size; ++i) {
		var_node = var_nodes->item(i);
		dom_id_str = XMLString::transcode(var_node->getAttributes()->getNamedItem(XMLString::transcode("domain"))->getTextContent());
		sscanf_s(dom_id_str, "D%d", &dom_id);
		model_->vars[i] = new XVar(i, dom_id);
		XMLString::release(&dom_id_str);
	}
}

void XBuilder::generateRelations() {
	DOMNode *node;
	DOMNodeList* nodes = root_->getElementsByTagName(XMLString::transcode("relation"));
	int arity;
	int num_tuples;
	Semantices sem;
	model_->rels = new XRel*[model_->feature.rs_size];

	for (int i = 0; i < model_->feature.rs_size; ++i) {
		node = nodes->item(i);
		arity = XMLString::parseInt(node->getAttributes()->getNamedItem(XMLString::transcode("arity"))->getTextContent());
		char* semantics = XMLString::transcode(node->getAttributes()->getNamedItem(XMLString::transcode("semantics"))->getTextContent());
		num_tuples = XMLString::parseInt(node->getAttributes()->getNamedItem(XMLString::transcode("nbTuples"))->getTextContent());
		//������semantics == supports��relation_type = SURPPOT
		sem = (strlen(semantics) == strlen("supports")) ? SEM_SUPPORT : SEM_CONFLICT;
		char* innertext = XMLString::transcode(node->getFirstChild()->getNodeValue());
		model_->rels[i] = new XRel(i, arity, num_tuples, sem, innertext);
		XMLString::release(&semantics);
		XMLString::release(&innertext);
	}
}

void XBuilder::generatePredicates() {
	DOMNode *node;
	DOMNodeList* nodes = root_->getElementsByTagName(XMLString::transcode("predicate"));
	model_->pres = new XPre*[model_->feature.ps_size];

	for (size_t i = 0; i < model_->feature.ps_size; ++i) {
		const int pre_id = i;
		node = nodes->item(i);
		char* paras_str = XMLString::transcode(node->getChildNodes()->item(1)->getFirstChild()->getNodeValue());
		char* fn_str = XMLString::transcode(node->getChildNodes()->item(3)->getChildNodes()->item(1)->getFirstChild()->getNodeValue());
		model_->pres[i] = new XPre(pre_id, paras_str, fn_str);
		XMLString::release(&paras_str);
		XMLString::release(&fn_str);
	}
}

void XBuilder::generateConstraints() {
	int rel_id;
	DOMNode* node;
	DOMNodeList* nodes = root_->getElementsByTagName(XMLString::transcode("constraint"));
	model_->cons = new XCon*[model_->feature.cs_size];
	RelType type;
	for (int i = 0; i < model_->feature.cs_size; ++i) {
		node = nodes->item(i);
		type = node->hasChildNodes() ? INT : EXT;
		int arity = XMLString::parseInt(node->getAttributes()->getNamedItem(XMLString::transcode("arity"))->getTextContent());
		char *scope_str = XMLString::transcode(node->getAttributes()->getNamedItem(XMLString::transcode("scope"))->getTextContent());
		char* rel_id_str = XMLString::transcode(node->getAttributes()->getNamedItem(XMLString::transcode("reference"))->getTextContent());
		if (type == INT) {
			sscanf_s(rel_id_str, "P%d", &rel_id);
			char* pars_str = XMLString::transcode(node->getChildNodes()->item(1)->getFirstChild()->getNodeValue());
			model_->cons[i] = new XINTCon(i, rel_id, arity, type, scope_str, pars_str);
			XMLString::release(&pars_str);
		}
		else if (type == EXT) {
			sscanf_s(rel_id_str, "R%d", &rel_id);
			model_->cons[i] = new XCon(i, rel_id, arity, type, scope_str);
		}
		XMLString::release(&scope_str);
		XMLString::release(&rel_id_str);
	}
}

}