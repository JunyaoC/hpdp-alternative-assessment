import subprocess

out = subprocess.Popen(['cat', 'test.txt'], 
           stdout=subprocess.PIPE, 
           stderr=subprocess.STDOUT)

stdout,stderr = out.communicate()
print(stdout)