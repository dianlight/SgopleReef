// SgopleReef 
// Based on https://hackaday.io/project/57044-3d-printable-fuel-cap-for-brush-cutter

use <FuelCap.scad>
use<threads.scad>;
use <Hose_Adaptor.scad>

//parameters mm


// Printables parts:
// TankCap Hose FlowCap
print = "TankCap";


// Parameters
flowScrew   = [13.6,1.5];
wallTickness= 2;

module hose(){   
    difference(){
        union(){
            rotate_extrude(angle = 360, convexity = 10) create_profile(
                // Outer Diameter (bottom)
                outer_diameter_1 = 7.5,
                // Wall Thickness (bottom)
                wall_thickness_1 = 1,
                // Rib Thickness (bottom), set to Zero to remove
                barb_size_1 = 0,
                // Length (bottom)
                length_1 = 0,
                // Outer Diameter (top), should be smaller than or equal to Outer Diameter (bottom)
                outer_diameter_2 = 7.5,
                // Wall Thickness (top)
                wall_thickness_2 = 2.3,
                // Rib Thickness (top), set to Zero to remove
                barb_size_2 = 0.5,
                // Length (top)
                length_2 = 13,
                // Middle Diameter
                mid_diameter = 7.5,
                // Middle Length
                mid_length = 0 
            );
            cylinder(r=-wallTickness/1.5+flowScrew[0]/2,h=wallTickness/2);
        }
        cylinder(r=2.8,h=15);
    }
}

module tankCap(){
    union(){
        inSmallCylHgt=5.9;
        difference(){
        innerHeight=15;
        outerHeight=20;
        smallCylHgt=5.9;
        smallCylRadius=17.9/2;    
          buildCap(
            innerHeight=15,
            outerHeight=20,
            smallCylHgt=5.9,
            smallCylRadius=17.9/2,
            inSmallCylHgt=5.9,
            inSmallCylRadius=15.14/2,
            metricThreadDiameter = 38,
            metricThreadPitch    = 4
          );
          translate([0,0,(outerHeight-innerHeight)]){cylinder(r=33/2, h=25);}
          cylinder(r=4.5,h=inSmallCylHgt*2);
          translate([10,10,0]) cylinder(r=4.5,h=inSmallCylHgt*2);
        }
        //small inner cylinder for air vent
        difference(){
              cylinder(r=wallTickness+flowScrew[0]/2,h=13,$fn=40);
              translate([0,0,2]){
                  cylinder(r=11.3/2, h=11);
                  metric_thread(flowScrew[0], flowScrew[1], inSmallCylHgt*2, internal=true);
              }
        }
    }
}

module flowCap(){
    union(){
        difference(){
          innerHeight=9;
          outerHeight=11;
          inSmallCylRadius=15.14/2;
          inSmallCylHgt=5.9;  
  
          cylinder(r=wallTickness+flowScrew[0]/2,h=13,$fn=6);  
          translate([0,0,2]){
              cylinder(r=11.3/2, h=11);
              metric_thread(flowScrew[0], flowScrew[1], inSmallCylHgt*2, internal=true);
          }
          cylinder(r=4.5,h=inSmallCylHgt*2,$fn=60);
        }
        //small inner cylinder for air vent
//        difference(){
//          translate([0,0,outerHeight-innerHeight]){cylinder(h=smallCylHgt, r=smallCylRadius, $fn=40);}
//          metric_thread(12.3, 1.5, inSmallCylHgt*2, internal=true);
//        }
    }
}


if( print == "TankCap"){
    tankCap();
} else if (print == "FlowCap"){
    flowCap();
} else if (print == "Hose"){
    hose($fn = 100*1);
}
