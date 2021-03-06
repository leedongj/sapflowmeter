// ================================================================
// ===                        LIBRARIES                         ===
// ================================================================


// ================================================================ 
// ===                       DEFINITIONS                        === 
// ================================================================
#define HEATPIN 12

// ================================================================ 
// ===                        STRUCTURES                        === 
// ================================================================

struct state_sapflow_t {
	float temp0, temp1, temp_diff;
};

// ================================================================ 
// ===                   GLOBAL DECLARATIONS                    === 
// ================================================================

unsigned long lastUpdate;
unsigned long currentTime;
bool sapflow_relay_on = false;

struct state_sapflow_t state_sapflow;


// ================================================================ 
// ===                   FUNCTION PROTOTYPES                    === 
// ================================================================
#if is_node == 1
void package_sapflow(OSCBundle *bndl, char packet_header_string[]);
void measure_sapflow();
double voltTotemp(double vout);
void heat(uint16_t pulse);
#endif
void run_sapflowmeter();

// ================================================================ 
// ===                          SETUP                           === 
// ================================================================
//
// Runs any startup for sapflowmeter that should occur on device startup
// 
// @return  Whether or not sensor initialization was successful
//
void setup_sapflow() 
{
  #if is_node == 1
  	setup_sht31d();
  
  	pinMode(HEATPIN,OUTPUT);
  	lastUpdate = millis();
  #endif

}


// ================================================================ 
// ===                        FUNCTIONS                         === 
// ================================================================

#if is_node == 1

// --- SAP FLOW METER
//Convert voltage output to temperature value
double voltTotemp( double vout )
{ 
	double vcc = 3300; //3.3V
	double Ro = 100000.0;
	double To = 298.15;
	double B = 4700.0;

	double Rbias = 100000.0;
	double long R = ((vcc / vout) - 1) * Ro;

	double long T = B / ( log( R * exp( B / To) ) - (log(Ro) ) ) - 273.15 ;
	return T;
}

void heat(uint16_t pulse)
{ //heatpulse = 0: TDM, heatpulse > 1: HRM
//  int heatpulse = 1000; //http://www.open-sensing.org/sapflowmeter-blog/2018/6/4/sap-flux-heat-calculations

	currentTime = millis();
	if (currentTime - lastUpdate > pulse) {
		digitalWrite(HEATPIN, sapflow_relay_on ? HIGH : LOW);
		sapflow_relay_on = !sapflow_relay_on;
		lastUpdate = currentTime;  
	}

}
	
// --- PACKAGE <MODULE> ---
//
// Adds OSC Message of most recent sensor readings to a provided OSC bundle
//
// @param bndl                  The OSC bundle to be added to
// @param packet_header_string  The device-identifying string to prepend to OSC messages
//   if I2C multiplexer sensor, then also
// [@param port                  Which port of the multiplexer the device is plugged into]
//
void package_sapflow(OSCBundle *bndl, char packet_header_string[]) 
{
	char address_string[255];
	
	sprintf(addressString, "%s%s", packet_header_string, "/temp0");
	bndl->add(addressString).add(state_sapflow.temp0);
	sprintf(addressString, "%s%s", packet_header_string, "/temp1");
	bndl->add(addressString ).add(state_sapflow.temp1);
	sprintf(addressString, "%s%s", packet_header_string, "/temp_diff");
	bndl->add(addressString).add(state_sapflow.temp_diff);
}


// --- MEASURE ANALOG ---
//
// Measure analog data and update analog state to most recent readings. 
//
void measure_sapflow() 
{
	double temp;

	temp = read_analog(0);  //analog Read gives values from 0-1023 based on 0-3.3V
	temp = map(temp, 0, 4095, 0, 3300);//map these to mV value
	state_sapflow.temp0 = voltTotemp(temp);

	temp = read_analog(1);  //analog Read gives values from 0-1023 based on 0-3.3V
	temp = map(temp, 0, 4095, 0, 3300);//map these to mV value
	state_sapflow.temp1 = voltTotemp(temp);

	state_sapflow.temp_diff = state_sapflow.temp0 - state_sapflow.temp1;
	if (state_sapflow.temp_diff < 0) {
		state_sapflow.temp_diff *= -1;
	}
}


#endif // of is_node == 1


void run_sapflowmeter(OSCBundle *bndl)
{
    #if is_hub == 1
  
    // Receive bundles, takes bundle to be filled and wireless platforms [WIFI, LORA, NRF]
      receive_bundle(bndl, LORA);
  
    if (!bundle_empty(bndl)) {
      print_bundle(bndl);
      send_bundle(bndl, PUSHINGBOX);
  
    }
  #endif // of is_hub

  #if is_node == 1
    measure_sensors();
    package_data(bndl);
    print_bundle(bndl);
    send_bundle(bndl, LORA);
    sd_save_bundle("Log0711.csv", bndl, 0, 3);
    //  read_all_from_file("newlog");
    
    #if probe_type  == 1      // 0:TDM, 1: HRM    
      heat(heatpulse);
    #endif // of probe_type
   
   delay(senddelay); //send data per 1 min
  
  #endif // of is_node
}



