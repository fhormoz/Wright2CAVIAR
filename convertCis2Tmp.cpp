#include <vector>
#include <string>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

using namespace std;

bool dirExists(string dirName){
	struct stat st;
	if((stat(dirName.c_str(), &st) == 0) && (st.st_mode & S_IFDIR != 0) )
		return true;
	else
		return false;	
}

int main() {
	if (!dirExists("/test"))
		mkdir("test", S_IRUSR | S_IWUSR | S_IXUSR);
	//mkdir("test", S_IRUSR | S_IWUSR | S_IXUSR);
	string snpFILENAME = "/u/home/f/fhormoz/project/data/WrightNG2014eQTL/snp.1kg.txt";
	string cisFILENAME = "/u/home/f/fhormoz/project/data/WrightNG2014eQTL//godot.eqtl.local.txt";
	vector <string> snpPosMap;
	vector <string> snpChrom;
	//snporder	snpid	s0	str	a1	a2	s1	s2
	//1	rs12565286	chr1	+	G	C	721289	721290
	char * pch;
	int MAX_SEQ_LENGTH = 2000;
        char line[MAX_SEQ_LENGTH];
	
	FILE * inputSNP1KGFile;
	inputSNP1KGFile = fopen(snpFILENAME.c_str(), "r");	
	fgets (line, MAX_SEQ_LENGTH , inputSNP1KGFile);
	while(fgets (line, MAX_SEQ_LENGTH, inputSNP1KGFile) != NULL) {
		pch = strtok(line, "\t");
		pch = strtok(NULL, "\t");
		pch = strtok(NULL, "\t");
		snpChrom.push_back(pch);
		pch = strtok(NULL, "\t");
		pch = strtok(NULL, "\t");
		pch = strtok(NULL, "\t");
		pch = strtok(NULL, "\t");
		pch = strtok(NULL, "\t");
		pch[strlen(pch)-1] = '\0';
		snpPosMap.push_back(pch);
	}
	//probeid	snporder	tstat	pval
	//11750509_a_at	5968687	0.301	0.7634144945
	FILE * inputCisFile = fopen(cisFILENAME.c_str(), "r");
	char probID[MAX_SEQ_LENGTH];
	int snpOder = 0;
	double tStat = 0;
	fgets (line, MAX_SEQ_LENGTH, inputCisFile);
	while(fgets (line, MAX_SEQ_LENGTH, inputCisFile) != NULL) {
		pch = strtok(line, "\t");
		strcpy(probID, pch);		
		pch = strtok(NULL, "\t");
		snpOder = atoi(pch);
		pch = strtok(NULL, "\t");
		tStat = atof(pch);
		if(abs(tStat <= 2))
			continue;
		snpOder--;
		cout << line << endl;
		if (! dirExists(string("/test/")+snpChrom[snpOder]))
			mkdir((string("test/")+snpChrom[snpOder]).c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
		FILE * tmpFile = fopen( (string("test/")+snpChrom[snpOder] + "/"+string(probID)).c_str(), "a");
		fprintf(tmpFile, "%s_%s\t%f\n", snpChrom[snpOder].c_str(), snpPosMap[snpOder].c_str(), tStat); 
		fclose(tmpFile);
	}		
}
