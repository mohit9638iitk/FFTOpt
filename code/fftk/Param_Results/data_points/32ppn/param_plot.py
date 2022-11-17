#!/usr/bin/env python
# coding: utf-8

# In[ ]:





# In[7]:


import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


# In[8]:


df=pd.read_csv("block_4n_32ppn_1024.txt",names=['Y'])
temp1=np.array(df['Y'])
df=pd.read_csv("block_8n_32ppn_1024.txt",names=['Y'])
temp2=np.array(df['Y'])
df=pd.read_csv("block_16n_32ppn_1024.txt",names=['Y'])
temp3=np.array(df['Y'])


# In[9]:


df=pd.read_csv("non_1l_4n_32ppn_1024.txt",names=['Y'])
temp4=np.array(df['Y'])
df=pd.read_csv("non_1l_8n_32ppn_1024.txt",names=['Y'])
temp5=np.array(df['Y'])
df=pd.read_csv("non_1l_16n_32ppn_1024.txt",names=['Y'])
temp6=np.array(df['Y'])


# In[10]:


df=pd.read_csv("non_2l_4n_32ppn_1024.txt",names=['Y'])
temp7=np.array(df['Y'])
df=pd.read_csv("non_2l_8n_32ppn_1024.txt",names=['Y'])
temp8=np.array(df['Y'])
df=pd.read_csv("non_2l_16n_32ppn_1024.txt",names=['Y'])
temp9=np.array(df['Y'])


# In[11]:


df=pd.read_csv("non_4l_4n_32ppn_1024.txt",names=['Y'])
temp10=np.array(df['Y'])
df=pd.read_csv("non_4l_8n_32ppn_1024.txt",names=['Y'])
temp11=np.array(df['Y'])
df=pd.read_csv("non_4l_16n_32ppn_1024.txt",names=['Y'])
temp12=np.array(df['Y'])


# In[12]:


df=pd.read_csv("non_8l_4n_32ppn_1024.txt",names=['Y'])
temp13=np.array(df['Y'])
df=pd.read_csv("non_8l_8n_32ppn_1024.txt",names=['Y'])
temp14=np.array(df['Y'])
df=pd.read_csv("non_8l_16n_32ppn_1024.txt",names=['Y'])
temp15=np.array(df['Y'])


# In[13]:


df=pd.read_csv("rma_1l_4n_32ppn_1024.txt",names=['Y'])
temp16=np.array(df['Y'])
df=pd.read_csv("rma_1l_8n_32ppn_1024.txt",names=['Y'])
temp17=np.array(df['Y'])
df=pd.read_csv("rma_1l_16n_32ppn_1024.txt",names=['Y'])
temp18=np.array(df['Y'])


# In[14]:


df=pd.read_csv("rma_2l_4n_32ppn_1024.txt",names=['Y'])
temp19=np.array(df['Y'])
df=pd.read_csv("rma_2l_8n_32ppn_1024.txt",names=['Y'])
temp20=np.array(df['Y'])
df=pd.read_csv("rma_2l_16n_32ppn_1024.txt",names=['Y'])
temp21=np.array(df['Y'])


# In[15]:


df=pd.read_csv("rma_4l_4n_32ppn_1024.txt",names=['Y'])
temp22=np.array(df['Y'])
df=pd.read_csv("rma_4l_8n_32ppn_1024.txt",names=['Y'])
temp23=np.array(df['Y'])
df=pd.read_csv("rma_4l_16n_32ppn_1024.txt",names=['Y'])
temp24=np.array(df['Y'])


# In[16]:


df=pd.read_csv("rma_8l_4n_32ppn_1024.txt",names=['Y'])
temp25=np.array(df['Y'])
df=pd.read_csv("rma_8l_8n_32ppn_1024.txt",names=['Y'])
temp26=np.array(df['Y'])
df=pd.read_csv("rma_8l_16n_32ppn_1024.txt",names=['Y'])
temp27=np.array(df['Y'])


# In[17]:


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


# In[18]:


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


# In[19]:


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


# In[20]:


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


# In[21]:


x5=[]
y5=[]
helper=[]
for i in range(0,len(temp13)):
    helper.append(temp13[i])
x5.append(np.mean(helper))
y5.append(np.std(helper))
helper=[]
for i in range(0,len(temp14)):
    helper.append(temp14[i])
x5.append(np.mean(helper))
y5.append(np.std(helper))
helper=[]
for i in range(0,len(temp15)):
    helper.append(temp15[i])
x5.append(np.mean(helper))
y5.append(np.std(helper))


# In[22]:


x6=[]
y6=[]
helper=[]
for i in range(0,len(temp16)):
    helper.append(temp16[i])
x6.append(np.mean(helper))
y6.append(np.std(helper))
helper=[]
for i in range(0,len(temp17)):
    helper.append(temp17[i])
x6.append(np.mean(helper))
y6.append(np.std(helper))
helper=[]
for i in range(0,len(temp18)):
    helper.append(temp18[i])
x6.append(np.mean(helper))
y6.append(np.std(helper))


# In[23]:


x7=[]
y7=[]
helper=[]
for i in range(0,len(temp19)):
    helper.append(temp19[i])
x7.append(np.mean(helper))
y7.append(np.std(helper))
helper=[]
for i in range(0,len(temp20)):
    helper.append(temp20[i])
x7.append(np.mean(helper))
y7.append(np.std(helper))
helper=[]
for i in range(0,len(temp21)):
    helper.append(temp21[i])
x7.append(np.mean(helper))
y7.append(np.std(helper))


# In[24]:


x8=[]
y8=[]
helper=[]
for i in range(0,len(temp22)):
    helper.append(temp22[i])
x8.append(np.mean(helper))
y8.append(np.std(helper))
helper=[]
for i in range(0,len(temp23)):
    helper.append(temp23[i])
x8.append(np.mean(helper))
y8.append(np.std(helper))
helper=[]
for i in range(0,len(temp24)):
    helper.append(temp24[i])
x8.append(np.mean(helper))
y8.append(np.std(helper))


# In[25]:


x9=[]
y9=[]
helper=[]
for i in range(0,len(temp25)):
    helper.append(temp25[i])
x9.append(np.mean(helper))
y9.append(np.std(helper))
helper=[]
for i in range(0,len(temp26)):
    helper.append(temp26[i])
x9.append(np.mean(helper))
y9.append(np.std(helper))
helper=[]
for i in range(0,len(temp27)):
    helper.append(temp27[i])
x9.append(np.mean(helper))
y9.append(np.std(helper))


# In[34]:


def plot_fcn():
    label_x=['4','8','16']
    plt.figure(figsize=(18,18), dpi=100)
    plt.rcParams.update({'font.size': 24})
    plt.xticks(fontsize=42)
    
    plt.yticks(fontsize=42)
    plt.errorbar(label_x, x1,yerr=y1,fmt='-o',label="1024^3 grid (default FFTK)",linewidth=3,color = 'red',linestyle='solid',ecolor='r',capsize=3)
    plt.errorbar(label_x, x2,yerr=y2,fmt='-o',label="1024^3 grid (1-leader case with Isend/Irecv FFTK)",linewidth=3,color = 'darkorange',linestyle='solid',ecolor='darkorange',capsize=3)
    plt.errorbar(label_x, x3,yerr=y3,fmt='-o',label="1024^3 grid (2-leader case with Isend/Irecv FFTK)",linewidth=3,color = 'blue',linestyle='solid',ecolor='blue',capsize=3)
    plt.errorbar(label_x, x4,yerr=y4,fmt='-o',label="1024^3 grid (4-leader case with Isend/Irecv FFTK)",linewidth=3,color = 'magenta',linestyle='solid',ecolor='magenta',capsize=3)
    plt.errorbar(label_x, x5,yerr=y5,fmt='-o',label="1024^3 grid (8-leader case with Isend/Irecv FFTK)",linewidth=3,color = 'green',linestyle='solid',ecolor='green',capsize=3)
    plt.errorbar(label_x, x6,yerr=y6,fmt='-o',label="1024^3 grid (1-leader case with RMA FFTK)",linewidth=3,color = 'olive',linestyle='solid',ecolor='olive',capsize=3)
    plt.errorbar(label_x, x7,yerr=y7,fmt='-o',label="1024^3 grid (2-leader case with RMA FFTK)",linewidth=3,color = 'cyan',linestyle='solid',ecolor='cyan',capsize=3)
    plt.errorbar(label_x, x8,yerr=y8,fmt='-o',label="1024^3 grid (4-leader case with RMA FFTK)",linewidth=3,color = 'lime',linestyle='solid',ecolor='lime',capsize=3)
    plt.errorbar(label_x, x9,yerr=y9,fmt='-o',label="1024^3 grid (8-leader case with RMA FFTK)",linewidth=3,color = 'gray',linestyle='solid',ecolor='gray',capsize=3)
    plt.ylim(bottom=0) 
    plt.xlim(left=0)
    
    plt.xlabel("Number of nodes (processes per node = 32)",fontsize=38)
    plt.ylabel("Time (sec)",fontsize=38)
    plt.title("Time vs Number of nodes",fontsize=38)
    plt.legend()
    plt.savefig("isend_vs_rma_32ppn_fftk.jpg")


# In[35]:


plot_fcn()


# In[ ]:





# In[ ]:





# In[ ]:




