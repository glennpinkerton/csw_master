//package com.glenn.eventmgt.entities.projections;
package evmgt;

import java.time.Instant;
import java.time.ZonedDateTime;

import org.springframework.data.rest.core.config.Projection;

//import com.glenn.eventmgt.entities.Event;
import evmgt.Event;


@Projection(name = "partial", types = { Event.class })
public interface PartialEventProjection {

    String getName();

    ZonedDateTime getStart_time();

    ZonedDateTime getEnd_time();
    
    Instant getCreated();
}
