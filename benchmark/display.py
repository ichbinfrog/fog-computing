import pandas as pd
import seaborn as sns
import numpy as np
import matplotlib.pyplot as plt

from string import digits


#
#	CS TRACE
#

def hit_ratio(df):
	"""Calcul le hit ratio pour chaque noeud du réseau"""
	hit_ratio = []
	for i in range(0,df.shape[0],2):
		if df.iloc[i,3] > 0:
			hit_ratio.append([df.iloc[i,0], df.iloc[i,1], df.iloc[i,3]/(df.iloc[i,3]+df.iloc[i+1,3])])

	return pd.DataFrame(data=hit_ratio, columns=['Time','Node','Hit_Ratio'])


def display_hit_ratio(df):
	"""Affiche le hit ratio en fonction du temps pour chaque noeud du réseau"""
	newDF = hit_ratio(df)
	print(newDF)

	partition = []
	remove_digits = str.maketrans('', '', digits)

	for i in newDF['Node']:
		partition.append(i.translate(remove_digits))
	partition = np.unique(partition)


	tmp = 0
	for i in partition:
		node = [j for j in newDF['Node'] if j.startswith(i)]
		node = np.unique(node)
		if len(node) > 0:
			plt.figure(tmp)
			for j in node:
				sns.lineplot(x='Time',y='Hit_Ratio', data=newDF.loc[newDF['Node']==j])
			plt.legend(labels=node)
		tmp +=1

	plt.show()


#	node = [i for i in np.unique(newDF['Node'])]
#	for i in node:
#		sns.lineplot(x='Time',y='Hit_Ratio', data=newDF.loc[newDF['Node']==i])
#	plt.legend(labels=node)
#	plt.show()


#
#	APP DELAY TRACE
#


def delay(df):
	mean_delay = []
	reduce_data = []
	df = df.loc[df['Type']=='FullDelay']
	for i in np.unique(df['Node']):
		df_tmp = df.loc[df['Node']==i]
		reduce_data.append((i,df_tmp[['Time','DelayS']]))
		mean_delay.append((i,sum(df_tmp['DelayS'])/df_tmp.shape[0]))
	return reduce_data, mean_delay


def display_delay(reduce_data,mean_delay):
	num = len(mean_delay)
	df = pd.DataFrame(data=mean_delay, columns=['Node','Mean Delay'])

	for i in range(num):
		plt.figure(i)
		plt.plot(reduce_data[i][1]['Time'],reduce_data[i][1]['DelayS'])
		plt.title(f'Consumer{i}')

	plt.show()
