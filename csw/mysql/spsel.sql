#!/bin/bash

use glp;

set @bbpoly = 'polygon ((20000000 20000000, 20000000 45000000, 45000000 45000000,
                          45000000 20000000, 20000000 20000000))';

select objects.object_id,pline_points_lookup.*
  from objects,objects_geom,pline_id_lookup,pline_points_lookup
  where
     (MBRIntersects(ST_GeomFromText(@bbpoly), objects_geom.bbox))
   and
     (objects.object_id = objects_geom.object_id)
   and
     (pline_id_lookup.object_id = objects.object_id)
   and
     (pline_points_lookup.line_id = pline_id_lookup.line_id)
   order by pline_points_lookup.file_id, pline_points_lookup.file_pos
;
