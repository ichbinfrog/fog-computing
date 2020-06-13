import pandas as pd
import seaborn as sns
import numpy as np
import matplotlib.pyplot as plt
import re
import bench

from string import digits
from os import listdir
from os.path import isfile, join, getsize



#
#	CS TRACE
#

def hit_ratio(df):
	"""Calcul le hit ratio pour chaque noeud du réseau"""
	hit_ratio = []
	for i in range(0,df.shape[0],2):
		if [df.iloc[i,1],0,0] not in hit_ratio:
			hit_ratio.append([df.iloc[i,1],0,0])
	for i in range(0,df.shape[0],2):
		if df.iloc[i,3] > 0:
			for j in hit_ratio:
				if df.iloc[i,1]==j[0]:
					j[1] += df.iloc[i,3]/(df.iloc[i,3]+df.iloc[i+1,3])
					j[2] += 1

	newDF = pd.DataFrame(data=hit_ratio, columns=['Node','Hit_Ratio','Count'])
	newDF = newDF.loc[newDF['Count'] != 0]
	newDF = newDF.drop(columns=['Count'])
	return newDF.sort_values(by='Node')


def display_hit_ratio(df,title):
	"""Affiche le hit ratio en fonction du temps pour chaque noeud du réseau"""
	newDF = hit_ratio(df)
	if newDF.shape[0] == 0:
		print(f'{title} is empty')
	else:
		sns.barplot(x='Node',y='Hit_Ratio',data=newDF)
		plt.xticks(rotation=45)
		f = title.replace('.txt','')
		plt.title(f)
		plt.savefig(f'img/{f}.png', dpi=300)


#
#	APP DELAY TRACE
#


def delay(df):
	mean_delay = []
	df = df.loc[df['Type']=='FullDelay']
	for i in np.unique(df['Node']):
		tmp = sum(df.loc[df['Node']==i,['Node','DelayS']]['DelayS'])/df.loc[df['Node']==i].shape[0]
		mean_delay.append([i,tmp])
	newDF = pd.DataFrame(data=mean_delay, columns=['Node','Mean Delay'])
	return newDF.sort_values(by='Node')



def display_delay(df,title):
	newDF = delay(df)
	if newDF.shape[0] == 0:
		print(f'{title} is empty')
	else:
		sns.barplot(x='Node',y='Mean Delay',data=newDF)
		plt.xticks(rotation=45)
		f = title.replace('.txt','')
		plt.title(f)
		plt.savefig(f'img/{f}.png',dpi=300)


if __name__ == '__main__':
	onlyfiles = [f for f in listdir('out/') if isfile(join('out/', f))]
	app_grid = [f for f in onlyfiles if re.search('app_grid',f)]
	cs_grid = [f for f in onlyfiles if re.search('cs_grid',f)]
	
	for i in cs_grid:
		cs = bench.ContentStore([f'out/{i}'])
		display_hit_ratio(cs.data_,i)

	for i in app_grid:
		if getsize(f'out/{i}') != 0:
			cs = bench.ContentStore([f'out/{i}'])
			display_delay(cs.data_,i)



