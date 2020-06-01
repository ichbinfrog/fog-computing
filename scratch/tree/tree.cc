#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/point-to-point-module.h"

namespace ns3 {

// Creates a full p2p network from an array of nodes
void
FullNetwork (NodeContainer &nodes)
{
  PointToPointHelper p2p;
  for (uint i = 0; i < nodes.size (); i++)
    {
      for (uint j = i + 1; j < nodes.size (); j++)
        {
          if (i != j)
            {
              p2p.Install (nodes.Get (i), nodes.Get (j));
            }
        }
    }
}

int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);

  // Node Creation
  // -------------
  // CORE_SIZE : number of full network central nodes (fog)
  // BUFFER_MULT : number of buffer NDN with cache nodes per fog node
  // FRONTIER_MULT : number of frontier NDN non cache nodes per buffer node
  // CONSUMER_MULT : number of consumer per frontier nodes
  //
  ndn::StackHelper ndnHelper;
  int CORE_SIZE = 4;
  int BUFFER_MULT = 4;
  int FRONTIER_MULT = 4;
  int CONSUMER_MULT = 4;

  NodeContainer fog;
  fog.Create (CORE_SIZE);
  NodeContainer buffer;
  buffer.Create (CORE_SIZE * BUFFER_MULT);
  NodeContainer frontier;
  frontier.Create (CORE_SIZE * BUFFER_MULT * FRONTIER_MULT);
  NodeContainer consumer;
  consumer.Create (CORE_SIZE * BUFFER_MULT * FRONTIER_MULT * CONSUMER_MULT);

  // P2P Network connection
  // ----------------------
  // - Creates full fog network
  // - Connects the outer layer to the inner layers
  PointToPointHelper p2p;
  Config::SetDefault ("ns3::PointToPointNetDevice::DataRate", StringValue ("10Mbps"));
  Config::SetDefault ("ns3::PointToPointChannel::Delay", StringValue ("10ms"));
  Config::SetDefault ("ns3::QueueBase::MaxSize", StringValue ("200p"));
  FullNetwork (fog);

  Config::SetDefault ("ns3::PointToPointNetDevice::DataRate", StringValue ("1Mbps"));
  Config::SetDefault ("ns3::PointToPointChannel::Delay", StringValue ("20ms"));
  Config::SetDefault ("ns3::QueueBase::MaxSize", StringValue ("20p"));
  for (uint i = 0; i < buffer.size (); i++)
    {
      p2p.Install (fog.Get ((int) i / BUFFER_MULT), buffer.Get (i));
    }

  for (uint i = 0; i < frontier.size (); i++)
    {
      p2p.Install (buffer.Get ((int) i / FRONTIER_MULT), frontier.Get (i));
    }

  Config::SetDefault ("ns3::PointToPointNetDevice::DataRate", StringValue ("0.2Mbps"));
  Config::SetDefault ("ns3::PointToPointChannel::Delay", StringValue ("15ms"));
  Config::SetDefault ("ns3::QueueBase::MaxSize", StringValue ("20p"));
  for (uint i = 0; i < consumer.size (); i++)
    {
      p2p.Install (frontier.Get ((int) i / CONSUMER_MULT), consumer.Get (i));
    }

  // Sets up cache
  // -------------
  // - fog & buffer : LRU of MaxSize 2000 packets
  // - frontier & buffer : no cache
  ndnHelper.SetOldContentStore ("ns3::ndn::cs::Lru", "MaxSize", "2000");
  ndnHelper.Install (fog);
  ndnHelper.Install (buffer);

  ndnHelper.SetOldContentStore ("ns3::ndn::cs::Nocache");
  ndnHelper.Install (frontier);
  ndnHelper.Install (consumer);

  // GlobalRoutingHelper installation
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll ();

  // Producer installation
  // ------------------------
  // All producer produce at prefix /root and with a 1024 bytes PayloadSize
  ndn::AppHelper producerHelper ("ns3::ndn::Producer");
  for (auto prod : fog)
    {
      ndnGlobalRoutingHelper.AddOrigins ("/root", prod);
      producerHelper.SetPrefix ("/root");
      producerHelper.SetAttribute ("PayloadSize", StringValue ("1024"));
      producerHelper.Install (prod);
    }

  // Consumer Installation
  // ---------------------
  // All consumers consume at /root with frequency 1 request per 40s
  ndn::AppHelper consumerHelper ("ns3::ndn::ConsumerCbr");
  int i = 0;
  for (auto csm : consumer)
    {
      consumerHelper.SetPrefix ("/root");
      consumerHelper.SetAttribute ("Frequency", StringValue ("40"));
      ApplicationContainer app = consumerHelper.Install (csm);
      app.Start (Seconds (1 + i * 0.1));
    }

  ndnGlobalRoutingHelper.CalculateRoutes ();

  Simulator::Stop (Seconds (60.0));

  ndn::AppDelayTracer::InstallAll ("benchmark/out/app-delays-trace.txt");
  ndn::CsTracer::InstallAll ("benchmark/out/cs-trace.txt", Seconds (1));

  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}

} // namespace ns3

int
main (int argc, char *argv[])
{
  return ns3::main (argc, argv);
}