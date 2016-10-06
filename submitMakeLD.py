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
#$ -l h_rt=6:00:00
#####################################
echo "------------------------------------------------------------------------"
echo "Job started on" `date`
echo "------------------------------------------------------------------------"
{script}
echo "------------------------------------------------------------------------"
echo "Job ended on" `date`
echo "------------------------------------------------------------------------"
"""

path1KGPLINK = "/u/home/f/fhormoz/project/data/SC/data";
currentPath = os.getcwd();

if not os.path.exists(currentPath+"/log/"):
        os.makedirs(currentPath+"/log/");

for chromFiles in glob.glob(currentPath+"/caviar_input/*") :	
	chromName = chromFiles.replace(currentPath+"/caviar_input/", "");
	scriptfile = currentPath+ "/log/command_"  + chromName;
	logfile    = currentPath + "/log/command_" + chromName + ".log";
	errfile    = currentPath + "/log/command_" + chromName + ".err";
	
	script = "/u/project/zarlab/fhormoz/data/WrightNG2014eQTL/generateLD " + \
		 " -f " + chromFiles + "/" +\
		 " -t " + path1KGPLINK + "/" + chromName + "_new.tped";
	print script;
	scriptFILEHandler = open(scriptfile+'.qsub', 'wb');
	scriptFILEHandler.write(TEMPLATE_SERIAL.format(script=script, name="eCAVIAR", logfile=logfile, errfile=errfile, slots=1))
	scriptFILEHandler.close();
	subprocess.call('qsub -v PYTHONPATH,LIBRARY_PATH ' + scriptfile + '.qsub', shell=True)
