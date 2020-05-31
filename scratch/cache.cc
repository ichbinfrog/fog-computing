#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {
int main(int argc, char *argv[]) {
  CommandLine cmd;
  cmd.Parse(argc, argv);

  AnnotatedTopologyReader topologyReader("", 1);
  topologyReader.SetFileName("scratch/topo-tree.txt");
  topologyReader.Read();

  // Install CCNx stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.SetOldContentStore("ns3::ndn::cs::Stats::Lru", "MaxSize", "100"); // default ContentStore parameters
  ndnHelper.InstallAll();

  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ccnxGlobalRoutingHelper;
  ccnxGlobalRoutingHelper.InstallAll();

  // Getting containers for the consumer/producer
  Ptr<Node> consumers[4] = { Names::Find<Node>("leaf-1"), Names::Find<Node>("leaf-2"),
                             Names::Find<Node>("leaf-3"), Names::Find<Node>("leaf-4") };
  Ptr<Node> producer = Names::Find<Node>("root");

  for(int i = 0; i < 4; i++)
    {
      ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
      consumerHelper.SetAttribute("Frequency", StringValue("10")); // 100 interests a second

      // Each consumer will express the same data /root/<seq-no>
      consumerHelper.SetPrefix("/root");
      ApplicationContainer app = consumerHelper.Install(consumers[i]);
      app.Start(Seconds(0.01 * i));
    }
    
  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));  

  // Register /root prefix with global routing controller and
  // install producer that will satisfy Interests in /root namespace
  ccnxGlobalRoutingHelper.AddOrigins("/root", producer);
  producerHelper.SetPrefix("/root");
  producerHelper.Install(producer);

  // Calculate and install FIBs
  ccnxGlobalRoutingHelper.CalculateRoutes();

  Simulator::Stop(Seconds(20.0));

  ndn::CsTracer::InstallAll("cs-trace.txt", Seconds(1));
  
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int main(int argc, char *argv[]) {
	return ns3::main(argc, argv);
}
