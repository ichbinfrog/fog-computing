#!/usr/bin/env python3
"""
Module implémentant des fonctions de lectures abstraites de bases
des fichiers résultats des Tracers NDN
"""
import pandas as pd
import os


class Reader:
    """Classe lecteur d'une suite de fichier ayant le même séparateur et le même mode de gestion de header"""

    def __init__(self, files, sep="\s+", header="infer"):
        self.data_ = {}
        self.sep_ = sep
        self.header_ = header
        self.files_ = files

    """Lis la liste de fichier et le stocke dans self.data_
    
    Si join = True, la fonction tente de joindre les dataframe en un seul dataframe commun
    qui garde dans sa colonne 'source' le nom de son fichier d'origine (sans chemin)

    Si join = False, stocke un dictionnaire de dataframe indexé par le nom du fichier   
    """

    def read(self, join=True):
        for file in self.files_:
            self.data_[file] = pd.read_csv(file, sep=self.sep_, header=self.header_)
            if join:
                self.data_[file]["source"] = os.path.basename(file)

        if join:
            self.data_ = pd.concat([df for _, df in self.data_.items()])
