//package com.glenn.eventmgt.entities;
package evmgt;

import java.util.Objects;
import java.util.Set;

import javax.persistence.Entity;
import javax.persistence.OneToMany;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;

@JsonPropertyOrder({"resourceId"})
@Entity
public class Organizer extends BaseEntity {
    private String name;
    @OneToMany(mappedBy = "organizer")
    private Set<Event> events;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Set<Event> getEvents() {
        return events;
    }

    public void setEvents(Set<Event> events) {
        this.events = events;
    }
    
    @Override
    public boolean equals (Object obj) {
        Organizer ev = (Organizer)obj;
        return (id == ev.id);
    }

    public Long getResourceId () {
        return this.id;
    }
    
    @Override
    public int hashCode() {
        return Objects.hash(id);
    }
    
}
