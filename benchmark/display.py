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
		if df.iloc[i,3] > 0:
			hit_ratio.append([df.iloc[i,0],df.iloc[i,1],df.iloc[i,3]/(df.iloc[i,3]+df.iloc[i+1,3])])
	return pd.DataFrame(data=hit_ratio, columns=['Time','Node','Hit Ratio'])
	


def display_hit_ratio(df,title):
	"""Affiche le hit ratio en fonction du temps pour chaque noeud du réseau"""
	newDF = hit_ratio(df)
	if newDF.shape[0] == 0:
		print(f'{title} is empty')
	else:
		sns.lineplot(x='Time',y='Hit Ratio',hue='Node', data=newDF)
		f = title.replace('.txt','')
		plt.title(f)
		plt.legend(loc='lower right', ncol=1, fontsize='xx-small', fancybox=True, shadow=True)
		plt.show()
		#fig.savefig(f'img/{f}.png', dpi=plt.gcf().dpi)


#
#	APP DELAY TRACE
#


def delay(df):
	mean_delay = []
	df = df.loc[df['Type']=='FullDelay']
	for i in range(df.shape[0]):
		mean_delay.append([df.iloc[i,0],df.iloc[i,1],df.iloc[i,5]])
	return pd.DataFrame(data=mean_delay, columns=['Time','Node', 'Delay'])


def display_delay(df,title):
	newDF = delay(df)
	if newDF.shape[0] == 0:
		print(f'{title} is empty')
	else:
		sns.lineplot(x='Time',y='Delay', hue='Node', data=newDF)
		f = title.replace('.txt','')
		plt.title(f)
		plt.legend(loc='lower right', ncol=1, fontsize='xx-small', fancybox=True, shadow=True)
		plt.show()
		#fig.savefig(f'img/{f}.png',dpi=plt.gcf().dpi)


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



