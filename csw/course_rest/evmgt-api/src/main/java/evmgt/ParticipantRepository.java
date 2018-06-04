//package com.glenn.eventmgt.repositories;
package evmgt;

import org.springframework.data.repository.PagingAndSortingRepository;

import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.data.repository.query.Param;

//import com.glenn.eventmgt.entities.Participant;
import evmgt.Participant;

public interface ParticipantRepository extends PagingAndSortingRepository<Participant, Long> {

    Page<Participant> findByEmail
        (@Param("email") String email, Pageable pageable);


}
