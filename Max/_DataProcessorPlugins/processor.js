// Author: Chet Udell
// automatically generate Data Map tracks on request
//
//
// inlets and outlets
inlets = 1;
outlets = 1;

// global variables and arrays
var memstr;
var thebpatchers = new Array(255);
var numtracks = 0;
//target = this.patcher.filepath+"/Plugins";
target = this.patcher.filepath;
//f = new File(target+"/Plugins","read");
f = new File(target,"read");
var dragging = 0;
var sel = -1;

// This function is to set the initial number of tracks from last secession
// Still cannot remove old instances from previous secession though!
function init(num)
{
    numtracks = num;
    post(target); post(); // for debug
//    this.patcher.applyif(_$act, _$scan);
//    delete tempname
}

function createBpatcher(file)
{

	if(numtracks<=255)
	{
		switch (file) {
			case "em.relay_(Expert)":
			case "em.servo_(Expert)": 
				dimensions = ["250", "200"]; break;
			case "em.servo": 
				dimensions = ["155", "120"]; break;
			case "em.relay": 
				dimensions = ["150", "100"]; break;
			case "em.neopixel": 
				dimensions = ["200", "145"]; break;
			case "em.IshieldMonitor": 
				dimensions = ["145", "220"]; break;
			case "em.LOOM_Channel_Manager":
				dimensions = ["325", "250"]; break;
			case "em.LOOMin":
				dimensions = ["200", "160"]; break;
			case "em.Decagon":
				dimensions = ["160", "135"]; break;
			case "em.MuxShieldMonitor":
				dimensions = ["170", "180"]; break;
			case "em.stepper_motor":
				dimensions = ["200", "150"]; break;	
			case "em.stepper_motor_extension":
				dimensions = ["270", "225"]; break;	
			default: 
				dimensions = ["250", "150"]; break;
		}

		// offset = (numtracks*175); // Use this offset if desiring scrolling
		offset = 1; //use this offset if bpatchers to spawn in same place
		thebpatchers[numtracks] = this.patcher.parentpatcher.newdefault(offset,0,
		"bpatcher",
		"@varname", 
		"obj["+numtracks+"]",
		"@name", file,
		"@patching_rect", "300", (offset+10), dimensions[0], dimensions[1],
		"@presentation_rect", "10", (offset+50), dimensions[0], dimensions[1],
		"@border", "1",
		"@clickthrough", "1",
		"@orderfront", "1",
		"@embed", "1",
		"@presentation", "1");

		// increment numtracks
		numtracks++;
		outlet(0,numtracks);    // output to int box to preserve
	}
//post(numtracks); post(); // for debug
} 

function deleteBpatcher()
{
if(numtracks>=1)
  {
    // decrement numtracks
    numtracks--;

    this.patcher.parentpatcher.remove(thebpatchers[numtracks]);
    outlet(0,numtracks);
//    post(numtracks); post(); // for debug
  }
} 


//Delete all generated Bpatchers from this session, reset.

function bang() {
    for (i=0; i<thebpatchers.length; i++) {
        this.patcher.remove(thebpatchers[i]);
    }
    numtracks = 0;
    thebpatchers = [];
	outlet(0,numtracks);
}
