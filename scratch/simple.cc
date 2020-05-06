
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

/*
 * 			[ Consommateur ] <-->  [ Routeur ] <-> [ Routeur ] <-> [ Routeur ] <-> [ Produceur ]
 */

int main(int argc, char *argv[]) {
	CommandLine cmd;
	cmd.Parse(argc, argv);

	// Création des noeuds
	NodeContainer nodes;
	nodes.Create(5);

	// Création des liens p2p entre noeuds
	PointToPointHelper p2p;
	for (int i = 0; i < 4; i++) {
		p2p.Install(nodes.Get(i), nodes.Get(i + 1));
	}

	// Installation du stack NDN sur chacun des noeuds
	ndn::StackHelper ndnHelper;
	ndnHelper.SetDefaultRoutes(true);
	ndnHelper.InstallAll();

	// Définition de la stratégie de forwarding
	// /localhost/nfd/strategy/best-route : Best Route stratégie
	//	 		- Forward l'interest packet vers le routeur upstream avec le coût de routage minimal
	// /localhost/nfd/strategy/multicast : Forward à tous les routeurs upstream
	ndn::StrategyChoiceHelper::InstallAll("/prefix",
			"/localhost/nfd/strategy/multicast");

	// Simulation du consommateur
	// ns3::ndn::ConsumerCbr :
	//		Générateur d'interest packet selon une distribution donnée à intervalle régulier/ aléatoire
	ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");

	// 2 Interest / second
	consumerHelper.SetAttribute("Frequency", DoubleValue(2.0));

	// exponential
	// uniform
	// none
	consumerHelper.SetAttribute("Randomize", StringValue("exponential"));

	// L'attribut Frequency == Fréquence à laquelle un interest packet doit être envoyé
	// Si Randomize = None => intervalle constante
	// Sinon Intervalle selon la loi de distribution choisie <= Frequency

	// Installation de l'application consommer
	auto apps = consumerHelper.Install(nodes.Get(0));
	apps.Stop(Seconds(10.0)); // Arrête le consommateur après 10s

	// Simulation du produceur
	// Producer : application qui répond avec un DataPacket d'une taille donnée au Interest Packet
	ndn::AppHelper producerHelper("ns3::ndn::Producer");

	// Répond qu'aux requêtes commençant par /prefix
	producerHelper.SetPrefix("/prefix");
	producerHelper.SetAttribute("PayloadSize", StringValue("1024")); // 1024 bit
	producerHelper.Install(nodes.Get(4));

	Simulator::Stop(Seconds(20.0));

	Simulator::Run();
	Simulator::Destroy();

	return 0;
}
}

int main(int argc, char *argv[]) {
	return ns3::main(argc, argv);
}
