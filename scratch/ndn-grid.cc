//
// Created by ichbinfrog on 06/05/2020.
//

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

int
main(int argc, char* argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);

  AnnotatedTopologyReader topologyReader("", 25);
  topologyReader.SetFileName("scratch/topo-grid-3x3.txt");
  topologyReader.Read();

  ndn::StackHelper ndnHelper;
  ndnHelper.InstallAll();

  ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/best-route");

  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

}