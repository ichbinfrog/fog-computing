#!/usr/bin/env python3
"""
Module implémentant des fonctions de lectures et de graph
des fichiers résultats des Tracers NDN
"""
import seaborn as sns
import matplotlib.pyplot as plt
from reader import Reader


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