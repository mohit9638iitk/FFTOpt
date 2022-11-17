#!/usr/bin/env python
# coding: utf-8

# In[58]:


import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


# In[59]:


df=pd.read_csv("block_4n_8ppn_2048.txt",names=['Y'])
temp1=np.array(df['Y'])
df=pd.read_csv("block_8n_8ppn_2048.txt",names=['Y'])
temp2=np.array(df['Y'])
df=pd.read_csv("block_16n_8ppn_2048.txt",names=['Y'])
temp3=np.array(df['Y'])


# In[60]:


df=pd.read_csv("opt_4n_8ppn_2048.txt",names=['Y'])
temp4=np.array(df['Y'])
df=pd.read_csv("opt_8n_8ppn_2048.txt",names=['Y'])
temp5=np.array(df['Y'])
df=pd.read_csv("opt_16n_8ppn_2048.txt",names=['Y'])
temp6=np.array(df['Y'])


# In[61]:


df=pd.read_csv("block_4n_32ppn_2048.txt",names=['Y'])
temp7=np.array(df['Y'])
df=pd.read_csv("block_8n_32ppn_2048.txt",names=['Y'])
temp8=np.array(df['Y'])
df=pd.read_csv("block_16n_32ppn_2048.txt",names=['Y'])
temp9=np.array(df['Y'])


# In[62]:


df=pd.read_csv("opt_4n_32ppn_2048.txt",names=['Y'])
temp10=np.array(df['Y'])
df=pd.read_csv("opt_8n_32ppn_2048.txt",names=['Y'])
temp11=np.array(df['Y'])
df=pd.read_csv("opt_16n_32ppn_2048.txt",names=['Y'])
temp12=np.array(df['Y'])


# In[63]:


x1=[]
y1=[]
helper=[]
for i in range(0,len(temp1)):
    helper.append(temp1[i])
x1.append(np.mean(helper))
y1.append(np.std(helper))
helper=[]
for i in range(0,len(temp2)):
    helper.append(temp2[i])
x1.append(np.mean(helper))
y1.append(np.std(helper))
helper=[]
for i in range(0,len(temp3)):
    helper.append(temp3[i])
x1.append(np.mean(helper))
y1.append(np.std(helper))


# In[64]:


x2=[]
y2=[]
helper=[]
for i in range(0,len(temp4)):
    helper.append(temp4[i])
x2.append(np.mean(helper))
y2.append(np.std(helper))
helper=[]
for i in range(0,len(temp5)):
    helper.append(temp5[i])
x2.append(np.mean(helper))
y2.append(np.std(helper))
helper=[]
for i in range(0,len(temp6)):
    helper.append(temp6[i])
x2.append(np.mean(helper))
y2.append(np.std(helper))


# In[65]:


x3=[]
y3=[]
helper=[]
for i in range(0,len(temp7)):
    helper.append(temp7[i])
x3.append(np.mean(helper))
y3.append(np.std(helper))
helper=[]
for i in range(0,len(temp8)):
    helper.append(temp8[i])
x3.append(np.mean(helper))
y3.append(np.std(helper))
helper=[]
for i in range(0,len(temp9)):
    helper.append(temp9[i])
x3.append(np.mean(helper))
y3.append(np.std(helper))


# In[66]:


x4=[]
y4=[]
helper=[]
for i in range(0,len(temp10)):
    helper.append(temp10[i])
x4.append(np.mean(helper))
y4.append(np.std(helper))
helper=[]
for i in range(0,len(temp11)):
    helper.append(temp11[i])
x4.append(np.mean(helper))
y4.append(np.std(helper))
helper=[]
for i in range(0,len(temp12)):
    helper.append(temp12[i])
x4.append(np.mean(helper))
y4.append(np.std(helper))


# In[75]:


def plot_fcn_8ppn():
    label_x=['4','8','16']
    plt.figure(figsize=(20,20), dpi=80)
    plt.rcParams.update({'font.size': 26})
    plt.xticks(fontsize=36)
    
    plt.yticks(fontsize=36)
    plt.errorbar(label_x, x1,yerr=y1,fmt='-o',label="default 3d-DFT",linewidth=3,color = 'red',linestyle='solid',ecolor='r',capsize=3,marker="o",markersize=10)
    plt.errorbar(label_x, x2,yerr=y2,fmt='-o',label="3d-DFT with FFTOpt",linewidth=3,color = 'blue',linestyle='solid',ecolor='b',capsize=3,marker="^",markersize=10)
    plt.ylim(bottom=0) 
    plt.xlim(left=0)
    
    plt.xlabel("Number of nodes (processes per node = 8)",fontsize=38)
    plt.ylabel("Time (sec)",fontsize=38)
    plt.title("Time vs Number of nodes",fontsize=38)
    plt.legend()
    plt.savefig("3d-dft_8ppn.jpg")


# In[76]:


def plot_fcn_32ppn():
    label_x=['4','8','16']
    plt.figure(figsize=(20,20), dpi=80)
    plt.rcParams.update({'font.size': 26})
    plt.xticks(fontsize=36)
    
    plt.yticks(fontsize=36)
    plt.errorbar(label_x, x3,yerr=y3,fmt='-o',label="default 3d-DFT",linewidth=3,color = 'red',linestyle='solid',ecolor='r',capsize=3,marker="o",markersize=10)
    plt.errorbar(label_x, x4,yerr=y4,fmt='-o',label="3d-DFT with FFTOpt",linewidth=3,color = 'blue',linestyle='solid',ecolor='b',capsize=3,marker="^",markersize=10)
    plt.ylim(bottom=0) 
    plt.xlim(left=0)
    
    plt.xlabel("Number of nodes (processes per node = 32)",fontsize=38)
    plt.ylabel("Time (sec)",fontsize=38)
    plt.title("Time vs Number of nodes",fontsize=38)
    plt.legend()
    plt.savefig("3d-dft_32ppn.jpg")


# In[77]:


plot_fcn_8ppn()


# In[78]:


plot_fcn_32ppn()


# In[ ]:




