// Interface from Falaise
#include <falaise/snemo/processing/module.h>
// Any data models we need
#include <bayeux/mctools/simulated_data.h>
#include <falaise/snemo/datamodels/calibrated_data.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/datamodels/particle_track_data.h>


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

    // Get simulated data bank SD
    auto& simData = event.get<mctools::simulated_data>("SD");

    //simData.tree_dump();
    std::cout << "Vertex x: " <<  simData.get_vertex().x() << std::endl;
    std::cout << "Vertex y: " <<  simData.get_vertex().y() << std::endl;
    std::cout << "Vertex z: " <<  simData.get_vertex().z() << std::endl;

    // Get calibrated data bank CD
    try {
      const auto& calData = event.get<snemo::datamodel::calibrated_data>("CD");

      for (const auto& trackerHitHdl : calData.calibrated_tracker_hits()) {
        std::cout << "Tracker Hit: " << trackerHitHdl->get_x() << "," << trackerHitHdl->get_y() << "," << trackerHitHdl->get_z() << std::endl;
      }

      for (const auto& calHitHdl : calData.calibrated_calorimeter_hits()) {
        std::cout << "Cal Hit Energy: " << calHitHdl->get_energy() << std::endl;
      }

    }
    catch (std::logic_error& e) {
      std::cerr << "failed to grab CD bank : " << e.what() << std::endl;
      //return dpp::base_module::PROCESS_INVALID;
    }

    // Grab clustered data bank TCD
    try {
      const auto& clusterData = event.get<snemo::datamodel::tracker_clustering_data>("TCD");

      int clusterCount=0;
      if (clusterData.has_default_solution()) { // Looks as if there is a possibility of alternative solutions. Is it sufficient to use the default?
        using tracker_clustering_solution = snemo::datamodel::tracker_clustering_solution;

        tracker_clustering_solution solution = clusterData.get_default_solution();
        for (const auto& cluster : solution.get_clusters()) {
          ++clusterCount;
        }
      }
      std::cout << "Number of tracker clusters: " << clusterCount << std::endl;

    }
    catch (std::logic_error& e) {
      std::cerr << "failed to grab TCD bank : " << e.what() << std::endl;
    }

    // Get tracked data bank PTD
    try {
      const auto& trackData = event.get<snemo::datamodel::particle_track_data>("PTD");
    }
    catch (std::logic_error& e) {
      std::cerr << "failed to grab PTD bank : " << e.what() << std::endl;
    }


    return falaise::processing::status::PROCESS_OK;
  }
};
FALAISE_REGISTER_MODULE(MyAccessThingsModule)

