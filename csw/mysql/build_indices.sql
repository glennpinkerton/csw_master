#!/bin/bash

use glp;

create index layer_id on objects(layer_id);

create spatial index bbox on objects_geom(bbox);

create index file_id_pos on pline_points_lookup(file_id, file_pos);

