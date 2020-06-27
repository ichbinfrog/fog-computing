import pandas as pd
import seaborn as sns
import numpy as np
import matplotlib.pyplot as plt
import re
import bench

from string import digits
from os import listdir
from os.path import isfile, join, getsize
from math import *



#
#	CS TRACE
#

def hit_ratio(df):
	"""Calcul le hit ratio pour chaque noeud du réseau"""
	hit_ratio = []
	for i in range(0,df.shape[0],2):
		if df.iloc[i+1,3] != 0:
			hit_ratio.append([df.iloc[i,0],df.iloc[i,7],df.iloc[i,3]/(df.iloc[i,3]+df.iloc[i+1,3])])
	df = pd.DataFrame(data=hit_ratio, columns=['Time','Cache','Hit Ratio'])
	return df


def display_hit_ratio(list_df):
	"""Affiche le hit ratio en fonction du temps pour chaque noeud du réseau"""
	for pos,i in enumerate(list_df):
		if i[0].empty == False:
			label = i[1].replace('.txt','')
			sns.lineplot(x='Time',y='Hit Ratio', data=i[0], label=label)

	plt.legend(loc='upper right', ncol=1, fontsize='xx-small', fancybox=True, shadow=True)
	plt.show()
		#fig.savefig(f'img/{f}.png', dpi=plt.gcf().dpi)


def display_hit_ratio_cee_15_15s():
	onlyfiles = ['out/'+f for f in listdir('out/') if isfile(join('out/', f)) and 'cs_grid' in f and 'fog' not in f and 'frontier' not in f]
	df = bench.ContentStore(onlyfiles).data_
	df = hit_ratio(df)
	df['Interval'] = df['Time'] // 2
	sns.lineplot(x='Interval',y='Hit Ratio', hue='Cache', data=df)
	sns.set(font_scale=1.1)
	plt.legend(loc='upper left')
	plt.figure(figsize=(10,8))
	plt.show()


def display_hit_ratio_cee_15_60s():
	onlyfiles = ['out_back_up/'+f for f in listdir('out_back_up/') if isfile(join('out_back_up/', f)) and 'cs_grid' in f and 'fog' not in f and 'frontier' not in f]
	df = bench.ContentStore(onlyfiles).data_
	df = hit_ratio(df)
	df['Interval'] = df['Time'] // 5
	sns.lineplot(x='Interval',y='Hit Ratio', hue='Cache', data=df)
	sns.set(font_scale=1.1)
	sns.set_style('darkgrid')
	plt.legend(loc='upper left')
	plt.figure(figsize=(10,8))
	plt.show()


def display_hit_ratio_cee_100_15s():
	onlyfiles = ['out_100/'+f for f in listdir('out_100/') if isfile(join('out_100/', f)) and 'cs_grid' in f and 'fog' not in f and 'frontier' not in f]
	df = bench.ContentStore(onlyfiles).data_
	df = hit_ratio(df)
	df['Interval'] = df['Time'] // 2	
	sns.lineplot(x='Interval',y='Hit Ratio', hue='Cache', data=df)
	sns.set(font_scale=1.1)
	plt.legend(loc='upper left')
	plt.figure(figsize=(10,8))
	plt.show()


def display_hit_ratio_only_buf():
	onlyfiles = ['out_back_up/'+f for f in listdir('out_back_up/') if isfile(join('out_back_up/', f)) and 'cs_grid' in f and ('fog' in f or 'reference' in f)]
	df = bench.ContentStore(onlyfiles).data_
	df = df.loc[df['NodeType'] == 'buf']
	df = hit_ratio(df)
	df['Interval'] = df['Time'] // 2
	
	sns.set_style("whitegrid")
	sns.set(font_scale=1.8)
	sns.lineplot(x='Interval',y='Hit Ratio', hue='Cache', data=df)
	plt.show()

def display_hit_ratio_only_frontier():
	onlyfiles = ['out_back_up/'+f for f in listdir('out_back_up/') if isfile(join('out_back_up/', f)) and 'cs_grid' in f and ('frontier' in f or 'reference' in f)]
	df = bench.ContentStore(onlyfiles).data_
	df = df.loc[df['NodeType'] == 'frt']
	df = hit_ratio(df)
	df['Interval'] = df['Time'] // 2
	
	sns.set_style("whitegrid")
	sns.set(font_scale=1.8)
	sns.lineplot(x='Interval',y='Hit Ratio', hue='Cache', data=df)
	plt.show()




