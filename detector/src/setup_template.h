// setup.h contains sensitive information, therefore it is not uploaded to the repository.
// This file provides a template of what setup.h should look like.

// ########################
// # Debug or production? #
// ########################
#define DEBUG
//#define PRODUCTION

// ####################
// # WiFi information #
// ####################
#define SETUP_WIFI_SSID "ThisIsAnSSID"
#define SETUP_WIFI_PSWD "ThisIsAPSWD"

// ######################################
// # Configuration specific information #
// ######################################
#ifdef DEBUG

// #### TCPClient information ####
// IP or URL of the API server.
#define SETUP_SERVER_ADDRESS "ThisIsAnIPAddressOrURL"
// Port of the API server.
#define SETUP_SERVER_PORT 80

#endif

#ifdef PRODUCTION

// #### TCPClient information ####
// IP or URL of the API server.
#define SETUP_SERVER_ADDRESS "ThisIsAnIPAddressOrURL"
// Port of the API server.
#define SETUP_SERVER_PORT 80

#endif