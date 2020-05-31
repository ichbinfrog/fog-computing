
# Tutoriel d'installation et de déploiement

## Installation de ndn-SIM

```sh
# Téléchargement
mkdir ndnSIM

cd ndnSIM
git clone https://github.com/named-data-ndnSIM/ns-3-dev.git ns-3 
git clone https://github.com/named-data-ndnSIM/pybindgen.git pybindgengit clone --recursive https://github.com/named-data-ndnSIM/ndnSIM.git ns-3/src/ndnSIM

git submodule update --init
```

```sh
# Compilation de la librairie
cd ns-3

# Options de compilation
# --disable-python : pas de bindings python
# -d optimized : compilation optimisé
# --visualize : permet le module de visualisation (qu'aveec python2)
# --enable-examples : compile les exemples dans examples/
# --enable-static : compilation statique des librairies (UNIX)
sudo ./waf configure -d optimized
```

## Mise en place d'Eclipse C/C++

Eclipse C/C++ permet de faire du syntax highlighting pour NDN-SIM sans configuration d'un cmake/qmake comme dans d'autres IDE.

- Installer Eclipse C/C++ ([source](https://www.eclipse.org/downloads/packages/release/helios/sr2/eclipse-ide-cc-developers))
- Créer un workspace dans le répertoire au dessus de ns-3
- `Import > General > File System > ns-3`

Il est aussi possible d'ajouter une étape de build et run directe dans Eclipse en faisant:

- `Run > External Tools > External Tools Configuration`
- 
