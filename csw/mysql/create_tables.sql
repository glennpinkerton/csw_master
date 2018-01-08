#!/bin/bash

-- This sql script is used as part of experimenting with a trivial
-- spatial data base development.  It is absolutely not meant for
-- use with an actual "real" data base.

-- I originally used unsigned sql data types for various id's.
-- Since Java does not support unsigned (at least not in a simple 
-- fashion), I changed to signed types.

use glp;

-- remove all tables and delete their contents

drop table objects;
drop table objects_geom;
drop table pline_id_lookup;
drop table global_stuff;
drop table pline_points_lookup;

-- The objects table is a high level table
-- for graphical (spatial) objects.  The
-- type 1 objects are single points, and their
-- location is in xmin, ymin.  The type 2 objects
-- are line related.  Their points are found via
-- a lineid lookup in the pline_id_lookup table.

create table objects (
    object_id bigint not null primary key,
    type tinyint not null,
    layer_id smallint,
    xmin double not null,
    ymin double not null,
    xmax double not null,
    ymax double not null
);


-- Separate the spatial indexable bbox into its own table.
-- Loading spatial data into mysql is a pain in the ass.
-- The pain is somewhat lessened by making this table very
-- simple.

create table objects_geom (
    object_id bigint not null primary key,
    bbox geometry not null
  );

-- The pline_id_lookup table relates the id numbers for polylines
-- to the object id numbers.  This is a many to many relationship. 
-- An object can and probably will have more than one polyline and
-- a polyline can be used by more than one object.

create table pline_id_lookup (
    object_id bigint not null,
    line_id bigint not null,
    primary key (object_id, line_id)
  );

-- The point_parent_dir table has "global" stuff needed
-- to access the point files and to convert the int point
-- coordinates to "world" coordinates.

create table global_stuff (
    dirname  varchar(100) not null primary key,
    xmin double not null,
    ymin double not null,
    xmax double not null,
    ymax double not null,
    ixmin int  not null,
    iymin int  not null,
    ixmax int  not null,
    iymax int  not null
  );

-- I could not get mysql to use a datadir other than the default.
-- (mysql 5.7.20 running on fedora 26 (4.13.16-202.fc26.x86_64))
-- I tried over 10 suggestions from the internet and none worked.
-- After a couple of days I gave up.  So, the points are stored in
-- binary files on my big disk, and the other stuff for the lines,
-- objects, etc are stored in the glp database symbolically linked
-- to the default mysql data area.  The files are in the dirname
-- specified in the point_parent_dir table defined above.  Each
-- file has the name:   glp_points_"file_id".dat.

create table pline_points_lookup (
    line_id bigint not null primary key,
    file_id int not null,
    file_pos int not null
  );
