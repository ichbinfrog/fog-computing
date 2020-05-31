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
		topoReader.SetFileName("/Users/Yann_ldv/Desktop/ndnSIM/ns-3/scratch/demo/demo.txt");
		topoReader.Read();

		ndn::StackHelper ndnHelper;
		//ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "15");
		//ndnHelper.InstallAll();

		ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "15");
		for(int i=0;i<44;i++) {
			Ptr<Node> node = Names::Find<Node>("Ndn"+std::to_string(i+1));
			ndnHelper.Install(node);
		}
		ndnHelper.Install(Names::Find<Node>("Consumer1"));
		ndnHelper.Install(Names::Find<Node>("Consumer2"));
		ndnHelper.Install(Names::Find<Node>("Consumer3"));
		ndnHelper.Install(Names::Find<Node>("Consumer4"));
		ndnHelper.Install(Names::Find<Node>("Consumer5"));


		ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "30");
		ndnHelper.Install(Names::Find<Node>("Fog1"));
		ndnHelper.Install(Names::Find<Node>("Fog2"));
		ndnHelper.Install(Names::Find<Node>("Fog3"));
		ndnHelper.Install(Names::Find<Node>("Fog4"));
		ndnHelper.Install(Names::Find<Node>("Fog5"));


		ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/best-route");


		ndn::AppHelper producerHelper("ns3::ndn::Producer");

		Ptr<Node> producers[49] = {
			Names::Find<Node>("Ndn1"),
			Names::Find<Node>("Ndn2"),
			Names::Find<Node>("Ndn3"),
			Names::Find<Node>("Ndn4"),
			Names::Find<Node>("Ndn5"),
			Names::Find<Node>("Ndn6"),
			Names::Find<Node>("Ndn7"),
			Names::Find<Node>("Ndn8"),
			Names::Find<Node>("Ndn9"),
			Names::Find<Node>("Ndn10"),
			Names::Find<Node>("Ndn11"),
			Names::Find<Node>("Ndn12"),
			Names::Find<Node>("Ndn13"),
			Names::Find<Node>("Ndn14"),
			Names::Find<Node>("Ndn15"),
			Names::Find<Node>("Ndn16"),
			Names::Find<Node>("Ndn17"),
			Names::Find<Node>("Ndn18"),
			Names::Find<Node>("Ndn19"),
			Names::Find<Node>("Ndn20"),
			Names::Find<Node>("Ndn21"),
			Names::Find<Node>("Ndn22"),
			Names::Find<Node>("Ndn23"),
			Names::Find<Node>("Ndn24"),
			Names::Find<Node>("Ndn25"),
			Names::Find<Node>("Ndn26"),
			Names::Find<Node>("Ndn27"),
			Names::Find<Node>("Ndn28"),
			Names::Find<Node>("Ndn29"),
			Names::Find<Node>("Ndn30"),
			Names::Find<Node>("Ndn31"),
			Names::Find<Node>("Ndn32"),
			Names::Find<Node>("Ndn33"),
			Names::Find<Node>("Ndn34"),
			Names::Find<Node>("Ndn35"),
			Names::Find<Node>("Ndn36"),
			Names::Find<Node>("Ndn37"),
			Names::Find<Node>("Ndn38"),
			Names::Find<Node>("Ndn39"),
			Names::Find<Node>("Ndn40"),
			Names::Find<Node>("Ndn41"),
			Names::Find<Node>("Ndn42"),
			Names::Find<Node>("Ndn43"),
			Names::Find<Node>("Ndn44"),
			Names::Find<Node>("Fog1"),
			Names::Find<Node>("Fog2"),
			Names::Find<Node>("Fog3"),
			Names::Find<Node>("Fog4"),
			Names::Find<Node>("Fog5")
		};


		for (auto prd : producers){
			producerHelper.SetPrefix("/root/" + Names::FindName(prd));
			producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
			producerHelper.Install(prd);
		}


		ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");

		Ptr<Node> consumers[5] = {
			Names::Find<Node>("Consumer1"),
			Names::Find<Node>("Consumer2"),
			Names::Find<Node>("Consumer3"),
			Names::Find<Node>("Consumer4"),
			Names::Find<Node>("Consumer5")
		};


		for(auto csm : consumers){
			consumerHelper.SetPrefix("/root/" + Names::FindName(csm));
			consumerHelper.SetAttribute("Frequency", StringValue("60"));
			consumerHelper.Install(csm);
		}

		ndn::GlobalRoutingHelper::CalculateRoutes();		
		//ndn::L3RateTracer::InstallAll("test-trace.txt", Seconds(0.5));
		ndn::AppDelayTracer::InstallAll("app-delays-trace.txt");
		ndn::CsTracer::InstallAll("cs-trace.txt", Seconds(1));

		Simulator::Stop(Seconds(50.0));
		Simulator::Run();
		Simulator::Destroy();

		return 0;
	}
}

int main(int argc, char* argv[]) {
	return ns3::main(argc,argv);
}