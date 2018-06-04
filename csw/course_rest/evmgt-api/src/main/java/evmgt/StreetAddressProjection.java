//package com.glenn.eventmgt.entities.projections;
package evmgt;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.data.rest.core.config.Projection;

//import com.glenn.eventmgt.entities.Venue;
import evmgt.Venue;

@Projection(name = "virtual", types = { Venue.class })
public interface StreetAddressProjection {

	@Value("#{target.street_address} #{target.street_address2}")
	String getCompleteStreetAddress();
}
