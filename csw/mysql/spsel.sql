#!/bin/bash

use glp;

-- set @bbpoly = 'polygon ((50000000 50000000, 50000000 53000000, 53000000 53000000,
--                           53000000 50000000, 50000000 50000000))';

set @bbpoly = 'polygon ((30000000 30000000, 30000000 45000000, 45000000 45000000,
                          45000000 30000000, 30000000 30000000))';

select pline_points_lookup.* from objects,objects_geom,pline_id_lookup,pline_points_lookup
  where
     (MBRIntersects(ST_GeomFromText(@bbpoly), objects_geom.bbox))
   and
     (objects.object_id = objects_geom.object_id)
   and
     (pline_id_lookup.object_id = objects.object_id)
   and
     (pline_points_lookup.line_id = pline_id_lookup.line_id)
;
