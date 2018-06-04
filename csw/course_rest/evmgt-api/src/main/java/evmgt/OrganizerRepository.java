//package com.glenn.eventmgt.repositories;
package evmgt;

//import org.springframework.data.repository.CrudRepository;
import org.springframework.data.repository.PagingAndSortingRepository;

//import com.glenn.eventmgt.entities.Organizer;
import evmgt.Organizer;

public interface OrganizerRepository extends PagingAndSortingRepository<Organizer, Long> {

}
