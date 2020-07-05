// SgopleReef 
// Based on https://hackaday.io/project/57044-3d-printable-fuel-cap-for-brush-cutter

//use <FuelCap.scad>
//use<threads.scad>;
use <threadlib/threadlib.scad>
use <Hose_Adaptor.scad>
use <NopSCADlib/utils/rounded_cylinder.scad>
use <NopSCADlib/utils/rounded_polygon.scad>

//parameters mm

MY_THREAD_TABLE = [
["M14x1.25-ext", [1.25, 6.1290, 12.4385, [[0, -0.5541], [0, 0.5541], [0.8035, 0.0902], [0.8035, -0.0902]]]],
["M14x1.25-int", [1.25, -7.1669, 14.1750, [[0, 0.6188], [0, -0.6188], [0.7772+0.35, -0.1700], [0.7772+0.35, 0.1700]]]],
                   ];

// Printables parts:
// TankPipe TankCap Hose FlowCap
print = "TankCap";


// Parameters
capThread        = "M48x4";
capThreadTurns   = 3;
wallTickness     = 2;
capSpc           = thread_specs(str(capThread,"-int"));
icapSpc          = thread_specs(str(capThread,"-ext"));
threadDiamater   = capSpc[2]+wallTickness*2;
ithreadDiamater  = icapSpc[2];
flowThread       = "M14x1.25"; // M14
flowThreadTurns  = 5;
flowSpc          = thread_specs(str(flowThread,"-int"));
gripTickness     = 5;
tankBorder       = 12;
tankBorderLen    = 90;
tankBorderTick   = 5;

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
            cylinder(r=-wallTickness/1.5+flowSpc[2]/2,h=wallTickness/2);
        }
        cylinder(r=2.8,h=15);
    }
}


module tankPipe(){
    difference(){
        union(){
            bolt(capThread,turns=capThreadTurns,higbee_arc=30);
            profile = [
                [-tankBorderLen/2,0,0],
                [0,1,threadDiamater/2],
                [tankBorderLen/2,0,0],
                [0,-1,threadDiamater/2]
            ];
            tangents = rounded_polygon_tangents(profile);
            length = rounded_polygon_length(profile, tangents);
            for(zindex=[1:1:tankBorderTick]){
                translate([0,0,-zindex*(tankBorder/tankBorderTick)-wallTickness]) 
                 linear_extrude(tankBorder/tankBorderTick)
                  scale([1,zindex % 2 == 1?1:0.95,1]) rounded_polygon(profile, tangents);
            }
        }
        cylinder(h=(capThreadTurns*capSpc[0]+wallTickness*2)*2,r=ithreadDiamater/2-wallTickness/2,center=true,$fn=160);
    }
}


module tankCap(){
    difference(){
     union(){   
        nut(capThread, turns=capThreadTurns, Douter=threadDiamater+gripTickness);
        translate([0,0,capThreadTurns*capSpc[0]+wallTickness]) 
                rounded_cylinder(h=wallTickness,r2=wallTickness,r=threadDiamater/2+gripTickness/2,$fn=180);
        translate([0,0,wallTickness+capThreadTurns*capSpc[0]-flowThreadTurns*flowSpc[0]]) nut(flowThread,turns=flowThreadTurns,Douter=flowSpc[2]+wallTickness*2,,table=MY_THREAD_TABLE);
     }
     cylinder(r=4.5,h=capThreadTurns*capSpc[0]+wallTickness*2,$fn=40);
     translate([threadDiamater/3,-7.2-wallTickness/2,0]) 
        rotate([0,0,90])
         cube([14.4+wallTickness,5.5+wallTickness,capThreadTurns*capSpc[0]+wallTickness*2]);
     for (rands=[0:45:360]){
         rotate([0,0,rands]) 
             translate([threadDiamater/2+gripTickness*2+wallTickness/2,0,0]) 
             cylinder(h=(capThreadTurns+1)*capSpc[0]+wallTickness,r=gripTickness*2.3,$fn=60);
     }
    }
    
}


module flowCap(){
    intersection(){
        union(){
            nut(flowThread,turns=flowThreadTurns*2,Douter=flowSpc[2]+wallTickness*2,table=MY_THREAD_TABLE);
            difference() {
              cylinder(r=flowSpc[2],h=wallTickness,$fn=60);  
              cylinder(r=4.5,h=wallTickness,$fn=60);
            }
        }
        cylinder(r=wallTickness+flowSpc[2]/2,h=23,$fn=6);  
    }
}


if( print == "TankCap"){
    tankCap();
} else if (print == "TankPipe"){
    tankPipe();
} else if (print == "FlowCap"){
    flowCap();
} else if (print == "Hose"){
    hose($fn = 100*1);
}
