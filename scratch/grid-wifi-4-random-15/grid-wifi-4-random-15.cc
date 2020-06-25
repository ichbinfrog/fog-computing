#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"

namespace ns3 {
int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  AnnotatedTopologyReader topoReader ("", 1);
  topoReader.SetFileName ("scratch/grid-wifi-4.txt");
  topoReader.Read ();

  // Wifi configuration
  // ------------------
  //
  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211a);
  YansWifiChannelHelper wifiChannel;
  // Set Wifi loss propagation rules
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::ThreeLogDistancePropagationLossModel");
  wifiChannel.AddPropagationLoss ("ns3::NakagamiPropagationLossModel");

  YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default ();
  wifiPhyHelper.SetChannel (wifiChannel.Create ());
  wifiPhyHelper.Set ("TxPowerStart", DoubleValue (5));
  wifiPhyHelper.Set ("TxPowerEnd", DoubleValue (5));

  WifiMacHelper wifiMacHelper;
  // AdhocWifi installations
  wifiMacHelper.SetType ("ns3::AdhocWifiMac");

  // Sets static wifi node
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  // For moving nodes, the following code and mobility model can be uncommented
  // Ptr<UniformRandomVariable> randomizer = CreateObject<UniformRandomVariable> ();
  // randomizer->SetAttribute ("Min", DoubleValue (10));
  // randomizer->SetAttribute ("Max", DoubleValue (100));

  // mobility.SetPositionAllocator ("ns3::RandomBoxPositionAllocator", "X", PointerValue (randomizer),
  //                                "Y", PointerValue (randomizer), "Z", PointerValue (randomizer));

  // Installs wifi on the global nodecontainer == all nodes declared in the file
  wifi.Install (wifiPhyHelper, wifiMacHelper, NodeContainer::GetGlobal ());  

  // NDN stack configuration
  // Adds fog router to node container
  NodeContainer fog;
  for (uint i = 1; i <= 4; i++)
  {
    fog.Add (Names::Find<Node> ("fog" + std::to_string (i)));
  }
  // Adds buffer nodes to node container
  NodeContainer buffer;
  for (uint i = 1; i <= 12; i++)
  {
    buffer.Add (Names::Find<Node> ("buf1" + std::to_string (i)));
  }
  for (uint i = 1; i <= 20; i++)
  {
    buffer.Add (Names::Find<Node> ("buf0" + std::to_string (i)));
  }
  // Adds frontier nodes to node container
  NodeContainer frontier;
  for (uint i = 1; i <= 28; i++)
  {
    frontier.Add (Names::Find<Node> ("frt" + std::to_string (i)));
  }
  // Adds consumer nodes to node container
  NodeContainer consumer;
  for (uint i = 1; i <= 36; i++)
  {
    consumer.Add (Names::Find<Node> ("csm" + std::to_string (i)));
  }

  ndn::StackHelper ndnHelper;
  ndnHelper.SetOldContentStore ("ns3::ndn::cs::Fifo", "MaxSize", "15");
  ndnHelper.Install (fog);
  ndnHelper.Install (buffer);
  ndnHelper.Install (frontier);

  ndnHelper.SetOldContentStore ("ns3::ndn::cs::Nocache");
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
      consumerHelper.SetAttribute ("Frequency", StringValue ("20"));
      ApplicationContainer app = consumerHelper.Install (csm);
      i++;
      // Add small delay between app start
			app.Start(Seconds(1 + i * 1));
    }

  ndnGlobalRoutingHelper.CalculateRoutes ();

  // Stops simulation after 40 seconds
  Simulator::Stop (Seconds (5.0));

  // Installs tracers
  ndn::AppDelayTracer::InstallAll ("benchmark/out/app_grid_4layers_random_15.txt");
  ndn::CsTracer::InstallAll ("benchmark/out/cs_grid_4layers_random_15.txt", Seconds (1));

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
