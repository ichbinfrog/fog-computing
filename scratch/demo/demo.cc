#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

#include <iostream>
#include <typeinfo>


namespace ns3{
	int main(int argc, char *argv[])
	{

		CommandLine cmd;
		cmd.Parse(argc, argv);

		AnnotatedTopologyReader topoReader("", 1);
		topoReader.SetFileName("scratch/demo/demo.txt");
		topoReader.Read();

		ndn::StackHelper ndnHelper;

		NodeContainer routers;
		for(uint i = 0; i < 44; i++) {
			routers.Add(Names::Find<Node>("Ndn"+std::to_string(i+1)));
		}
		ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "2000");
		ndnHelper.Install(routers);
		
		NodeContainer consumerNodes;
		for (uint i = 1; i <= 5; i++) {
			consumerNodes.Add(Names::Find<Node>("Consumer"+std::to_string(i)));
		}
		ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "2000");
		ndnHelper.Install(consumerNodes);

		NodeContainer fogNodes;
		for (uint i = 1; i <= 5; i++) {
			fogNodes.Add(Names::Find<Node>("Fog"+std::to_string(i)));
		}
		ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "2000");
		ndnHelper.Install(fogNodes);

		ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
		ndnGlobalRoutingHelper.InstallAll();

		// ndn::StrategyChoiceHelper::InstallAll("/prefix", "/localhost/nfd/strategy/best-route");
		ndn::AppHelper producerHelper("ns3::ndn::Producer");

		for (auto prd : fogNodes){
			ndnGlobalRoutingHelper.AddOrigins("/root", prd);
			producerHelper.SetPrefix("/root");
			producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
			producerHelper.Install(prd);
		}

		ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
		for(auto csm : consumerNodes){
			consumerHelper.SetPrefix("/root");
			consumerHelper.SetAttribute("Frequency", StringValue("10"));
			ApplicationContainer app = consumerHelper.Install(csm);
			app.Start(Seconds(1));
		}

		ndnGlobalRoutingHelper.CalculateRoutes();		

		Simulator::Stop(Seconds(50.0));

		ndn::AppDelayTracer::InstallAll("benchmark/out/app-delays-trace.txt");
		ndn::CsTracer::Install(fogNodes, "benchmark/out/cs-trace.txt", Seconds(1));
		
		Simulator::Run();
		Simulator::Destroy();

		return 0;
	}
}

int main(int argc, char* argv[]) {
	return ns3::main(argc,argv);
}
