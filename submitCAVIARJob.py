import os
import time
import random
import glob
import numpy as npy
import sys, traceback
import subprocess
from subprocess import Popen, PIPE


def maxSubmitReached(max):
	p1 = Popen(["qstat", "-u", "fhormoz"], stdout=PIPE)
	p2 = Popen(["wc", "-l"], stdin=p1.stdout, stdout=PIPE)
	output = p2.communicate()[0]	
	if(int(output) < max) :
		return False;
	else:
		return True;


TEMPLATE_SERIAL = """
#####################################
#$ -S /bin/bash
#$ -cwd
#$ -N {name}
#$ -e {errfile}
#$ -o {logfile}
#$ -pe make {slots}
#$ -l h_data=2G
#$ -l h_rt=4:00:00
#####################################
echo "------------------------------------------------------------------------"
echo "Job started on" `date`
echo "------------------------------------------------------------------------"
{script}
echo "------------------------------------------------------------------------"
echo "Job ended on" `date`
echo "------------------------------------------------------------------------"
"""

currentPath = os.getcwd();
outputPath  = currentPath + "/caviar_output/";

if not os.path.exists(currentPath):      
        os.makedirs(currentPath);
if not os.path.exists(outputPath):
	os.makedirs(outputPath);
if not os.path.exists(outputPath+"/log/"):      
        os.makedirs(outputPath+"/log/");

index = 0;
script = "";
for chrom in glob.glob(currentPath+"/caviar_input/*"):
	chromName = chrom.replace(currentPath+"/caviar_input/", "").replace("/","");
	for ldFile in glob.glob(chrom + "/*_new.LD") :	
		ldName = ldFile.replace(chrom+"/", "");	
		while( maxSubmitReached(400) ) :
			time.sleep(1);
		
		scriptfile = outputPath + "/log/command_" + chromName + "_" + ldName;
		logfile    = outputPath + "/log/command_" + chromName + "_" + ldName + ".log";
		errfile    = outputPath + "/log/command_" + chromName + "_" + ldName + ".err";
		
		outputfile = outputPath + "/" + chromName + "_" + ldName + ".out";
		zFile      = ldFile.replace('.LD', '.Z'); 
		index = index + 1;
		
		script = script + "/u/project/zarlab/abzhu/caviar/CAVIAR-C++/CAVIAR " + \
		 " -o " + outputfile + \
		 " -z " + zFile + \
		 " -l " + ldFile + \
		 " -c 6 " + \
		 " -f 1 -r 0.95 \n";
		
		if(index % 2 == 0 and index != 0):
			print script;
			scriptFILEHandler = open(scriptfile+'.qsub', 'wb');
			scriptFILEHandler.write(TEMPLATE_SERIAL.format(script=script, name="CAVIAR", logfile=logfile, errfile=errfile, slots=1))
			scriptFILEHandler.close();
			subprocess.call('qsub -v PYTHONPATH,LIBRARY_PATH ' + scriptfile + '.qsub', shell=True)
			script = "";
