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
		ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "15");

		NodeContainer routers;
		for(uint i = 0; i < 44; i++) {
			routers.Add(Names::Find<Node>("Ndn"+std::to_string(i+1)));
		}
		ndnHelper.Install(routers);
		
		NodeContainer consumerNodes;
		for (uint i = 1; i <= 5; i++) {
			consumerNodes.Add(Names::Find<Node>("Consumer"+std::to_string(i)));
		}
		ndnHelper.Install(consumerNodes);

		NodeContainer fogNodes;
		for (uint i = 1; i <= 5; i++) {
			fogNodes.Add(Names::Find<Node>("Fog"+std::to_string(i)));
		}

		ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "30");
		ndnHelper.Install(fogNodes);

		ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/best-route");
		ndn::AppHelper producerHelper("ns3::ndn::Producer");
		ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
		ndnGlobalRoutingHelper.InstallAll();

		for (auto prd : fogNodes){
			ndnGlobalRoutingHelper.AddOrigins("/root", prd);
			producerHelper.SetPrefix("/root");
			producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
			producerHelper.Install(prd);
		}

		ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
		for(auto csm : consumerNodes){
			consumerHelper.SetPrefix("/root");
			consumerHelper.SetAttribute("Frequency", StringValue("60"));
			consumerHelper.Install(csm);
		}

		ndn::GlobalRoutingHelper::CalculateRoutes();		
		ndn::AppDelayTracer::InstallAll("benchmark/out/app-delays-trace.txt");
		ndn::CsTracer::InstallAll("benchmark/out/cs-trace.txt", Seconds(0.5));

		Simulator::Stop(Seconds(50.0));
		Simulator::Run();
		Simulator::Destroy();

		return 0;
	}
}

int main(int argc, char* argv[]) {
	return ns3::main(argc,argv);
}
