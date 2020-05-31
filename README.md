# Tutoriel installation ndn-SIM


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

Les pybinding (binding python pour ndn-SIM) et la visualisation utilise python2 (:warning: en cours de transition vers python3 et une majorité des packages ne sont plus disponible sur apt) et nécessite les librairies suivantes:
```sh
sudo apt install python-dev \
                python-pygraphviz \
                python-kiwi \
                python-gnome2 \
                ipython \
                libcairo2-dev \
                python3-gi \
                libgirepository1.0-dev \
                python-gi \
                python-gi-cairo \
                gir1.2-gtk-3.0 \
                gir1.2-goocanvas-2.0 \
                python-pip
                
sudo pip install pygraphviz \
                pycairo \
                PyGObject \
                pygccxml
```
L'utilisation de `sudo` est nécessaire car ndn-SIM utilise certaines librairies statiques/header du système qui nécessite l'accès en tant que super-utilisateur.
![](https://i.imgur.com/57wfdDZ.png)



## Organisation du repo

```shell
ns-3/
├── AUTHORS            # Auteurs de base de ndnSIM
├── bindings/          # Binding python
├── build/             # Dossier résultat de la compilation
├── CHANGES.html
├── contrib/            
├── Debug/    
├── doc/               # Docstring généré par doxygen
├── LICENSE
├── Makefile           # Makefile qui appèle waf
├── README            
├── RELEASE_NOTES
├── scratch/           # Dossier où il faut mettre les modules/scénarios
├── src/               # Code source (contenant le module ndn-SIM)
├── test.py
├── testpy.supp
├── utils
├── utils.py
├── VERSION
├── waf                 # utilitaire de build
├── waf.bat
├── waf-tools
├── wscript
└── wutils.py

```


## Mise en place d'Eclipse C/C++

Eclipse C/C++ permet de faire du syntax highlighting pour NDN-SIM sans configuration d'un cmake/qmake comme dans d'autres IDE.

- Installer Eclipse C/C++ ([source](https://www.eclipse.org/downloads/packages/release/helios/sr2/eclipse-ide-cc-developers))
- Créer un workspace dans le répertoire au dessus de ns-3
- `Import > General > File System > ns-3`

Il est aussi possible d'ajouter une étape de build et run directe dans Eclipse en faisant:

- `Run > External Tools > External Tools Configuration`

![](https://i.imgur.com/Kmo9mTw.png)

- Clique droit, `New configuration`

![](https://i.imgur.com/jKuuSLI.png)

- `Location : ${workspace_loc}/ns-3/waf` (lieu du binaire waf à exécuter par rapport à la racine du workspace Eclipse)
- `Working Directory : ${workspace_loc}/ns-3` (lieu du dossier ns-3, correspond aussi au lieu d'export des fichiers de métriques collectés)
- `Arguments : --run=${string_prompt}` (va lancer un prompt qui demande le nom du module à faire tourner)

---
## Création de scénario

Il est possible de créer des scénarios dans deux dossiers:
- `examples` : ce dossier contient par défaut beaucoup d'exemple de ns-3 et de ndnSIM. Comme lors d'un build **TOUT** les scénarios sont recompilés, il n'est pas recommandé d'utiliser `examples` ou de supprimer les scénarios par défaut.
- `scratch` : contient un exemple d'une application. La compilation dans ce dossier est unitaire, seul le scénario appelé est compilé.

Etapes:

- Création d'un scénario test: `mkdir scratch/test`
- Ajout du code dans le dossier `scratch/test/` (plusieurs classes, fichiers et dossiers imbriqués possible, la structure d'un projet C++ est acceptée dans ce dossier lors de compilation)
- Compilation du scénario : `./waf --run=test` (pas besoin d'indiquer `scratch`)
- Exécution du scénario : `./build/scratch/test/test`

---
## Utilisation de ndn-SIM

### Lecture d'un fichier de scénario et création de noeuds

Les fichiers de définition de scénarios sont de la forme suivante:

```
router
# node  comment    yPos    xPos
Sensor1  NA        1       1
Sensor2  NA        1       2
Sensor3  NA        1       3
Sensor4  NA        1       4
Sensor5  NA        1       5
Sensor6  NA        1       6
Rtr1   	 NA        2       1
Rtr2     NA        2       2
Rtr3     NA        2       3
Agg 	 NA        3       1


link

#x      y       capacity(kbps)  OSPF    Delay   MaxPackets
Sensor1 Rtr1    1Mbps           1       50ms    20
Sensor2 Rtr1    1Mbps           1       10ms    20
Sensor3 Rtr2    1Mbps           1       100ms   20
Sensor4 Rtr2    1Mbps           1       50ms    20
Sensor5 Rtr3    1Mbps           1       10ms    20
Sensor6 Rtr3    1Mbps           1       100ms   20
Rtr1    Rtr2    10Mbps          1       20ms    200
Rtr1    Rtr3    10Mbps          1       20ms    200
Rtr2    Rtr3    10Mbps          1       20ms    200
Rtr1    Agg     10Mbps          1       20ms    200
Rtr2    Agg     10Mbps          1       20ms    200
Rtr3    Agg     10Mbps          1       20ms    200
```

Dans une première partie, il faut définir les noeuds après le terme `routeur` avec chaque ligne correspondant à un noeud, constitué d':
- un nom unique
- un commentaire 
- des coordonnées `(x,y)` sur la grille qui correspond à sa position dans le réseau.

Dans un deuxième temps, il faut définir les connections entre les différentes noeuds après la balise `link`. Chaque ligne correspond à canal bilaterale caractérisée par:
- `x` et `y` : les noeuds reliés (obligatoire)
- `capacity(kbps)` : capacité du canal  
- `OSPF`: 0 = n'utilise pas l'algorithme de routage OSPF
- `delay`: delai du canal
- `queue`: nombre de paquets en queue max
- `metric` : indique si le canal garde des métriques
 
:warning: L'ordre des propriétés est important dans la définition.

Pour lire le fichier:
```cpp
// Constructeur de l'objet lecteur
// - path : chemin du fichier (relative au workspace/absolu)
// - scale : échelle multiplicatrice des coordonnées définies dans le fichier
ns3::AnnotatedTopologyReader::AnnotatedTopologyReader	(	
        const std::string & path = "",
        double scale = 1.0 
)


// Construction du lecteur
ns3::AnnotatedTopologyReader topoReader(file_name, 1);

// Lis la topologie
topoReader.Read()
```

L'exemple donne donc la topologie suivante:

![](https://i.imgur.com/Zfdl9ix.png)

### Installation de ndn-SIM sur l'ensemble des noeuds

Il est obligatoire d'installer la couche protocolaire ndn-SIM sur l'ensemble des noeuds du réseau grâce au helper `ns3::ndn::StackHelper`.

```cpp
ns3::ndn::StackHelper ndnHelper;
ndnHelper.InstallAll();
ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "15");
```

### Création de consommateur

```cpp
// Consommateur qui envoie des requêtes à intervalle uniforme
ns3::ndn::AppHelper consumerHelper(“ns3::ndn::ConsumerCbr”);

// Consommateur qui envoie des requêtes en Batch à interval régulier
ns3::ndn::AppHelper consumerHelper(“ns3::ndn::ConsumerBatches”);

// Consommateur qui envoie des requêtes en fenêtre glissante
ns3::ndn::AppHelper consumerHelper(“ns3::ndn::ConsumerWindow”);

// Consommateur qui envoie des requêtes selon une distribution de Mandelbrot
ns3::ndn::AppHelper consumerHelper(“ns3::ndn::ConsumerZipfMandelbrot”);

// Préfixe sur lequel le consommateur va demander
consumerHelper.SetPrefix(prefix);

// Indique la fréquence en nombre de transmission par seconde
consumerHelper.SetAttribute(“Frequency”, StringValue(“60”));

// Indique la loi de probabilité à suivre
consumerHelper.SetAttribute(“Randomize”, StringValue(“exponential”);
```

Chaque noeud initialisé par lecture du fichier est vierge, il ne contient aucune application ou interface. Pour installer un consommateur dessus, il suffit de faire:

```cpp
// Recherche du noeud par nom
// Ce nom doit correspondre au nom indiqué dans le fichier
Ptr<Node> consumer = Names::Find<Node>("Sensor1");

// Installation de l'application consumer sur le noeud
consumerHelper.Install(consumer);
```

### Création de produceur

```cpp
// Application qui produit des contenus à intervalle uniforme
ns3::ndn::AppHelper producerHelper(“ns3::ndn::Producer”);

// Préfixe du contenu produit
producerHelper.SetPrefix(prefix);

// Choix de la fréquence de production
producerHelper.SetAttribute(“Frequency”, StringValue(“60”));

// Installation du produceur sur un noeud donné
producerHelper.Install(Ptr<Node>);
```

### Choix de stratégie globale

```cpp
// Installation de la stratégie de routage best route sur l'ensemble
// des noeuds simulés
ndn::StrategyChoiceHelper::InstallAll(
    "/", "/localhost/nfd/strategy/best-route"
);
// /localhost/nfd/strategy/ncc : stratégie CCNx 0.7.2
// /localhost/nfd/strategy/broadcast : stratégie de broadcast
// /localhost/nfd/strategy/client-control : contrôle manuel


// Calcul du routage global
ns3::ndn::GlobalRoutingHelper routingHelper ;
routingHelper.InstallAll();
routingHelper.CalculateRoutes();
```

### Contrôle de la simulation

```cpp
// Arrête le simulateur après 20s
ns3::Simulator::Stop(20.0) ;

// Simule un évènement
//  à la 10ème seconde depuis le début de la simulation
//  d'une interruption de canal
//  entre les noeuds 0 et 1
ns3::Simulator::Schedule(
    Seconds(10.0), 
    ndn::LinkControllerHelper::FaillLink, 
    nodes.Get(0), nodes.Get(1)
);

// Simule un évènement
//  à la 15ème seconde depuis le début de la simulation
//  d'une apparation d'un canal
//  entre les noeuds 0 et 1
ns3::Simulator::Schedule(
    Seconds(15.0), 
    ndn::LinkControllerHelper::UpLink, 
    nodes.Get(0), nodes.Get(1)
);

// Comme ns-3 est un simulateur discret, on crée des évènements
// continus par une suite d'évènement discret:
// 
//     une interruption du canal [0 , 1] à 10s
//     une apparition du canal [0 , 1] à 15s
//     = une interruption du canal [0 , 1] entre 10-15s d'une durée 5s
//


// Lance de simulateur
ns3::Simulator::Run() ;
// Finis la simulation et libère la mémoire une fois qu'elle est finie
ns3::Simulator::Destroy() ;
```


### Collection des métriques

```cpp
// stocke le taux de transfert en bytes 
// et en nombre de paquet tout les 1 secondes sur l’ensemble des noeuds
ns3::ndn::L3RateTracer::InstallAll(file, Seconds(1.0));


// stocke le nombre de bytes et en nombre de paquet perdu 
// par débordement de la queue tout les 1 secondes 
// sur l’ensemble des noeuds
ns3::L2RateTracer::InstallAll(file, Seconds(1.0));


// Délai au niveau de la réponse de l’application 
ns3::ndn::AppDelayTracer::InstallAll(file, Seconds(1.0));  


// Statistique des cache hit et miss
ns3::ndn::CSTracer::InstallAll(file, Seconds(1.0)); 
```