//package com.glenn.eventmgt.entities.projections;
package evmgt;

import java.time.Instant;
import java.time.ZonedDateTime;

import org.springframework.data.rest.core.config.Projection;

//import com.glenn.eventmgt.entities.Participant;
import evmgt.Participant;


@Projection(name = "partialPar", types = { Participant.class })
public interface PartialParticipantProjection {

    String getName();

    Boolean getCheckedin();
}
