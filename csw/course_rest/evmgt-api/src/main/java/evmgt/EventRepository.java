//package com.glenn.eventmgt.repositories;
package evmgt;

import java.time.ZoneId;

import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.data.repository.PagingAndSortingRepository;
import org.springframework.data.repository.query.Param;

import org.springframework.data.rest.core.annotation.RepositoryRestResource;

//import com.glenn.eventmgt.entities.Event;
//import com.glenn.eventmgt.entities.projections.PartialEventProjection;

import evmgt.Event;
import evmgt.PartialEventProjection;


// The following annotation makes the PartialEventProjection
// the default behavior for the get from the event resource

@RepositoryRestResource(excerptProjection=PartialEventProjection.class)
public interface EventRepository extends PagingAndSortingRepository<Event, Long> {

// turn on search by name only

// for example, client uses     url/search/findByName?name=Button Rock Hike

    Page<Event> findByName(@Param("name") String name, Pageable pageable);

// turn on search by name and zone

// for example, client uses     url/search/findByNameAndZone?
//                                  name=Button Rock Hike&
//                                  zone=US/Mountain

    Page<Event> findByZone(@Param("zone") ZoneId zoneId, Pageable pageable);

    Page<Event> findByNameAndZone(@Param("name") String name,
                                  @Param("zone") ZoneId zoneId,
                                  Pageable pageable);
}
