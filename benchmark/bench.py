#!/usr/bin/env python3
"""
Module implémentant des fonctions de lectures et de graph
des fichiers résultats des Tracers NDN
"""
import seaborn as sns
import matplotlib.pyplot as plt
from reader import Reader

def split_cache(x):
    if "lru" in x:
        return "lru"
    elif "lfu" in x:
        return "lfu"
    elif "random" in x:
        return "random"
    elif "reference" in x:
        return "reference"
    
def split_capacity(x):
    if "15" in x:
        return "15"
    elif "2000" in x:
        return "2000"
    
def split_layers(x):
    if "3layers" in x:
        return "3"
    else:
        return "4"
    
def split_type(x):
    if "frontier" in x:
        return "frontier"
    else:
        return "everywhere"

class ContentStore(Reader):
    """Implémentation du Reader pour les CS Tracer"""

    def __init__(self, files, sep="\s+", header="infer"):
        super().__init__(files, sep, header)
        super().read(join=True)

        self.data_["Type"] = (
            self.data_["Type"]
            .str.replace("Cache", "")
            .replace("Hits", "Hit")
            .replace("Misses", "Miss")
        )
        self.data_["NodeType"] = self.data_["Node"].str.replace("\d+", "")
        self.data_["NodeNumber"] = self.data_["Node"].str.replace("[a-zA-Z]+", "")
        self.data_["Cache"] = self.data_["source"].apply((lambda x: split_cache(x)))
        self.data_["CacheCapacity"] = self.data_["source"].apply((lambda x: split_capacity(x)))
        self.data_["CacheStrategy"] = self.data_["source"].apply((lambda x: split_type(x)))

class AppDelay(Reader):
    """Implémentation du Reader pour les AppDelay Tracer"""

    def __init__(self, files, sep="\s+", header="infer"):
        super().__init__(files, sep, header)
        super().read(join=True)
        
        self.data_["NodeType"] = self.data_["Node"].str.replace("\d+", "")
        self.data_["Type"] = (
            self.data_["Type"]
            .str.replace("Delay", "")
        )        
        self.data_["NodeType"] = self.data_["Node"].str.replace("\d+", "")
        self.data_["NodeNumber"] = self.data_["Node"].str.replace("[a-zA-Z]+", "")
        self.data_["Cache"] = self.data_["source"].apply((lambda x: split_cache(x)))
        self.data_["CacheCapacity"] = self.data_["source"].apply((lambda x: split_capacity(x)))
        self.data_["CacheStrategy"] = self.data_["source"].apply((lambda x: split_type(x)))
