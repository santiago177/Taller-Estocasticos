#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/global-route-manager.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/random-variable-stream.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/aodv-helper.h"
#include "ns3/dsdv-module.h"
#include "ns3/dsr-module.h"
#include "ns3/internet-module.h"
#include "ns3/trace-helper.h"


#include <iostream>
#include <fstream>
#include <vector>
#include <string>

NS_LOG_COMPONENT_DEFINE ("flat-architecture");

using namespace ns3;
using namespace std;
using namespace dsr;

int main (int argc, char *argv[])
{

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%
//%                        Basic simulation parameters
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


  std::string phyMode ("DsssRate1Mbps");
  std::string rtslimit = "2200";
  double distance = 150;  // distance between nodes
  uint32_t numNodes = 30; // number of nodes in the network
  double interval = 0.001; // seconds
  uint32_t packetSize = 1024; // bytes
  uint32_t flows=7; // number of data flows in the network
  double totalTime = 50.0; // simulation time
  uint32_t gridWidth = 5;
    int mobilityModel = 0;
	int routing = 0;
	int seed = 1;


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%
//%                        Command line arguments
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  CommandLine cmd;
  cmd.AddValue ("gridWidth", "grid Width", gridWidth);
  cmd.AddValue ("numNodes", "Number of nodes", numNodes);
  cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
  cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
  cmd.AddValue ("distance", "distance (m)", distance);
  cmd.AddValue ("PacketSize", "Packet size in bytes", packetSize);
  cmd.AddValue ("mobilityModel", "Mobility model", mobilityModel);
  cmd.AddValue ("routing", "Routing", routing);
  cmd.AddValue ("seed", "Seed", seed);
  cmd.Parse (argc, argv);
    ns3::RngSeedManager::SetSeed(seed);
  // Convert to time object
  Time interPacketInterval = Seconds (interval);

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%
//%                        Nodes configuration
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  NodeContainer nodes;   
  nodes.Create (numNodes);


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%
//%                        Channel and network configuration
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));

  // turn off RTS/CTS for frames below 2200 bytes
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue (rtslimit));
  // Fix non-unicast data rate to be the same as that of unicast
  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (phyMode));


  // The below set of helpers will help us to put together the wifi NICs we want
  WifiHelper wifi;

  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  // set it to zero; otherwise, gain will be added
  wifiPhy.Set ("RxGain", DoubleValue (-20) ); 
  // ns-3 supports RadioTap and Prism tracing extensions for 802.11b
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO); 

  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
  wifiPhy.SetChannel (wifiChannel.Create ());

  // Add a non-QoS upper mac, and disable rate control
  NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue (phyMode),
                                "ControlMode",StringValue (phyMode));
  // Set it to adhoc mode
  wifiMac.SetType ("ns3::AdhocWifiMac");
  NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, nodes);

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%
//%                        Mobility model
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/*  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (distance),
                                 "DeltaY", DoubleValue (distance),
                                 "GridWidth", UintegerValue (gridWidth),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);*/
MobilityHelper mobility;
	switch(mobilityModel) {
		case 0:			
		 	mobility.SetPositionAllocator ("ns3::GridPositionAllocator", "MinX", DoubleValue (20.0),
				                                                      "MinY", DoubleValue (20.0),
				                                                      "DeltaX", DoubleValue (20.0),
				                                                      "DeltaY", DoubleValue (20.0),
				                                                      "GridWidth", UintegerValue (5),
				                                                      "LayoutType",StringValue("RowFirst"));


		  	mobility.SetMobilityModel ("ns3::RandomDirection2dMobilityModel",
				                      "Bounds", RectangleValue (Rectangle (0, 100, 20, 80)),
				                      "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=20]"),
				                      "Pause", StringValue ("ns3::ConstantRandomVariable[Constant=10]"));


		  	mobility.Install (nodes);
			break;
		case 1:
			mobility.SetPositionAllocator ("ns3::RandomRectanglePositionAllocator",
                                    "X", StringValue ("ns3::UniformRandomVariable[Min=50.0|Max=120.0]"),
                                    "Y", StringValue ("ns3::UniformRandomVariable[Min=50.0|Max=120.0]"));


			 mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
						                 "Mode", StringValue ("Time"),
						                 "Time", StringValue ("2s"),
						                 "Speed", StringValue ("ns3::ExponentialRandomVariable[Mean=20]"),
						                 "Bounds", StringValue ("30|120|30|120"));
			  mobility.Install (nodes);
			break;
		default:
			ObjectFactory position;

			 position.SetTypeId ("ns3::RandomRectanglePositionAllocator");
			 position.Set ("X", StringValue ("ns3::UniformRandomVariable[Min=20|Max=70]"));
			 position.Set ("Y", StringValue ("ns3::UniformRandomVariable[Min=20|Max=70]"));
			 Ptr<PositionAllocator> PositionAlloc = position.Create ()->GetObject<PositionAllocator> ();
			 mobility.SetMobilityModel ("ns3::RandomWaypointMobilityModel",
							                      "Speed", StringValue ("ns3::ExponentialRandomVariable[Mean=20]"),
							                      "Pause",StringValue ("ns3::ExponentialRandomVariable[Mean=5]"),
							                      "PositionAllocator", PointerValue (PositionAlloc));
							                        
			  mobility.SetPositionAllocator (PositionAlloc);
			  mobility.Install (nodes);
			break;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%
//%                        Routing Algorith 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



  InternetStackHelper internet;
  Ipv4ListRoutingHelper list;



  //Enable OLSR
   NS_LOG_INFO ("OLSR protocol");
  OlsrHelper olsr;
	AodvHelper aodv;
	DsdvHelper dsdv;
	switch(routing) {
		case 0:
			list.Add (olsr, 100);
			break;
		case 1:
			list.Add(aodv, 100);	
			break;
		default:
			list.Add(dsdv, 100);	
			break;
	}
  

  internet.SetRoutingHelper (list); // has effect on the next Install ()
  internet.Install (nodes);

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*  
//Enable AODV
  AodvHelper aodv;
  NS_LOG_INFO ("AODV protocol");
  list.Add (aodv, 100);
  internet.SetRoutingHelper (list); // has effect on the next Install ()
  internet.Install (nodes);
*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/* 
  // Enable DSDV  
  NS_LOG_INFO ("DSDV protocol");
  DsdvHelper dsdv;
  list.Add (dsdv, 100);
  internet.SetRoutingHelper (list); // has effect on the next Install ()
  internet.Install (nodes);
*/

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



  Ipv4AddressHelper ipv4;
  NS_LOG_INFO ("Assign IP Addresses.");
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer ifcont = ipv4.Assign (devices);

 //Collect all interfaces in a single container
 NodeContainer allNodes;
 allNodes.Add(nodes); 
 Ipv4InterfaceContainer allInterfaces;
 allInterfaces.Add(ifcont);


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%
//%                        Random Traffic- (Poisson Model)
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
std::ostringstream stringRate;
stringRate <<packetSize;

int min = 0;
int max = allNodes.GetN()-1;
Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
x->SetAttribute ("Min", DoubleValue (min));
x->SetAttribute ("Max", DoubleValue (max));
// The values returned by a uniformly distributed random
// variable should always be within the range
//
//     [min, max)  .
//
//int value = x->GetValue ();


 

for ( uint32_t i = 0; i <flows ; i++)
   {
  
//int source= UniformVariable().GetInteger(0,allNodes.GetN()-1);
int source = x->GetValue ();
int sink =source;
while(sink==source){
sink=x->GetValue ();
}

  OnOffHelper onoff("ns3::UdpSocketFactory",InetSocketAddress (allInterfaces.GetAddress (source), i+9));
  onoff.SetAttribute ("OnTime", StringValue ("ns3::ExponentialRandomVariable[Mean=1]"));
  onoff.SetAttribute ("OffTime",   StringValue ("ns3::ExponentialRandomVariable[Mean=1]"));
  onoff.SetAttribute ("PacketSize", StringValue(stringRate.str()));
  onoff.SetAttribute ("DataRate", StringValue("2048bps"));

   AddressValue remoteAddress (InetSocketAddress (allInterfaces.GetAddress (sink), i+9));
      onoff.SetAttribute ("Remote", remoteAddress);

      Ptr<UniformRandomVariable> var = CreateObject<UniformRandomVariable> ();
      ApplicationContainer temp = onoff.Install (allNodes.Get (source));
      temp.Start (Seconds (5.1));    
      temp.Stop (Seconds (totalTime));
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  Simulator::Stop(Seconds(250.0));
  FlowMonitorHelper flowmon;               // always install flow monitor before Simulator::run          
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();  
  Simulator::Run ();

 

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//                        Clase FlowMonitor 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



  uint64_t bytesTotal = 0;
  double lastRxTime=-1;
  double firstRxTime=-1;
  uint32_t allTx = 0;
  uint32_t allRx = 0;
  float allDelay = 0.;
  uint32_t allLx = 0;
 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();

   for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
	
	if (firstRxTime < 0)
		firstRxTime = i->second.timeFirstRxPacket.GetSeconds();  // Find first packet arrival time
	else if (firstRxTime > i->second.timeFirstRxPacket.GetSeconds() )
		firstRxTime = i->second.timeFirstRxPacket.GetSeconds();

	if (lastRxTime < i->second.timeLastRxPacket.GetSeconds() )
		lastRxTime = i->second.timeLastRxPacket.GetSeconds();  // Find last packet arrival time

	bytesTotal = bytesTotal + i->second.rxBytes;   

  bytesTotal += i->second.rxBytes * 8.0 / (lastRxTime-firstRxTime)/1024;

 allTx += i->second.txPackets;
 allRx += i->second.rxPackets;
 allDelay += i->second.delaySum.GetSeconds()/i->second.rxPackets ;
 allLx +=  i->second.lostPackets;
 }


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//                           network performance
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//std::cout << "Nodes = " << nodes.GetN() << " "<< "Avg throughput = "<< bytesTotal*8/(lastRxTime-firstRxTime)/1024<<" kbits/sec"<< " AllTx= "<<allTx <<" AllRx= "<< allRx <<" AllLx= "<< allLx << " TotalDelay= " <<allDelay <<std::endl;

std::cout <<numNodes << " "<< bytesTotal*8/(lastRxTime-firstRxTime)/1024<< " "<<allTx <<" "<<allRx <<" "<<allLx <<" "<<allDelay<<std::endl;


  Simulator::Destroy ();
  return 0;
}
