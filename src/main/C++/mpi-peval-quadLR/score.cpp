#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <math.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <deque>

#include <map>
#include <mpi.h>

#include "Log_r.h"

using namespace std;
const int MAX_BUF_LEN = 1024*20;
map<unsigned int, int> *feasign2id_map;
typedef std::vector<double> DblVec;
DblVec X;
DblVec P;
vector<double>* randMatrixForUser;
vector<double>* randMatrixForAd;
char g_str_logconf[1024] ="/home/a/share/phoenix/mpi-algo-platform/conf/log4cpp.conf";

char feamap_path[2048];
char ins_path[2048];
char model_path[2048];

int rpAdFeaCount = 0;
int rpUserFeaCount = 0;

vector<double> split(string str, string pattern){
	int pos;
	vector<double> result;
	str+=pattern;
	int size = str.size();
	for(int i = 0; i < size; i++){
		pos = str.find(pattern, i);
		if(pos< size){
			string s = str.substr(i, pos-i);
			result.push_back(atof(s));
			i = pos+pattern.size() -1;
		}
	}
	return result;
}

void load_randmat_for_user(const char* randmat_path_user){
	string line;
	ifstream pfeamap(randmat_path_user);
	if(!pfeamap.good()){
		cerr << "error rand mat file for user" << endl;
		exit(1);
	}
	getline(pfeamap, line);
	int pos = line.find("X");
	int rownum = atoi(line.substr(0, pos).c_str());
	int colnum = atoi(line.substr(pos+1, line.length()-pos-1).c_str());
	randMatrixForUser = new vector<double>[rownum];
	int i = 0;
	while (getline(pfeamap, line)){
		randMatrixForUser[i++] = split(line, "\t");
	}
	pfeamap.close();
	rpUserFeaCount = rownum;
}

void load_randmat_for_ad(const char* randmat_path_ad){
	string line;
	ifstream pfeamap(randmat_path_ad);
	if(!pfeamap.good()){
		cerr << "error rand mat file for ad" << endl;
		exit(1);
	}
	getline(pfeamap, line);
	int pos = line.find("X");
	int rownum = atoi(line.substr(0, pos).c_str());
	int colnum = atoi(line.substr(pos+1, line.length()-pos-1).c_str());
	randMatrixForAd = new vector<double>[rownum];
	int i = 0;
	while (getline(pfeamap, line)){
		randMatrixForAd[i++] = split(line, "\t");
	}
	pfeamap.close();
	rpAdFeaCount = rownum;
}

vector<int> get_rp_instance(const vector<int>& instance){
	vector<int> rpInstance;
	for(int i = 0; i < rpAdFeaCount; i++){
		double sum = 0;
		for(int k = 0; k < instance.size(); k++){
			if(instance[k] < getAdFeaCount()){
				sum += randMatrixForAd[i][instance[k]];
			}
		}
		if(sum > 0) rpInstance.push_back(i);
	}
	for(int i = 0; i < rpUserFeaCount; i++){
		double sum = 0;
		for(int k = 0; k < instance.size(); k++){
			if(instance[k] >= getAdFeaCount() && instance[k] < getAdFeaCount() + getUserFeaCount()){
				sum += randMatrixForUser[i][instance[k]-getAdFeaCount()];
			}
		}
		if(sum > 0) rpInstance.push_back(i+rpAdFeaCount);
	}
	return rpInstance;
}

int getAdFeaCount(){
	return feasign2id_map[0].size();
}
int getUserFeaCount(){
	return feasign2id_map[1].size();
}
int getOtherFeaCount(){
	return feasign2id_map[2].size();
}
int getAllFeaCount(){
	return feasign2id_map[0].size() + feasign2id_map[1].size() + feasign2id_map[2].size();
}	
int getRpAdFeaCount(){
	return rpAdFeaCount;
}
int getRpUserFeaCount(){
	return rpUserFeaCount;
}

	/*
    * Get feature Type
    * AdFeature: a 		return 0
    * UserFeature: u 	return 1 
    * OtherFeature: o 	return 2
    */
    
int get_feature(const char* fsign, unsigned int& feasign){
	feasign = strtoul(fsign+1, NULL, 16);
	if(fsign[0] == 'a'){
		return 0;
	}
	else if(fsign[0] == 'u'){
		return 1;
	}
	else{
		return 2;
	}
}

// FEATUREIDLIST: 
//	AD FEATURE[0 - feaid[0]-1], 
//	USER FEATURE[feaid[0] - (feaid[1] + feaid[0] - 1)], 
//	OTHER FEATURE[(feaid[1]+feaid[0]) - (feaid[0] + feaid[1] +feaid[2] -1)]
		
int get_featid(const char* fsign){
	unsigned int feasign = 0;
	int type = get_feature(fsign, feasign);
	int featid = 0;
	if(feasign2id_map[type].find(feasign) == feasign2id_map[type].end()) return -1;
	if(type == 0) {
		featid = feasign2id_map[type][feasign];
	}
	else if(type == 1){
		featid = getAdFeaCount() + feasign2id_map[type][feasign];
	}
	else{
		featid = getAdFeaCount() + getUserFeaCount() + feasign2id_map[type][feasign];
	}
	return featid;
}

int load_feamap(const char* feamap_path){
	unsigned int feasign = 0;
	string line;
	int feaid[3];
	feaid[0] = 1; feaid[1] = 1; feaid[2] = 0;
	ifstream pfeamap(feamap_path);

	
	if(!pfeamap.good()){
		cerr << "error feature map file" << endl;
		exit(1);
	}

	while (getline(pfeamap, line)){
		int type = get_feature(line.c_str(), feasign);
		feasign2id_map[type][feasign] = feaid[type];
		feaid[type]++;
	}
	pfeamap.close();
	return 0;
}

int load_model(const char* model_path){
	string line;
	char model_file_name[50];
	sprintf(model_file_name, "%sP", model_path);
	double fea_weight;
	
	//load P
	ifstream pmodel(model_file_name);
	if(!pmodel.good()){
		cerr << "error model file " << model_file_name << endl;
		exit(1);
	}
	while (getline(pmodel, line)){
    	fea_weight = atof(line.c_str());
    	P.push_back(fea_weight);
	}
	pmodel.close();
	
	
	//load W
	sprintf(model_file_name, "%sX", model_path);
	ifstream xmodel(model_file_name);
	if(!xmodel.good()){
		cerr << "error model file " << model_file_name  << endl;
		exit(1);
	}
	while (getline(xmodel, line)){
    	fea_weight = atof(line.c_str());
    	X.push_back(fea_weight);
	}
	xmodel.close();
	
	return 0;
}

double cal_score(vector <size_t> instance){
	//f(x)=tXu + Px
	vector<size_t> rp_instance = get_rp_instance(instance);
	double score = 0.0;	
	vector<size_t> a;
	vector<size_t> u;
	for (size_t j = 0; j < rp_instance.size(); j++){
		size_t index = rp_instance[j];
		//Ad Feature
		if(index < numRpAdFeature){
			a.push_back(index);
		}
		//User Feature
		else if(index < numRpAdFeature + numRpUserFeature){
			u.push_back(index-numRpAdFeature);
		}
	}
	for(size_t i = 0; i < a.size(); i++){
		for(size_t j = 0; j < u.size(); j++){
			score += X[a[i]*numRpUserFeature + u[j]];
		}
	}
		
	for (size_t j = instance_starts[i]; j < instance_starts[i+1]; j++){
		if(features[j] >= getAdFeaCount()+getUserFeaCount())
			score += P[features[j]]*1.0;
	}
	return score;
}

double get_ctr(double score) {
    double ctr = 1/(1 + exp(-score));
    return ctr;
}


int score_ins(const char* score_path){
	char insinput[50];
	sprintf(insinput, "%s", ins_path);
	ifstream fins(insinput);
	FILE* p_out = fopen(score_path, "w");
	char line[MAX_BUF_LEN];
	string linestr;
	const char CTRL_A = '';
	const char CTRL_B = '';
    const char END = '';
    
	
	char feasign[10];
	int feaid = 0;
	
	if(!fins.good()){
		cerr << "error instance file" << endl;
		exit(1);
	}

	
	while(getline(fins, linestr)){
		strcpy(line, linestr.c_str());
		line[linestr.size()] = END;
		
		vector <size_t> instance;
		size_t temp_nonclick = 0;
		size_t temp_click = 0;
		// Instance parser
		// Get each instance 
		// CLICK_NUM/NONCLICK_NUM/FEATUREIDLIST
		// FEATUREIDLIST: 
		//	AD FEATURE[0 - feaid[0]-1], 
		//	USER FEATURE[feaid[0] - (feaid[1] + feaid[0] - 1)], 
		//	OTHER FEATURE[(feaid[1]+feaid[0]) - (feaid[0] + feaid[1] +feaid[2] -1)]
			
		char *p_begin = line;
		char *p_end = p_begin;
		char *p_fea;
		//nonclick
		while(*p_begin != CTRL_A){
			p_begin ++;
		}
		*p_begin=0;
		sscanf(p_end, "%u", &temp_nonclick);
		p_begin++;
		p_end = p_begin;
		
		//clk
		while(*p_begin != CTRL_A){
			p_begin++;
		}
		*p_begin = 0;
		sscanf(p_end, "%u", &temp_click);
		p_begin++;
		p_end = p_begin;
		
		bool bEnd = false;
		instance.push_back(0);
		instance.push_back(getAdFeaCount());
		while(!bEnd){
			p_end = p_begin;
			while(*p_end != CTRL_A && *p_end != END){
				p_end++;
			}
			if(*p_end == END){
				bEnd = true;
			}
			
			if(*p_end == CTRL_A || *p_end == END){
				p_fea = p_begin;
				*p_end = 0;
				sscanf(p_fea, "%s", feasign);
				//getfeaid
				feaid = get_featid(feasign);
				if(feaid == -1){
					p_begin = p_end + 1;
					continue;
				}
				//add to instance list
				instance.push_back(feaid);
				
			}
			
			p_begin = p_end+1;
			
		}
		sort(&instance[0], &instance[instance.size()]);
		//score and output to file
		double score = cal_score(instance);
		//cout << score << endl;
		double ctr = get_ctr(score);
		fprintf(p_out, "%lf%d%d%s\n", 1.0*ctr, temp_nonclick, temp_click, "Q");
		
	}
	delete []feasign2id_map;
	fins.close();
	fclose(p_out);
	return 0;				
}

int init(const char* feamap, const char* model, const char* ins, const char* randmat_path_ad, const char* randmat_path_user)
{
	snprintf(feamap_path, 2048, "%s", feamap);
	snprintf(model_path, 2048, "%s", model);
	snprintf(ins_path, 2048, "%s", ins);
	feasign2id_map = new map<unsigned int, int>[3];
	load_feamap(feamap_path);
	load_model(model_path);
	load_randmat_for_user(randmat_path_user);
	load_randmat_for_ad(randmat_path_ad);
	return 0;
}

int main(int argc, char *argv[]) {

    int rank_id = 0;
    int num_procs = 0;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank_id);
    if(3 >= argc) {
        fprintf(stderr, "Args : model instance output.\n");
		MPI_Finalize();
        return -1;
    }
    if (Log_r::Init(g_str_logconf)) {
        fprintf(stderr, "log init fail str_logconf[%s]\n", g_str_logconf);
		MPI_Finalize();
        return -1;
    }
    Log_r_Info("Log_r::Init ok");


    Log_r_Info("argv[1]:%s\n", argv[1]);
    Log_r_Info("argv[2]:%s\n", argv[2]);
    Log_r_Info("argv[3]:%s\n", argv[3]);


   
    char inst_file[1024];
    inst_file[0] = '\0';
    snprintf(inst_file, sizeof(inst_file), "%s-%05d", argv[2], rank_id);

    Log_r_Info("Rank[%d][Eval Ins:%s]\n", rank_id, inst_file);

	init("./FeaDict.dat", argv[1], inst_file, "./rand_ad", "./rand_user");
	score_ins(argv[3]);


    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}

