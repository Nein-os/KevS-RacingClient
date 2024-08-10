#include "services.h"
#include <iostream>
#include <chrono>
#include <thread>

#include "datatypes/driver.h"
#include "datatypes/team.h"
#include "irsdk_client.h"
#include "connection/data_collector.h"
#include "connection/iracing_collector.h"

void run_connection(IKevS_DataCollector *data, bool *run)
{
	while (*run) {
#if KEVS_USAGE_TYPE == IRACING_USAGE
		if (irsdkClient::instance().waitForData(16)) {
			/*if (irsdkClient::instance().wasSessionStrUpdated()) 
				processYAMLSessionString(irsdkClient::instance().getSessionStr());*/
			data->update_session();
			// Seems like, its not needed for now
			// std::this_thread::sleep_for(std::chrono::milliseconds(100));
		} else
#elif KEVS_USAGE_TYPE == SECRET_USAGE
	// TODO
#else 
	// TODO
#endif
			std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}
