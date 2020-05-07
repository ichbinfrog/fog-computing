#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

int main(int argc, char *argv[]) {
	CommandLine cmd;
	cmd.Parse(argc, argv);

	AnnotatedTopologyReader topologyReader("", 1);
	topologyReader.SetFileName(
			"/home/ichbinfrog/Desktop/TX/ICN/fog/scratch/two-bottle-neck.txt");
	topologyReader.Read();

	// Install NDN stack on all nodes
	ndn::StackHelper ndnHelper;
	ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1"); // ! Attention ! If set to 0, then MaxSize is infinite
	ndnHelper.InstallAll();

	ndn::StrategyChoiceHelper::InstallAll("/",
			"/localhost/nfd/strategy/best-route");

	// Getting containers for the consumer/producer
	Ptr<Node> consumers[4] = { Names::Find<Node>("c1"), Names::Find<Node>("c2"),
			Names::Find<Node>("c3"), Names::Find<Node>("c4") };
	Ptr<Node> producers[4] = { Names::Find<Node>("p1"), Names::Find<Node>("p2"),
			Names::Find<Node>("p3"), Names::Find<Node>("p4") };

	if (consumers[0] == 0 || consumers[1] == 0 || consumers[2] == 0
			|| consumers[3] == 0 || producers[0] == 0 || producers[1] == 0
			|| producers[2] == 0 || producers[3] == 0) {
		NS_FATAL_ERROR(
				"Error in topology: one nodes c1, c2, c3, c4, p1, p2, p3, or p4 is missing");
	}

	for (int i = 0; i < 4; i++) {
		std::string prefix = "/data/" + Names::FindName(producers[i]);

		/////////////////////////////////////////////////////////////////////////////////
		// install consumer app on consumer node c_i to request data from producer p_i //
		/////////////////////////////////////////////////////////////////////////////////

		ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
		consumerHelper.SetAttribute("Frequency", StringValue("10")); // 100 interests a second

		consumerHelper.SetPrefix(prefix);
		ApplicationContainer consumer = consumerHelper.Install(consumers[i]);
		consumer.Start(Seconds(i));     // start consumers at 0s, 1s, 2s, 3s
		consumer.Stop(Seconds(19 - i)); // stop consumers at 19s, 18s, 17s, 16s

		///////////////////////////////////////////////
		// install producer app on producer node p_i //
		///////////////////////////////////////////////

		ndn::AppHelper producerHelper("ns3::ndn::Producer");
		producerHelper.SetAttribute("PayloadSize", StringValue("1024"));

		// install producer that will satisfy Interests in /dst1 namespace
		producerHelper.SetPrefix(prefix);
		ApplicationContainer producer = producerHelper.Install(producers[i]);
		// when Start/Stop time is not specified, the application is running throughout the simulation
	}

	// Manually configure FIB routes
	ndn::FibHelper::AddRoute("c1", "/data", "n1", 1); // link to n1
	ndn::FibHelper::AddRoute("c2", "/data", "n1", 1); // link to n1
	ndn::FibHelper::AddRoute("c3", "/data", "n1", 1); // link to n1
	ndn::FibHelper::AddRoute("c4", "/data", "n1", 1); // link to n1

	ndn::FibHelper::AddRoute("n1", "/data", "n2", 1);  // link to n2
	ndn::FibHelper::AddRoute("n1", "/data", "n12", 2); // link to n12

	ndn::FibHelper::AddRoute("n12", "/data", "n2", 1); // link to n2

	ndn::FibHelper::AddRoute("n2", "/data/p1", "p1", 1); // link to p1
	ndn::FibHelper::AddRoute("n2", "/data/p2", "p2", 1); // link to p2
	ndn::FibHelper::AddRoute("n2", "/data/p3", "p3", 1); // link to p3
	ndn::FibHelper::AddRoute("n2", "/data/p4", "p4", 1); // link to p4

	// Schedule simulation time and run the simulation
	Simulator::Stop(Seconds(20.0));
	Simulator::Run();
	Simulator::Destroy();

	return 0;
}

} // namespace ns3

int main(int argc, char *argv[]) {
	return ns3::main(argc, argv);
}
