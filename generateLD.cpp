#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <map>
#include <string.h>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;

int getdir (string dir, vector<string> &files) {
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }
    while ((dirp = readdir(dp)) != NULL) {
        if(string(dirp->d_name).find(".Z") != string::npos)
                files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

std::size_t find_nth(const std::string& s, std::size_t nth, char val) {
    std::size_t pos = 0;
    unsigned occurrence = 0;
    while (occurrence != nth) {
        pos = s.find(val, pos+1);
	++occurrence;
    }
    return pos;
}

double computeAvg(vector <double> data) {
        double total = 0;
        for(int i = 0; i < data.size(); i++)
                total = total + data[i];
        return(total/data.size());
}

double computeVar(vector <double> data, double mean) {
        double total = 0;
        for(int i = 0; i < data.size(); i++)
                total = total + ((data[i]-mean) * (data[i]-mean));
        return( total/data.size() );
}

double computeCov(vector <double> data1, vector <double> data2) {
        double mean1 = computeAvg(data1);
        double mean2 = computeAvg(data2);
        double total = 0;
        for(int i = 0; i < data1.size(); i++)
                total = total + ((data1[i]-mean1) * (data2[i]-mean2));
        return(total / data1.size());
}

double computeCor(string data1, string data2) {
        double d;
        vector <double> dataV1;
        vector <double> dataV2;
        stringstream ssin1(data1);
        stringstream ssin2(data2);
        while(!ssin1.eof()) {
                ssin1 >> d;
                dataV1.push_back(d);
                ssin2 >> d;
                dataV2.push_back(d);
        }
        double mean1 = computeAvg(dataV1);
        double var1  = computeVar(dataV1, mean1);
        double mean2 = computeAvg(dataV2);
        double var2  = computeVar(dataV2, mean2);
        double cov   = computeCov(dataV1, dataV2);
        return (cov/(sqrt(var1)*sqrt(var2)));
}


void computeLD( vector <string> snpIds, map<string, string> hashMapSNP2Genotype, double **LD) {
	for(int i = 0; i < snpIds.size(); i++){
		for(int j = 0; j < snpIds.size(); j++){
			LD[i][j] = computeCor(hashMapSNP2Genotype[snpIds[i]], hashMapSNP2Genotype[snpIds[j]]);
		}
	}
}

void writeZ2File(vector <string> SNPs, vector <double> Z, string newZFile) {
	FILE * outFile = fopen(newZFile.c_str(), "w");
	for(int i = 0; i < SNPs.size(); i++) {
		fprintf(outFile, "%s\t%f\n", SNPs[i].c_str(), Z[i]);
	}	
	fclose(outFile);
}

void writeLD2File(string ldFile, double ** LD, int snpSize){
	FILE * outFile = fopen(ldFile.c_str(), "w");
	for(int i = 0; i < snpSize; i++) {
		for(int j = 0; j < snpSize; j++) {
			fprintf(outFile, "%f\t", LD[i][j]);
		}	
		fprintf(outFile, "\n");
	}
	fclose(outFile);
}

int main(int argc, char *argv[]){
	int oc = 0;
	map <string, int> hashMapSNPs;
	map<string, string> hashMapSNP2Genotype;
	string plinktPed = "/u/home/f/fhormoz/project/data/SC/data/chr1_new.tped";
	string folderPath = "/u/home/f/fhormoz/project/data/WrightNG2014eQTL/caviar_input/chr1/";

	char *pch;
	FILE * inputZFile;
	FILE * inputTpedFile;
	int MAX_SEQ_LENGTH = 2000;
	char line[MAX_SEQ_LENGTH];
	char tmp[MAX_SEQ_LENGTH];


	while ((oc = getopt(argc, argv, "vhl:f:t:")) != -1) {
                switch (oc) {
                        case 'v':
                                cout << "version 0.0:" << endl;
                        case 'h':
                                cout << "Options: " << endl;
                                cout << " -h help"   << endl;
                                cout << " -t Tped 1KG File"  << " specify the name of tissue" << endl;
                                cout << " -f Genotype File" << " specify the GTEx Genotype folder" << endl;
                                return(0);
                        case 't':
                                plinktPed  = string(optarg);
                                break;
                        case 'f':
                                folderPath = string(optarg);
                                break;
                }
        }


	vector <string> zFiles;
	getdir(folderPath, zFiles);
	for(int i = 0; i < zFiles.size(); i++) {
		cout << zFiles[i]<< endl;
		string tmpFile = string(folderPath) + zFiles[i];
		inputZFile = fopen(tmpFile.c_str(), "r");
		while(fgets (line, MAX_SEQ_LENGTH , inputZFile) != NULL) {
			pch = strtok (line,"\t");
			hashMapSNPs[string(pch).substr(string(pch).find("_")+1)] = 1;
		}
		fclose(inputZFile);
	}
	//read the tped data
	inputTpedFile = fopen(plinktPed.c_str(), "r");
	while(fgets (line, MAX_SEQ_LENGTH , inputTpedFile) != NULL) {
		strcpy(tmp, line);
		pch = strtok (tmp,"\t");
		pch = strtok (NULL,"\t");
		pch = strtok (NULL,"\t");
		pch = strtok (NULL,"\t");		
		if(hashMapSNPs.find(string(pch)) != hashMapSNPs.end()) {
			hashMapSNP2Genotype[string(pch)] = string(line).substr(find_nth(string(line), 4, '\t')+1);
		}	
	}
	vector <string> tmpSNPs;
	vector <double> tmpZ;
	double ** LD;
	int snpSize = 0;
	for(int i = 0; i < zFiles.size(); i++) {
                snpSize = 0;
		string tmpFile = string(folderPath) + zFiles[i];
                inputZFile = fopen(tmpFile.c_str(), "r");
		while(fgets (line, MAX_SEQ_LENGTH , inputZFile) != NULL) {
                        pch = strtok (line,"\t");
			if(hashMapSNP2Genotype.find(string(pch).substr(string(pch).find("_")+1)) != hashMapSNP2Genotype.end()) {
				tmpSNPs.push_back(string(pch).substr(string(pch).find("_")+1));
				pch = strtok (NULL,"\t");
				tmpZ.push_back(atof(pch));
				snpSize = snpSize + 1;
			}
		}
		if(snpSize >=2 ){
			LD = new double*[snpSize];
			for(int j=0; j < snpSize; j++)
				LD[j] = new double[snpSize];
			computeLD(tmpSNPs, hashMapSNP2Genotype, LD);
			//Write LD matrix
			string ldFile = tmpFile;
			string newZFile = tmpFile;
			ldFile.replace(ldFile.find(".Z"),7,"_new.LD");
			newZFile.replace(newZFile.find(".Z"),6,"_new.Z");
			writeLD2File(ldFile, LD, snpSize);
			writeZ2File(tmpSNPs, tmpZ, newZFile);
			cout << ldFile << endl;
			//Clean it
			for(int j = 0; j < snpSize; j++)
				delete [] LD[j];
			delete [] LD;
			tmpSNPs.erase(tmpSNPs.begin(), tmpSNPs.end());
			tmpZ.erase(tmpZ.begin(), tmpZ.end());	
		} else{
			tmpSNPs.erase(tmpSNPs.begin(), tmpSNPs.end());
			tmpZ.erase(tmpZ.begin(), tmpZ.end());
		}
	}
}
