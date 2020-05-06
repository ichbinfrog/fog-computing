
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

int main(int argc, char* argv[]) {
	CommandLine cmd;
	cmd.Parse(argc, argv);

	PointToPointHelper p2p;
	PointToPointGridHelper grid(3,3, p2p);

	// Set bounding box where nodes will be randomly places (if positions are unspecified)
	// upper left x, upper left y
	// lower right x, lower right y
	grid.BoundingBox(100,100,200,200);

	// Install stack
	ndn::StackHelper stackHelper;
	stackHelper.InstallAll();

	// Best Route strategy
	ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/best-route");

	// Install Global Routing interface on all nodes
	ndn::GlobalRoutingHelper globalHelper;
	globalHelper.InstallAll();

	// Getting containers for the consumer/producer
	Ptr<Node> producer = grid.GetNode(1,1);
	NodeContainer consumerNodes;
	consumerNodes.Add(grid.GetNode(0, 0));
	consumerNodes.Add(grid.GetNode(1, 0));

	// Install NDN applications
	std::string prefix = "/prefix";
	ndn::AppHelper appHelper("ns3::ndn::ConsumerCbr");
	appHelper.SetPrefix(prefix);
	appHelper.SetAttribute("Frequency", StringValue("100"));
	appHelper.Install(consumerNodes.Get(0));

	// Demande des contenus selon la distrubtion de Mandelbrot
	ndn::AppHelper mandelbrot("ns3::ndn::ConsumerZipfMandelbrot");
	mandelbrot.SetPrefix(prefix);
	mandelbrot.Install(consumerNodes.Get(1));

	// Création du produceurs
	ndn::AppHelper producerHelper("ns3::ndn::Producer");
	producerHelper.SetPrefix(prefix);
	producerHelper.Install(producer);


	// Add /prefix origins to ndn::GlobalRouter
	globalHelper.AddOrigins(prefix, producer);

	// Calculate and install FIBs
	ndn::GlobalRoutingHelper::CalculateRoutes();

	L2RateTracer::InstallAll("drop-trace.txt", Seconds(0.5));

	Simulator::Stop(Seconds(20.0));

	Simulator::Run();
	Simulator::Destroy();

	return 0;
}
}

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
