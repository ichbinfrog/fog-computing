
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {
	int main(int argc, char* argv[]) {

		CommandLine cmd;
		cmd.Parse(argc, argv);

		AnnotatedTopologyReader topoReader("", 1);
		topoReader.SetFileName(
				"/home/ichbinfrog/Desktop/TX/ICN/fog/scratch/sensor/sensor.txt"
		);
		topoReader.Read();

		ndn::StackHelper ndnHelper;
		ndnHelper.InstallAll();
		ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "15");
		ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/best-route");

		Ptr<Node> producers[6] = {
				Names::Find<Node>("Sensor1"),
				Names::Find<Node>("Sensor2"),
				Names::Find<Node>("Sensor3"),
				Names::Find<Node>("Sensor4"),
				Names::Find<Node>("Sensor5"),
				Names::Find<Node>("Sensor6"),
		};

		ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
		ndnGlobalRoutingHelper.InstallAll();

		ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
		Ptr<Node> consumer = Names::Find<Node>("Agg");
		if (consumer == 0) {
			NS_FATAL_ERROR(
					"Topology error: unknown consumer"
			);
		}

		ndn::AppHelper producerHelper("ns3::ndn::Producer");
		for (auto producer: producers) {
			if (producer == 0) {
				NS_FATAL_ERROR(
						"Topology error: unknown producer"
				);
			}
			producerHelper.SetPrefix("/root/" + Names::FindName(producer));
			producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
			producerHelper.Install(producer);

			consumerHelper.SetPrefix("/root/" + Names::FindName(producer));
			consumerHelper.SetAttribute("Frequency", StringValue("60"));
			consumerHelper.Install(consumer);
		}
		ndn::GlobalRoutingHelper::CalculateRoutes();
		ndn::L3RateTracer::InstallAll("rate-trace.txt", Seconds(0.5));

		Simulator::Stop(Seconds(20.0));
		Simulator::Run();
		Simulator::Destroy();

		return 0;
	}
}


int main(int argc, char* argv[]) {
	return ns3::main(argc,argv);
}
