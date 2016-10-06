import os
import numpy as npy

snpIDFile = "snp.1kg.txt";
cisGeneFile = "godot.eqtl.local.txt";

snpData = npy.genfromtxt(open(snpIDFile,'r'), dtype='str', usecols=(0,2,6), skip_header=1);

snpID = snpData[:,0];
chrom = snpData[:,1];
start = snpData[:,2];

cisGenFileHandler = open(cisGeneFile,'r');
cisGenFileHandler.readline();

currentPath = os.getcwd();
if not os.path.exists(currentPath + "/tmp/"):
	os.makedirs(currentPath + "/tmp/");

for line in cisGenFileHandler:
	splitData = line.split();
	if not os.path.exists(currentPath + "/tmp/" + chrom[int(splitData[1])-1]):
		os.makedirs(currentPath + "/tmp/" + chrom[int(splitData[1])-1]);
	fileData = open(currentPath + "/tmp/" + chrom[int(splitData[1])-1] +"/"+ splitData[0] + ".tmp", "a");
	fileData.write( chrom[int(splitData[1])-1]+"_"+start[int(splitData[1])-1] + "\t" + splitData[2] + "\n");
	fileData.close(); 
	print splitData[0], chrom[int(splitData[1])-1]+"_"+start[int(splitData[1])-1];	
	
