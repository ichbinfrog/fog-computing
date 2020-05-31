#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/point-to-point-module.h"

namespace ns3 {

    void FullNetwork(NodeContainer& nodes) {
        PointToPointHelper p2p;
        for (uint i = 0; i < nodes.size(); i++) {
            for (uint j = i + 1; j < nodes.size(); j++) {
                if (i != j) {
                    p2p.Install(nodes.Get(i), nodes.Get(j));
                }
            }
        }
    }

    int main(int argc, char *argv[]) {
        CommandLine cmd;
        cmd.Parse(argc, argv);

		ndn::StackHelper ndnHelper;
        int core_size = 4;

        // Fog network core
        NodeContainer fog;
        fog.Create(core_size);
        // Buffer ndn nodes with caching
        NodeContainer buffer;
        buffer.Create(core_size * 2);                

        // Frontier nodes without caching
        NodeContainer frontier;
        frontier.Create(core_size * 4);
        NodeContainer consumer;
        consumer.Create(core_size * 8);

        PointToPointHelper p2p;
        Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("10Mbps"));
        Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
        Config::SetDefault("ns3::QueueBase::MaxSize", StringValue("200p"));
        p2p.Install(fog.Get(0), fog.Get(1));
        p2p.Install(fog.Get(0), fog.Get(2));
        p2p.Install(fog.Get(0), fog.Get(3));
        p2p.Install(fog.Get(1), fog.Get(2));
        p2p.Install(fog.Get(1), fog.Get(3));
        p2p.Install(fog.Get(2), fog.Get(3));

        Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
        Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("20ms"));
        Config::SetDefault("ns3::QueueBase::MaxSize", StringValue("20p"));
        for (uint i = 0; i < buffer.size(); i++) {
            p2p.Install(fog.Get((int)i / 2), buffer.Get(i));
        }

        for (uint i = 0; i < frontier.size(); i++) {
            p2p.Install(buffer.Get((int)i / 2), frontier.Get(i));
        }

        Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("0.2Mbps"));
        Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("15ms"));
        Config::SetDefault("ns3::QueueBase::MaxSize", StringValue("20p"));
        for (uint i = 0; i < consumer.size(); i++) {
            p2p.Install(frontier.Get((int)i / 2), consumer.Get(i));
        }

		ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1000");
        ndnHelper.Install(fog);
		ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "2000");
        ndnHelper.Install(buffer);

		ndnHelper.SetOldContentStore("ns3::ndn::cs::Nocache");
        ndnHelper.Install(frontier);
        ndnHelper.Install(consumer);

        // GlobalRoutingHelper installation
        ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
		ndnGlobalRoutingHelper.InstallAll();

        // Application installation
		ndn::AppHelper producerHelper("ns3::ndn::Producer");
        for (auto prod : fog){
			ndnGlobalRoutingHelper.AddOrigins("/root", prod);
			producerHelper.SetPrefix("/root");
			producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
			producerHelper.Install(prod);
		}

		ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
		for(auto csm : consumer){
			consumerHelper.SetPrefix("/root");
			consumerHelper.SetAttribute("Frequency", StringValue("5"));
			ApplicationContainer app = consumerHelper.Install(csm);
			app.Start(Seconds(1));
		}

		ndnGlobalRoutingHelper.CalculateRoutes();

		Simulator::Stop(Seconds(50.0));

		ndn::AppDelayTracer::InstallAll("benchmark/out/app-delays-trace.txt");
		ndn::CsTracer::InstallAll("benchmark/out/cs-trace.txt", Seconds(1));
		
		Simulator::Run();
		Simulator::Destroy();

		return 0;
    }

}

int main(int argc, char *argv[]) {
    return ns3::main(argc, argv);
}