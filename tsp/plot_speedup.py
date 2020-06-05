#!/usr/bin/env python
# coding: utf-8

# In[48]:


import numpy as np
import matplotlib.pyplot as plt
import pandas as pd


# In[92]:


data = pd.read_csv("data/speedup_asus.csv")
#seq = np.array(data["seq"]).astype(int)*0.01
interval = np.array(range(1,9))
seq_0 = int(data["seq"][0])
seq = seq_0/np.array([seq_0 / (i)for i in interval])

thread = seq_0/np.array(data["Thread"]).astype(int)
omp = seq_0/np.array(data["OMP"]).astype(int)
ff = seq_0/np.array(data["ff"]).astype(int)
ff_v2 = seq_0/np.array(data["ff_v2"]).astype(int)
f = 1/16
teoretical =1/ (f + (1-f)/interval)# multiple line plot
plt.title("Speedup with 10000 agents on asus machine")
plt.plot( interval,teoretical, markerfacecolor='blue', linewidth=2,color='blue', label = "perfect speedup")
plt.plot( interval, omp, markerfacecolor='red', linewidth=2,color='red', label = "omp", linestyle = "--")
plt.plot(interval,  thread,  color='green', linewidth=2,  label="thread", linestyle = ":")

plt.plot( interval, ff_v2,  markerfacecolor='yellow', linewidth=2,color='blue', label = "fast flow v2", linestyle = "-.")
plt.plot( interval, ff,  markerfacecolor='black', linewidth=2,color='black', label = "fast flow", linestyle = "-.")

plt.xlabel("Number of workers")
plt.ylabel("Speedup")
plt.legend()
plt.show()
plt.savefig('data/Speedup_asus.png', dpi=300)

# multiple line plot

plt.title("Efficiency with 10000 agents on asus machine")
#plt.plot( interval,seq/interval, markerfacecolor='blue', linewidth=3,color='skyblue', label = "perfect speedup")
plt.plot( interval, omp/interval, markerfacecolor='red', linewidth=2,color='red', label = "omp", linestyle = "--")
plt.plot(interval,  thread/interval,  color='green', linewidth=2,  label="thread", linestyle = ":")
plt.plot( interval,ff_v2/interval,  markerfacecolor='black', linewidth=2,color='blue', label = "fast flow v2", linestyle = "-.")
plt.plot( interval,ff/interval,  markerfacecolor='black', linewidth=2,color='black', label = "fast flow", linestyle = "-.")

plt.xlabel("Number of workers")
plt.ylabel("Efficiency")
plt.legend()
plt.show()
plt.savefig('data/Efficiency_asus.png', dpi=300)



data = pd.read_csv("data/speedup_xeon.csv")


#seq = np.array(data["seq"]).astype(int)*0.01
interval = np.array(range(16,257, 16))
seq_0 = int(data["seq"][0])
seq = seq_0/np.array([seq_0 / (i)for i in interval])

thread = seq_0/np.array(data["Thread"]).astype(int)
omp = seq_0/np.array(data["OMP"]).astype(int)
ff = seq_0/np.array(data["ff"]).astype(int)
f_val = 1/80
teoretical =seq_0/ (f_val*seq_0 + ((1-f_val)*seq_0)/interval)
# multiple line plot
plt.title("Speedup with 100000 agents on xeon machine")
plt.plot( interval[:5],teoretical[:5], markerfacecolor='blue', linewidth=2,color='blue', label = "perfect speedup")
plt.plot( interval[:5], omp[:5], markerfacecolor='red', linewidth=2,color='red', label = "omp", linestyle = "--")
plt.plot(interval[:5],  thread[:5],  color='green', linewidth=2,  label="thread", linestyle = ":")

plt.plot( interval[:5], ff[:5],  markerfacecolor='black', linewidth=2,color='black', label = "fast flow", linestyle = "-.")

plt.xlabel("Number of workers")
plt.ylabel("Speedup")
plt.legend()
plt.show()
plt.savefig('data/Speedup_xeon.png', dpi=300)

