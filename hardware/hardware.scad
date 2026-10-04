// 3DScad hardware library: metric screws, nuts, washers and holes.
//
// Written in the OpenSCAD subset the editor round-trips (no functions,
// no if inside modules, named arguments only). The Insert > Hardware menu
// copies the modules a part needs into the document and adds a call with
// ISO dimensions for the chosen size.
//
// Conventions:
// - z = 0 is the surface the part sits on / is driven into.
// - Screws: head above z = 0 (countersunk heads end flush at z = 0),
//   shank goes down to z = -l.
// - Nuts and washers sit on z = 0 and go up.
// - Holes are negative bodies: put them in a difference() as cuts. They
//   start 1 mm above z = 0 so the cut is clean and go down to z = -l.
// - "_threaded" variants model a printable thread (single-start, right hand)
//   with a twisted eccentric circle; the plain variants are smooth
//   cylinders at the nominal diameter.

// Hexagonal prism, s = width across flats, from z = 0 to z = h.
module hw_hex_prism(s = 10, h = 5) {
    intersection() {
        translate([0, 0, h / 2]) {
            cube([s, s * 2, h], center=true);
        }
        rotate([0, 0, 60]) {
            translate([0, 0, h / 2]) {
                cube([s, s * 2, h], center=true);
            }
        }
        rotate([0, 0, 120]) {
            translate([0, 0, h / 2]) {
                cube([s, s * 2, h], center=true);
            }
        }
    }
}

// Hex head bolt, ISO 4017.
module hex_bolt(d = 3, l = 10, s = 5.5, k = 2) {
    union() {
        hw_hex_prism(s = s, h = k);
        translate([0, 0, -l]) {
            cylinder(h=l, r=d / 2);
        }
    }
}

module hex_bolt_threaded(d = 3, l = 10, s = 5.5, k = 2, pitch = 0.5) {
    union() {
        hw_hex_prism(s = s, h = k);
        translate([0, 0, -l]) {
            linear_extrude(height=l, twist=-360 * l / pitch, slices=ceil(l / pitch * 10)) {
                translate([pitch * 0.3, 0, 0]) {
                    circle(r=d / 2 - pitch * 0.3);
                }
            }
        }
    }
}

// Socket head cap screw, ISO 4762.
module socket_head_screw(d = 3, l = 10, dk = 5.5, k = 3, key = 2.5) {
    difference() {
        union() {
            cylinder(h=k, r=dk / 2);
            translate([0, 0, -l]) {
                cylinder(h=l, r=d / 2);
            }
        }
        translate([0, 0, k * 0.4]) {
            hw_hex_prism(s = key, h = k);
        }
    }
}

module socket_head_screw_threaded(d = 3, l = 10, dk = 5.5, k = 3, key = 2.5, pitch = 0.5) {
    difference() {
        union() {
            cylinder(h=k, r=dk / 2);
            translate([0, 0, -l]) {
                linear_extrude(height=l, twist=-360 * l / pitch, slices=ceil(l / pitch * 10)) {
                    translate([pitch * 0.3, 0, 0]) {
                        circle(r=d / 2 - pitch * 0.3);
                    }
                }
            }
        }
        translate([0, 0, k * 0.4]) {
            hw_hex_prism(s = key, h = k);
        }
    }
}

// Countersunk socket screw, ISO 10642. l is the overall length (head included).
module countersunk_screw(d = 3, l = 10, dk = 6.72, k = 1.86, key = 2) {
    difference() {
        union() {
            translate([0, 0, -k]) {
                cylinder(h=k, r1=d / 2, r2=dk / 2);
            }
            translate([0, 0, -l]) {
                cylinder(h=l - k, r=d / 2);
            }
        }
        translate([0, 0, -k * 0.6]) {
            hw_hex_prism(s = key, h = k);
        }
    }
}

module countersunk_screw_threaded(d = 3, l = 10, dk = 6.72, k = 1.86, key = 2, pitch = 0.5) {
    difference() {
        union() {
            translate([0, 0, -k]) {
                cylinder(h=k, r1=d / 2, r2=dk / 2);
            }
            translate([0, 0, -l]) {
                linear_extrude(height=l - k, twist=-360 * (l - k) / pitch, slices=ceil((l - k) / pitch * 10)) {
                    translate([pitch * 0.3, 0, 0]) {
                        circle(r=d / 2 - pitch * 0.3);
                    }
                }
            }
        }
        translate([0, 0, -k * 0.6]) {
            hw_hex_prism(s = key, h = k);
        }
    }
}

// Threaded rod / stud from z = 0 to z = l.
module threaded_rod(d = 3, l = 20) {
    cylinder(h=l, r=d / 2);
}

module threaded_rod_threaded(d = 3, l = 20, pitch = 0.5) {
    linear_extrude(height=l, twist=-360 * l / pitch, slices=ceil(l / pitch * 10)) {
        translate([pitch * 0.3, 0, 0]) {
            circle(r=d / 2 - pitch * 0.3);
        }
    }
}

// Hex nut, ISO 4032.
module hex_nut(d = 3, s = 5.5, m = 2.4) {
    difference() {
        hw_hex_prism(s = s, h = m);
        translate([0, 0, -1]) {
            cylinder(h=m + 2, r=d / 2);
        }
    }
}

// Prevailing torque (nylon insert) nut, ISO 10511. h is the overall height.
module nyloc_nut(d = 3, s = 5.5, m = 2.4, h = 4) {
    difference() {
        union() {
            hw_hex_prism(s = s, h = m);
            cylinder(h=h, r=s * 0.45);
        }
        translate([0, 0, -1]) {
            cylinder(h=h + 2, r=d / 2);
        }
    }
}

// Plain washer, ISO 7089.
module washer(d1 = 3.2, d2 = 7, h = 0.5) {
    difference() {
        cylinder(h=h, r=d2 / 2);
        translate([0, 0, -1]) {
            cylinder(h=h + 2, r=d1 / 2);
        }
    }
}

// ---- Holes (negative bodies) ----

// Through hole with clearance, ISO 273 medium.
module clearance_hole(d = 3.4, l = 10) {
    translate([0, 0, -l]) {
        cylinder(h=l + 1, r=d / 2);
    }
}

// Clearance hole with a counterbore for a socket head.
module counterbore_hole(d = 3.4, l = 10, cb_d = 6.5, cb_h = 3.4) {
    union() {
        translate([0, 0, -l]) {
            cylinder(h=l + 1, r=d / 2);
        }
        translate([0, 0, -cb_h]) {
            cylinder(h=cb_h + 1, r=cb_d / 2);
        }
    }
}

// Clearance hole with a 90 degree countersink for a countersunk head.
module countersink_hole(d = 3.4, l = 10, dk = 6.9, k = 1.86) {
    union() {
        translate([0, 0, -l]) {
            cylinder(h=l + 1, r=d / 2);
        }
        translate([0, 0, -k]) {
            cylinder(h=k, r1=d / 2, r2=dk / 2);
        }
        cylinder(h=1, r=dk / 2);
    }
}

// Hex nut pocket at the surface plus a clearance hole below it.
// s and m already include printing clearance.
module nut_trap(d = 3.4, s = 5.9, m = 2.8, l = 10) {
    union() {
        translate([0, 0, -l]) {
            cylinder(h=l + 1, r=d / 2);
        }
        translate([0, 0, -m]) {
            hw_hex_prism(s = s, h = m + 1);
        }
    }
}

// Pilot hole for a heat-set threaded insert.
module heat_insert_hole(d = 4, depth = 6.5) {
    translate([0, 0, -depth]) {
        cylinder(h=depth + 1, r=d / 2);
    }
}

// Printable internal thread (tapped hole) for a printed or metal screw.
module tapped_hole(d = 3, l = 10, pitch = 0.5, clearance = 0.2) {
    translate([0, 0, -l]) {
        linear_extrude(height=l + 1, twist=-360 * (l + 1) / pitch, slices=ceil((l + 1) / pitch * 10)) {
            translate([pitch * 0.3, 0, 0]) {
                circle(r=d / 2 - pitch * 0.3 + clearance);
            }
        }
    }
}
