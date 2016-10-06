import os
import glob
import numpy as npy


currentPath = os.getcwd();
if not os.path.exists(currentPath + "/caviar_input/"):
	os.makedirs(currentPath + "/caviar_input/");

for chrom in glob.glob(currentPath + "/tmp/*"):
	chromName = chrom.replace(currentPath + "/tmp/","");
	if not os.path.exists(currentPath + "/caviar_input/" + chromName):
        	os.makedirs(currentPath + "/caviar_input/" + chromName);
	for cisFile in glob.glob(chrom + "/*"):
		sigData = npy.loadtxt(cisFile, usecols=(1,), dtype='float');
		rsID    = npy.loadtxt(cisFile, usecols=(0,), dtype='str');	
		if(npy.any(npy.abs(sigData) >= 5)):
			#make the Zscore file & LD
			print cisFile;
			print sigData;
			cisFileName = cisFile.replace(chrom,"");
			print cisFileName;
			print chromName;

			if(len(sigData) >= 50):
				index    = npy.argsort(-npy.abs(sigData));
				Data     = sigData[index[0:50]];
				DatarsID = rsID[index[0:50]]
			else:
				Data = sigData;
				DatarsID = rsID;
			outFile = open(currentPath + "/caviar_input/" + chromName + "/" + cisFileName + ".Z","w");
			for rs,z in zip(DatarsID, Data):
				outFile.write(rs + "\t" + str(z) + "\n");
			outFile.close();	
