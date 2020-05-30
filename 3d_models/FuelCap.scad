/*
 * https://hackaday.io/project/57044-3d-printable-fuel-cap-for-brush-cutter#menu-descri 
 *
*/
use<threads.scad>;
use<shapes.scad>;

//parameters mm
innerHeight=15;
outerHeight=20;

smallCylHgt=5.9;
smallCylRadius=17.9/2;
inSmallCylHgt=5.9;
inSmallCylRadius=15.14/2;

gripRadius=46.66/2;
gripHeight=15;
innerRadius=36.2/2;  //33.5/2;  //inner cylinder of threads 33.5mm
outerRadius=40.73/2;

//small lip at top to make it easier to atach cap before thread starts
threadStart=2.85;
topLipRadius=38.5/2;

threadOuterRadius=36.5/2;

/*
thread details metric standard Nominal Diameter D = 33mm
thread pitch 3.5mm Coarse
*/

difference(){
buildCap();
translate([0,0,(outerHeight-innerHeight)]){cylinder(r=33/2, h=25);}
}
//small inner cylinder for air vent
difference(){
  translate([0,0,outerHeight-innerHeight]){cylinder(h=smallCylHgt, r=smallCylRadius, $fn=40);}
  translate([0,0,(outerHeight-innerHeight)+0.1]){cylinder(h=inSmallCylHgt, r=inSmallCylRadius, $fn=40);}
}


module buildCap (
metricThreadDiameter = 38,
metricThreadPitch    = 4,
innerHeight=15,
outerHeight=20,

smallCylHgt=5.9,
smallCylRadius=17.9/2,
inSmallCylHgt=5.9,
inSmallCylRadius=15.14/2,

gripRadius=46.66/2,
gripHeight=15,
innerRadius=36.2/2,  //33.5/2;  //inner cylinder of threads 33.5mm
outerRadius=40.73/2,

//small lip at top to make it easier to atach cap before thread starts
threadStart=2.85,
topLipRadius=38.5/2,

threadOuterRadius=36.5/2
){

//Build cap upside down
difference(){
  union(){
    cylinder(h=outerHeight, r=outerRadius, $fn=40); //outer cylinder with thread
    translate([0,0,0]){cylinder(h=gripHeight, r=gripRadius, $fn=40);} //Finger grip section
  }
  translate([0,0,outerHeight-threadStart]){cylinder(h=threadStart+1, r=topLipRadius, $fn=40);} //holow out
  translate([0,0,outerHeight-innerHeight]){
     metric_thread(metricThreadDiameter, metricThreadPitch, innerHeight-threadStart, internal=true);
  }
  rotate([0,0,60]){translate([gripRadius+20,0,0]){cylinder(h=50, r=22, $fn=40);}}
  rotate([0,0,120]){translate([gripRadius+20,0,0]){cylinder(h=50, r=22, $fn=40);}}
  rotate([0,0,180]){translate([gripRadius+20,0,0]){cylinder(h=50, r=22, $fn=40);}}
  rotate([0,0,240]){translate([gripRadius+20,0,0]){cylinder(h=50, r=22, $fn=40);}}
  rotate([0,0,300]){translate([gripRadius+20,0,0]){cylinder(h=50, r=22, $fn=40);}}
  rotate([0,0,0]){translate([gripRadius+20,0,0]){cylinder(h=50, r=22, $fn=40);}}
}

} //end module buildcap

/*//hexagon pattern on top of cap
hexagonWidth=10;

translate([0,0,-2]){
  difference(){
    hexagon(hexagonWidth, 2);
    hexagon(hexagonWidth-2, 2);
  }
}
translate([0,hexagonWidth,-2]){
  difference(){
    hexagon(hexagonWidth, 2);
    hexagon(hexagonWidth-2, 2);
  }
}
translate([0,-hexagonWidth,-2]){
  difference(){
    hexagon(hexagonWidth, 2);
    hexagon(hexagonWidth-2, 2);
  }
}
translate([2*hexagonWidth/3,hexagonWidth/2,-2]){
  difference(){
    hexagon(hexagonWidth, 2);
    hexagon(hexagonWidth-2, 2);
  }
}
translate([2*-hexagonWidth/3,-hexagonWidth/2,-2]){
  difference(){
    hexagon(hexagonWidth, 2);
    hexagon(hexagonWidth-2, 2);
  }
}
translate([2*hexagonWidth/3,-hexagonWidth/2,-2]){
  difference(){
    hexagon(hexagonWidth, 2);
    hexagon(hexagonWidth-2, 2);
  }
}
translate([2*-hexagonWidth/3,hexagonWidth/2,-2]){
  difference(){
    hexagon(hexagonWidth, 2);
    hexagon(hexagonWidth-2, 2);
  }
}

*/
//pin hole in centre
//Drill it manually