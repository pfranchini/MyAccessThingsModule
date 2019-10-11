// Interface from Falaise
#include "falaise/snemo/processing/module.h"
// Any data models we need
#include <bayeux/mctools/simulated_data.h>
#include <falaise/snemo/datamodels/calibrated_data.h>
#include "falaise/snemo/datamodels/tracker_clustering_data.h"
#include "falaise/snemo/datamodels/particle_track_data.h"


class MyAccessThingsModule {
public:
  MyAccessThingsModule() = default;
  MyAccessThingsModule(falaise::config::property_set const& /*ps*/,
		     datatools::service_manager& /*services*/) {
  }
  // Process event
  falaise::processing::status process(datatools::things& event) {
    // Print most basic information
    //std::cout << std::endl << "MyAccessThingsModule::process called!" << std::endl;
    //std::cout << "[name]        : " << event.get_name() << std::endl;
    //std::cout << "[description] : " << event.get_description() << std::endl;

    // Extract list of keys stored by the object
    std::vector<std::string> keys;
    event.get_names(keys);
    // Print each key and its concrete type
    for (const auto& k : keys) {
      std::cout << "- [key, serial_tag] : "
		<< k
		<< ", "
		<< event.get_entry_serial_tag(k)
		<< std::endl; 
    }

    // Grab simulated data bank SD
    auto& simData = event.get<mctools::simulated_data>("SD");
   
    //simData.tree_dump();
    std::cout << "Vertex x: " <<  simData.get_vertex().x() << std::endl;
    std::cout << "Vertex y: " <<  simData.get_vertex().y() << std::endl;
    std::cout << "Vertex z: " <<  simData.get_vertex().z() << std::endl;

    // Grab calibrated data bank CD
    try {
      const snemo::datamodel::calibrated_data& calData = event.get<snemo::datamodel::calibrated_data>("CD");
      
      const snemo::datamodel::calibrated_data::tracker_hit_collection_type & trackerHits=calData.calibrated_tracker_hits();
      for (snemo::datamodel::calibrated_data::tracker_hit_collection_type::const_iterator   iHit = trackerHits.begin(); iHit != trackerHits.end(); ++iHit) {
	const snemo::datamodel::calibrated_tracker_hit & trackerHit = iHit->get();
      	std::cout << "Tracker Hit: " << trackerHit.get_x() << "," << trackerHit.get_y() << "," << trackerHit.get_z() << std::endl;
	
      }      
      
      const snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calHits=calData.calibrated_calorimeter_hits();
      for (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::const_iterator iHit = calHits.begin(); iHit != calHits.end(); ++iHit) {
	const snemo::datamodel::calibrated_calorimeter_hit & calHit = iHit->get();
      	std::cout << "Cal Hit Energy: " << calHit.get_energy() << std::endl;
      }      

    }
    catch (std::logic_error& e) {
      std::cerr << "failed to grab CD bank : " << e.what() << std::endl;
      //return dpp::base_module::PROCESS_INVALID;
    }
    
    // Grab clustered data bank TCD
    try {
      const snemo::datamodel::tracker_clustering_data& clusterData = event.get<snemo::datamodel::tracker_clustering_data>("TCD");

      int clusterCount=0;
      if (clusterData.has_default_solution ()) // Looks as if there is a possibility of alternative solutions. Is it sufficient to use the default?
	{
	  snemo::datamodel::tracker_clustering_solution solution = clusterData.get_default_solution () ;
	  snemo::datamodel::tracker_clustering_solution::cluster_col_type clusters=solution.get_clusters();
	  for (snemo::datamodel::tracker_clustering_solution::cluster_col_type::const_iterator iCluster = clusters.begin();  iCluster != clusters.end(); ++ iCluster)
	    {
	      ++clusterCount;
	    }
	}
      std::cout << "Number of tracker clusters: " << clusterCount << std::endl;

    }
    catch (std::logic_error& e) {
      std::cerr << "failed to grab TCD bank : " << e.what() << std::endl;
    }
    
    // Grab tracked data bank PTD
    try {
      const snemo::datamodel::particle_track_data& trackData = event.get<snemo::datamodel::particle_track_data>("PTD");
    }
    catch (std::logic_error& e) {
      std::cerr << "failed to grab PTD bank : " << e.what() << std::endl;
    }


    return falaise::processing::status::PROCESS_OK;
  }
};
FALAISE_REGISTER_MODULE(MyAccessThingsModule)

