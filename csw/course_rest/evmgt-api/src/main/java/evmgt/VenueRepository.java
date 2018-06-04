//package com.glenn.eventmgt.repositories;
package evmgt;

import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.data.repository.query.Param;

import org.springframework.data.repository.PagingAndSortingRepository;

//import com.glenn.eventmgt.entities.Venue;
import evmgt.Venue;

public interface VenueRepository extends PagingAndSortingRepository<Venue, Long> {

// for example, client uses     url/search/findByPostal?postal=12345

// The findBy naming requirements are fairly fragile.  I (Glenn) have never
// had something like postal_code (with underscore) work.  The api servelet
// does not start (using gradlew bootRun).  When the variable is postalcode
// without underscore, the servelet starts up.

    Page<Venue> findByPostalcode
        (@Param("postalcode") String postalcode, Pageable pageable);

// I cannot get postalCode as the variable name to work.  The docs
// on this naming stuff are undecipherable.  I give up and will use
// only all lower case field names, with the names identical in the
// java code and in the database.  When trying to post a venue object,
// the exception complains about no postal_code field available.  I
// have no idea why it wants the lower case underscore name when camel
// case is used every where in the code and in the table column name.

/*
    Page<Venue> findByPostalCode
        (@Param("postalCode") String postalcode, Pageable pageable);
*/

}
