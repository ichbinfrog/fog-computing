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
  topoReader.SetFileName ("scratch/wifi/wifi.txt");
  topoReader.Read ();

  // Wifi configuration
  // ------------------
  //
  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211a);
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::ThreeLogDistancePropagationLossModel");
  wifiChannel.AddPropagationLoss ("ns3::NakagamiPropagationLossModel");

  YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default ();
  wifiPhyHelper.SetChannel (wifiChannel.Create ());
  wifiPhyHelper.Set ("TxPowerStart", DoubleValue (5));
  wifiPhyHelper.Set ("TxPowerEnd", DoubleValue (5));

  WifiMacHelper wifiMacHelper;
  wifiMacHelper.SetType ("ns3::AdhocWifiMac");

  Ptr<UniformRandomVariable> randomizer = CreateObject<UniformRandomVariable> ();
  randomizer->SetAttribute ("Min", DoubleValue (10));
  randomizer->SetAttribute ("Max", DoubleValue (100));

  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::RandomBoxPositionAllocator", "X", PointerValue (randomizer),
                                 "Y", PointerValue (randomizer), "Z", PointerValue (randomizer));

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  wifi.Install (wifiPhyHelper, wifiMacHelper, NodeContainer::GetGlobal ());
  mobility.Install (NodeContainer::GetGlobal ());
  // NDN stack configuration
  ndn::StackHelper ndnHelper;

  NodeContainer routers;
  for (uint i = 0; i < 44; i++)
    {
      routers.Add (Names::Find<Node> ("Ndn" + std::to_string (i + 1)));
    }
  ndnHelper.SetOldContentStore ("ns3::ndn::cs::Lru", "MaxSize", "2000");
  ndnHelper.Install (routers);

  NodeContainer consumerNodes;
  for (uint i = 1; i <= 5; i++)
    {
      consumerNodes.Add (Names::Find<Node> ("Consumer" + std::to_string (i)));
    }
  ndnHelper.SetOldContentStore ("ns3::ndn::cs::Lru", "MaxSize", "2000");
  ndnHelper.Install (consumerNodes);

  NodeContainer fogNodes;
  for (uint i = 1; i <= 5; i++)
    {
      fogNodes.Add (Names::Find<Node> ("Fog" + std::to_string (i)));
    }
  ndnHelper.SetOldContentStore ("ns3::ndn::cs::Lru", "MaxSize", "2000");
  ndnHelper.Install (fogNodes);

  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll ();

  // ndn::StrategyChoiceHelper::InstallAll("/prefix", "/localhost/nfd/strategy/best-route");
  ndn::AppHelper producerHelper ("ns3::ndn::Producer");

  for (auto prd : fogNodes)
    {
      ndnGlobalRoutingHelper.AddOrigins ("/root", prd);
      producerHelper.SetPrefix ("/root");
      producerHelper.SetAttribute ("PayloadSize", StringValue ("1024"));
      producerHelper.Install (prd);
    }

  ndn::AppHelper consumerHelper ("ns3::ndn::ConsumerCbr");
  for (auto csm : consumerNodes)
    {
      consumerHelper.SetPrefix ("/root");
      consumerHelper.SetAttribute ("Frequency", StringValue ("10"));
      ApplicationContainer app = consumerHelper.Install (csm);
      app.Start (Seconds (1));
    }

  ndnGlobalRoutingHelper.CalculateRoutes ();

  Simulator::Stop (Seconds (50.0));

  ndn::AppDelayTracer::InstallAll ("benchmark/out/app-delays-trace.txt");
  ndn::CsTracer::Install (fogNodes, "benchmark/out/cs-trace.txt", Seconds (1));

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
