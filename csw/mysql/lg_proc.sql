#!/bin/bash

-- This mechanism for loading the bbox data as spatial polygons
-- is extremely slow.  I wrote java code to write brute force 
-- inserts and it is somewhat faster.  There seems to be no 
-- fast way to insert spatial data into tables.  At least none
-- that I can found.

-- I doubt this script will actually be run much, if at all.  
-- But it does have an example of a stored procedure for me
-- to reference.  (A stored procedure that I actually wrote).

-- remove procedure if previously created

drop procedure if exists load_bbox;

-- change the client side delimiter to allow the ; characters
-- in the procedure definition to be passed to the server 
-- without interference from the mysql client

delimiter ;;

create procedure load_bbox ()
begin

-- use the non @ variable definition to limit the
-- scope to this procedure

-- create and initialize loop counting variables
  declare n int default 0;
  declare i int default 0;
  declare sid bigint default -1;

-- create and initialize bounding box variables
  declare id bigint default 0;
  declare x1 double default 0;
  declare y1 double default 0;
  declare x2 double default 0;
  declare y2 double default 0;

  declare x1_s varchar(50) default '';
  declare y1_s varchar(50) default '';
  declare x2_s varchar(50) default '';
  declare y2_s varchar(50) default '';

  declare s1000 varchar(1000) default '';

  declare bad_ob_id varchar(40) default 'cant find object id in select';

-- get number of rows in objects table
  select count(*) from objects into n;

  set i = 0;

-- try to get xmin, ymin, xmax, ymax for each object
-- and use these user variables to create polygons
-- (rectangles) to insert into the objects_geom table

  bbox1: LOOP

    set id = -1;
    select object_id into id from objects where object_id = sid;
    set sid = sid + 1;

-- if no row with the sid was found try the next sid

    if id = -1 then
      iterate bbox1;
    end if;

-- use the id variable to get xmin, ymin, xmax and ymax
-- and put them into the x1, y1, x2 and y2 variables, respectively.

    select xmin into x1 from objects where object_id = id;
    select ymin into y1 from objects where object_id = id;
    select xmax into x2 from objects where object_id = id;
    select ymax into y2 from objects where object_id = id;

-- multiply by 1000000 to allow integer polygon points
-- seems like float values are not liked in some cases

    set x1 = x1 * 1000000;
    set y1 = y1 * 1000000;
    set x2 = x2 * 1000000;
    set y2 = y2 * 1000000;

-- format integer points with no commas and 0 decimal places

    set x1_s = format(x1, 0);
    set x1_s = replace(x1_s, ',' , '');
    set y1_s = format(y1, 0);
    set y1_s = replace(y1_s, ',' , '');
    set x2_s = format(x2, 0);
    set x2_s = replace(x2_s, ',' , '');
    set y2_s = format(y2, 0);
    set y2_s = replace(y2_s, ',' , '');

-- build a polygon tet string that can be passed to
-- ST_PolygonFromText

    set s1000 = 'polygon (( ' ;
    set s1000 = concat (s1000 , x1_s, ' ', y1_s, ' , ' ) ;
    set s1000 = concat (s1000 , x1_s, ' ', y2_s, ' , ' ) ;
    set s1000 = concat (s1000 , x2_s, ' ', y2_s, ' , ' ) ;
    set s1000 = concat (s1000 , x2_s, ' ', y1_s, ' , ' ) ;
    set s1000 = concat (s1000 , x1_s, ' ', y1_s, ' ))' ) ;

    insert into objects_geom values (id, ST_PolygonFromText(s1000));

    set i = i + 1;
    if i < n then
      iterate bbox1;
    end if;
    leave bbox1;
  end loop bbox1;

end;  -- end of load_bbox procedure
;;


-- change the delimiter back

delimiter ;


-- clean out rows from prior test runs

delete from objects_geom;

-- run the stored procedure

call load_bbox;

create spatial index bbox on objects_geom (bbox);

drop procedure if exists load_bbox;

