#!/usr/bin/env python
# coding: utf-8

# In[31]:


import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


# In[32]:


df=pd.read_csv("fftw_block_4n_4ppn_128.txt",names=['Y'])
temp1=np.array(df['Y'])
df=pd.read_csv("fftw_block_8n_4ppn_128.txt",names=['Y'])
temp2=np.array(df['Y'])
df=pd.read_csv("fftw_block_16n_4ppn_128.txt",names=['Y'])
temp3=np.array(df['Y'])
df=pd.read_csv("fftw_block_32n_4ppn_128.txt",names=['Y'])
temp4=np.array(df['Y'])


# In[33]:


df=pd.read_csv("fftw_block_4n_4ppn_256.txt",names=['Y'])
temp5=np.array(df['Y'])
df=pd.read_csv("fftw_block_8n_4ppn_256.txt",names=['Y'])
temp6=np.array(df['Y'])
df=pd.read_csv("fftw_block_16n_4ppn_256.txt",names=['Y'])
temp7=np.array(df['Y'])
df=pd.read_csv("fftw_block_32n_4ppn_256.txt",names=['Y'])
temp8=np.array(df['Y'])


# In[34]:


df=pd.read_csv("fftw_non_block_4n_4ppn_128.txt",names=['Y'])
temp9=np.array(df['Y'])
df=pd.read_csv("fftw_non_block_8n_4ppn_128.txt",names=['Y'])
temp10=np.array(df['Y'])
df=pd.read_csv("fftw_non_block_16n_4ppn_128.txt",names=['Y'])
temp11=np.array(df['Y'])
df=pd.read_csv("fftw_non_block_32n_4ppn_128.txt",names=['Y'])
temp12=np.array(df['Y'])


# In[35]:


df=pd.read_csv("fftw_non_block_4n_4ppn_256.txt",names=['Y'])
temp13=np.array(df['Y'])
df=pd.read_csv("fftw_non_block_8n_4ppn_256.txt",names=['Y'])
temp14=np.array(df['Y'])
df=pd.read_csv("fftw_non_block_16n_4ppn_256.txt",names=['Y'])
temp15=np.array(df['Y'])
df=pd.read_csv("fftw_non_block_32n_4ppn_256.txt",names=['Y'])
temp16=np.array(df['Y'])


# In[36]:


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
helper=[]
for i in range(0,len(temp4)):
    helper.append(temp4[i])
x1.append(np.mean(helper))
y1.append(np.std(helper))


# In[37]:


x2=[]
y2=[]
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
helper=[]
for i in range(0,len(temp7)):
    helper.append(temp7[i])
x2.append(np.mean(helper))
y2.append(np.std(helper))
helper=[]
for i in range(0,len(temp8)):
    helper.append(temp8[i])
x2.append(np.mean(helper))
y2.append(np.std(helper))


# In[38]:


x3=[]
y3=[]
helper=[]
for i in range(0,len(temp9)):
    helper.append(temp9[i])
x3.append(np.mean(helper))
y3.append(np.std(helper))
helper=[]
for i in range(0,len(temp10)):
    helper.append(temp10[i])
x3.append(np.mean(helper))
y3.append(np.std(helper))
helper=[]
for i in range(0,len(temp11)):
    helper.append(temp11[i])
x3.append(np.mean(helper))
y3.append(np.std(helper))
helper=[]
for i in range(0,len(temp12)):
    helper.append(temp12[i])
x3.append(np.mean(helper))
y3.append(np.std(helper))


# In[39]:


x4=[]
y4=[]
helper=[]
for i in range(0,len(temp13)):
    helper.append(temp13[i])
x4.append(np.mean(helper))
y4.append(np.std(helper))
helper=[]
for i in range(0,len(temp14)):
    helper.append(temp14[i])
x4.append(np.mean(helper))
y4.append(np.std(helper))
helper=[]
for i in range(0,len(temp15)):
    helper.append(temp15[i])
x4.append(np.mean(helper))
y4.append(np.std(helper))
helper=[]
for i in range(0,len(temp16)):
    helper.append(temp16[i])
x4.append(np.mean(helper))
y4.append(np.std(helper))


# In[40]:


def plot_fcn():
    label_x=['4','8','16','32']
    plt.figure(figsize=(15,15), dpi=200)
    plt.rcParams.update({'font.size': 20})
    plt.xticks(fontsize=30)
    
    plt.yticks(fontsize=30)
    
    plt.errorbar(label_x, x2,yerr=y2,fmt='-o',label="256^3 grid (default FFTW)",linewidth=3,color = 'r',linestyle='solid',ecolor='r',capsize=3,marker="o",markersize=10)
    plt.errorbar(label_x, x4,yerr=y4,fmt='-o',label="256^3 grid (optimized FFTW)",linewidth=3,color = 'b',linestyle='solid',ecolor='b',capsize=3,marker="^",markersize=10)
    plt.errorbar(label_x, x1,yerr=y1,fmt='-o',label="128^3 grid (default FFTW)",linewidth=3,color = 'r',linestyle='dashed',ecolor='r',capsize=3,marker="o",markersize=10)
    plt.errorbar(label_x, x3,yerr=y3,fmt='-o',label="128^3 grid (optimized FFTW)",linewidth=3,color = 'b',linestyle='dashed',ecolor='b',capsize=3,marker="^",markersize=10)
    plt.ylim(bottom=0) 
    plt.xlim(left=0)
    
    plt.xlabel("Number of nodes (processes per node = 4)",fontsize=30)
    plt.ylabel("Time (sec)",fontsize=30)
    plt.title("Time vs Number of nodes",fontsize=30)
    plt.legend()
    plt.savefig("fftw_scale.jpg")


# In[41]:

plot_fcn()





