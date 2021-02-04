import subprocess
import pandas as pd

avgs = []

for i in range(2,32):

	runningAVG = 0

	for j in range(0,10):
		print("{} Processor iteration {}".format(i,j),end="\r")
		if i > 8:
			with subprocess.Popen(['mpirun', '-np',str(i),'--oversubscribe','solution.o'], stdout=subprocess.PIPE, stderr=subprocess.STDOUT) as p:
				output, errors = p.communicate()

			lines = output.decode('utf-8').splitlines()
		else:
			with subprocess.Popen(['mpirun', '-np',str(i),'solution.o'], stdout=subprocess.PIPE, stderr=subprocess.STDOUT) as p:
				output, errors = p.communicate()

			lines = output.decode('utf-8').splitlines()

		ellapsed = []

		for line in lines:
			if "Task Completed in " in line:
				second = float(line.split("in ")[1].split(" ")[0])
				ellapsed.append(second)

	
		runningAVG = runningAVG + float(sum(ellapsed)/i)

	procAvg = float(runningAVG/10)
	avgs.append(procAvg)
	print("{} Processors - Avg ellapsed: {} seconds".format(i,procAvg))

outDF = pd.DataFrame(avgs)
outDF.to_csv("benchmarking.csv")