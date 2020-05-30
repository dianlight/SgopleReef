// SgoopleReef Central


use <NopSCADlib/utils/rounded_polygon.scad>
use <NopSCADlib/utils/layout.scad>
use <NopSCADlib/vitamins/pcb.scad>
include <NopSCADlib/vitamins/pin_headers.scad>
include <NopSCADlib/vitamins/green_terminals.scad>
include <NopSCADlib/vitamins/leds.scad>

// Printable parts
// Box | Panel | Buttons
// Support parts (not for print )
// PCB


print        = "Buttons";
showElements = false;

// Parametres
layerHeight = 0.3;
//             L  W   H   Lc Hc
boxSize     = [85,85,85, 20, 10 ];
boxSmoot    = 1;
boxTikness  = 2.1;
boxOverlap  = 5;


module _box_perimeter(part){
    if(part == "panel"){
        profile_front = [
           [ -boxSize[0]/2 , 0,  boxSmoot],
           [ -boxSize[0]/2 , boxSize[1]-boxSmoot-boxSize[4],  boxSmoot],
           [ -(boxSize[0]/2)+boxTikness-boxSmoot*2 , boxSize[1]-boxSmoot-boxSize[4],  boxSmoot],
           [ -(boxSize[0]/2)+boxTikness , boxTikness,  -boxSmoot],
           [ boxSize[0]/2-boxSize[3] , boxTikness-boxSmoot*2,  boxSmoot],
           [ boxSize[0]/2-boxSize[3] , 0,  boxSmoot]
            ];
        tangents_front = rounded_polygon_tangents(profile_front);

        linear_extrude(boxSize[2])
               rounded_polygon(profile_front, tangents_front);

    } else {

        profile_base = [
            [ boxTikness-boxSize[0]/2 , boxTikness,  boxSmoot],
            [ boxTikness-boxSize[0]/2 , boxSize[1]-boxSmoot-boxSize[4],  boxSmoot],
            [ boxTikness-boxSize[0]/2+boxSize[4] , boxSize[1]-boxSmoot-boxSize[4],  boxSmoot],
            [ boxSize[0]/2-boxSize[3] , boxTikness+boxSize[3],  boxSmoot],
            [ boxSize[0]/2-boxSize[3] , boxTikness,  boxSmoot]
        ]; 
        tangents_base = rounded_polygon_tangents(profile_base);

        linear_extrude(boxTikness)
               rounded_polygon(profile_base, tangents_base);

        translate([0,0,boxSize[2]-boxTikness]) linear_extrude(boxTikness)
               rounded_polygon(profile_base, tangents_base);


        profile_back = [
           [ boxTikness*2-boxSize[0]/2+boxSize[4]-boxSmoot*2, boxSize[1]-boxOverlap-boxSize[4]+boxSmoot*2,  -boxSmoot],        
           [ boxTikness*2-boxSize[0]/2, boxSize[1]-boxOverlap-boxSize[4]+boxSmoot*2,  -boxSmoot],        
           [ boxTikness-boxSize[0]/2, boxSize[1]-boxSmoot-boxOverlap-boxSize[4],  boxSmoot],
           [ boxTikness-boxSize[0]/2 , boxSize[1]-boxSmoot-boxSize[4],  boxSmoot],
           [ boxTikness-boxSize[0]/2+boxSize[4] , boxSize[1]-boxSmoot-boxSize[4],  boxSmoot],
           [ boxSize[0]/2-boxSize[3] , boxTikness+boxSize[3],  boxSmoot],
           [ boxSize[0]/2-boxSize[3] , boxTikness,  boxSmoot],
           [ (boxSize[0]/2)-boxOverlap-boxSize[3] , boxTikness,  boxSmoot],
           [ (boxSize[0]/2)-boxOverlap-boxSize[3]+boxSmoot*3 , boxTikness*2,  -boxSmoot],
           [ (boxSize[0]/2)-boxOverlap-boxSize[3]+boxSmoot*3 , boxTikness*2+boxSize[3]-boxSmoot*2,  -boxSmoot],
            ];
        tangents_back = rounded_polygon_tangents(profile_back);

        linear_extrude(boxSize[2])
               rounded_polygon(profile_back, tangents_back);
               
        translate([boxSize[0]/2-boxSize[3]-(boxOverlap+boxSmoot*2)/2,boxTikness+boxSize[3]/2,boxSize[0]/8]) 
            cube([boxOverlap+boxSmoot*2,boxSize[3],boxOverlap+boxSmoot*2],center=true);       

        translate([boxSize[0]/2-boxSize[3]-(boxOverlap+boxSmoot*2)/2,boxTikness+boxSize[3]/2,boxSize[0]/8*7]) 
            cube([boxOverlap+boxSmoot*2,boxSize[3],boxOverlap+boxSmoot*2],center=true);       

        translate([-boxSize[0]/2+boxSize[4]-(boxOverlap+boxSmoot*2)/2,boxSize[2]-boxSize[3]/2-(boxOverlap+boxSmoot*2)/2,boxSize[0]/8]) 
            cube([boxSize[4],boxOverlap+boxSmoot*2,boxOverlap+boxSmoot*2],center=true);       

        translate([-boxSize[0]/2+boxSize[4]-(boxOverlap+boxSmoot*2)/2,boxSize[2]-boxSize[3]/2-(boxOverlap+boxSmoot*2)/2,boxSize[0]/8*7]) 
            cube([boxSize[4],boxOverlap+boxSmoot*2,boxOverlap+boxSmoot*2],center=true);       

    }
            
}


module _screw(head=false){
     size=head?3/2:3.8/2;
     translate([boxSize[0]/2-boxSize[3]-size/2-boxOverlap/2,boxOverlap*2,boxSize[2]/8]) rotate([90,0,0]) cylinder(h=boxOverlap*4,r=size,$fn=50);
     translate([boxSize[0]/2-boxSize[3]-size/2-boxOverlap/2,boxOverlap*2,boxSize[2]/8*7]) rotate([90,0,0]) cylinder(h=boxOverlap*4,r=size,$fn=50);

     translate([-boxSize[0]/2-boxTikness*2,boxSize[1]-boxSize[4]-size/2-boxOverlap/2,boxSize[2]/8]) rotate([90,0,90]) cylinder(h=boxOverlap*3,r=size,$fn=50);
     translate([-boxSize[0]/2-boxTikness*2,boxSize[1]-boxSize[4]-size/2-boxOverlap/2,boxSize[2]/8*7]) rotate([90,0,90]) cylinder(h=boxOverlap*3,r=size,$fn=50);
}
module box(){
    difference(){
        _box_perimeter("back");
        _pcb("cut");
        _screw(false);
    }
}

module panel(){
    difference(){
        union(){
            _box_perimeter("panel");
            _pcb("spacer");
        }
        _screw(true);
        _pcb("cut");
    }
}


module _button_layout(){
        translate([18,-18.5,-40]) rotate([0,0,90]) 
         layout([(3+layerHeight)*2,(3+layerHeight)*2,(3+layerHeight)*2], gap=4)
             children();
}

module buttons(){
    translate([-7,29,47]) rotate([90,0,90]){
        _button_layout(){
            rotate([0,0,90]) cylinder(r=2.3,h=12+boxTikness*3,$fn=6);
            translate([0,0,boxTikness*3]) cylinder(r=4,h=boxTikness,$fn=30);
        }
    }
}
module _pcb(show="full"){  // full / spacer / cut
 translate([-23,29,47]) rotate([90,0,90]){   
    control = ["SgopleReef","SgopleReef",
        50,70,1.6,
        0,      // Corner radius
        2.75,   // Mounting hole diameter
        0,      // Pad around mounting hole
        "green",
        false, // True if the parts should be separate BOM items
        // hole offsets
        [ [-2.3, 2.3], [-2.3, -2.3] ],
        // components
        [
            [  1.7,  35,  90, "-2p54socket", 24, 1 ],
            [  22.8,  35,  90, "-2p54socket", 24, 1 ],
            [  12.8,  35,  90, "2p54socket", 24, 1 ],
            [  -6.8,  18.4+3,  90, "-button_6mm", 24, 1 ],
            [  -6.8,  18.4+3+8.75,  90, "-button_6mm", 24, 1 ],
            [  -6.8,  18.4+3+8.75+8.75,  90, "-button_6mm", 24, 1 ],
        ],
        // accessories
        []
    ];
    if(show == "full")pcb(control);
    if(show == "cut"){
        pcb_cutouts(control);
        _button_layout(){
            rotate([0,0,90]) cylinder(r=3+layerHeight,h=40,$fn=6);
        }
//        translate([18,-13.5-2,-40]) cylinder(r=3+layerHeight,h=40,$fn=6);
//        translate([18,-13.5+8.75,-40]) cylinder(r=3+layerHeight,h=40,$fn=6);
//        translate([18,-13.5+2+8.75*2,-40]) cylinder(r=3+layerHeight,h=40,$fn=6);
    } 
    if (show == "full" || show == "spacer"){
        pcb_screw_positions(control){
            translate([0,0,-20]) difference() {
                cylinder(r=3.8/2+(layerHeight*4), h=21);
                cylinder(r=3.8/2, h=21);
            }
        }
    }
    
    esp32 = ["ESP32","ESP32",
        26,48,1.6,
        0,      // Corner radius
        2.75,   // Mounting hole diameter
        0,      // Pad around mounting hole
        "black",
        false, // True if the parts should be separate BOM items
        // hole offsets
        [],
        // components
        [
            [  1.7,  48/2,  90, "2p54header", 19, 1 ],
            [  22.8,  48/2,  90, "2p54header", 19, 1 ],
            [  26/2,  48,  -90, "-usb_uA"],
            
        ],
        // accessories
        []
    ];
    
    translate([-12,7,-12]){ 
        if(show == "full")pcb(esp32);
        if(show == "cut"){
            pcb_cutouts(esp32);
            translate([7,19,-40]) cylinder(r=1.2+layerHeight,h=40,$fn=50);
            translate([-7,19,-40]) cylinder(r=1.2+layerHeight,h=40,$fn=50);
        }
    }


    oled = ["OLED","OLED",
        38,12,1.6,
        0,      // Corner radius
        2.75,   // Mounting hole diameter
        0,      // Pad around mounting hole
        "gray",
        false, // True if the parts should be separate BOM items
        // hole offsets
        [],
        // components
        [
            [  1.7,  12/2,  90, "2p54header", 4,  1 ],
            
        ],
        // accessories
        []
    ];
    
    translate([-6,-25.6,-12]){
        if(show == "full")pcb(oled);
        if(show == "cut"){
            pcb_cutouts(oled);
            translate([0,0,-20]) cube([24,10,40],center=true);
        }
    }

    connector = ["SgopleReef2","SgopleReef2",
        30,70,1.6,
        0,      // Corner radius
        2.75,   // Mounting hole diameter
        0,      // Pad around mounting hole
        "green",
        false, // True if the parts should be separate BOM items
        // hole offsets
        [ [2.3, 2.3], [2.3, -2.3], [-2.3, 2.3], [-2.3, -2.3] ],
        // components
        [
            [  -1.7,  35,  90, "2p54header", 24, 1,undef, undef, false ],
            [  2.75,  54,  180, "-term254", 2],
            [  2.75,  35,   90, "-jst_xh", 5],
            [  6+2.75,  8,   0, "-jst_xh", 4],
        ],
        // accessories
        []
    ];
    translate([-17,0,24]) rotate([0,90,0]) {
         if(show == "full")pcb(connector);
         if(show == "cut"){
             pcb_cutouts(connector);
             translate([-6,-27,-40]) cube([jst_xh_header[3]*5,jst_xh_header[7],80],center=true);
             translate([-12,0,-40]) rotate([0,0,90]) cube([jst_xh_header[3]*6,jst_xh_header[7],80],center=true);
             color("green") translate([-13,20,-40]) rotate([0,0,0]) cube([gt_2p54[3],gt_2p54[3],80],center=true);
   //          color("green") translate([-45,18,-5]) rotate([0,90,0]) cube([gt_2p54[3],gt_2p54[3],80],center=true);
         }
        if (show == "full" || show == "spacer"){
            pcb_screw_positions(connector){
                    translate([0,0,-11]) difference() {
                        cylinder(r=3.8/2+(layerHeight*4), h=8);
                        cylinder(r=3.8/2, h=8);
                    }
                }
        }
         
    }
    
    
    translate([-15,-35,20]) rotate([90,90,0]){
        if(show == "full")jst_xh_header(jst_xh_header, 5, right_angle=false); 
        if(show == "cut")translate([0,-37,10-jst_xh_header[7]/2]) rotate([90,0,0]) cube([jst_xh_header[3]*5,jst_xh_header[7]/2,80],center=true);
        translate([10,25,-50]) cube([20,20,20],center=true);
    
    }
  }  
    
}



if (print == "Box"){
    box();
    if(showElements)%_pcb();
} else if (print == "Panel"){
    panel();
    if(showElements){
        %_pcb();
        %box();
        %buttons();
    }
} else if (print == "Buttons"){
    buttons();
    if(showElements){
        %panel();
    }
} else if (print == "PCB") {
    _pcb();
}