//package com.glenn.eventmgt.entities;
package evmgt;

import java.time.ZoneId;
import java.time.ZonedDateTime;
import java.util.Objects;
import java.util.Set;

import javax.persistence.CascadeType;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.Column;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;

import org.springframework.data.rest.core.annotation.RestResource;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;

@JsonPropertyOrder({"resourceId"})
@Entity
public class Event extends BaseEntity {
    private String  name;
    private String description;
    private ZonedDateTime start_time;
    private ZonedDateTime end_time;

// The zone member was originally ycalled zone_id or zoneId and 
// the jpa shat itself unpredictably.  Probably best not to use
// and variable, here and in the data store column name, that has
// camel case Id or _id.  To be safe, don't use id at all.

    private ZoneId zone;

    @Column(columnDefinition="int default 0")
    private Boolean  started = false;

    @ManyToOne(fetch = FetchType.EAGER)
    @JoinColumn(nullable = false)
    private Organizer organizer;
    @OneToMany(mappedBy="event", fetch = FetchType.EAGER,
                   cascade = CascadeType.REMOVE, orphanRemoval = true)
    private Set<Participant> participants;
    @ManyToOne(fetch = FetchType.EAGER, cascade = CascadeType.ALL)
    // why is this nullable?
    @RestResource(exported = false)
    private Venue  venue;
    
    public String getName() {
        return name;
    }
    public void setName(String name) {
        this.name = name;
    }
    public String getDescription() {
        return description;
    }
    public void setDescription(String description) {
        this.description = description;
    }
    public ZonedDateTime getStart_time() {
        return start_time;
    }
    public void setStart_time(ZonedDateTime start_time) {
        this.start_time = start_time;
    }
    public ZonedDateTime getEnd_time() {
        return end_time;
    }
    public void setEnd_time(ZonedDateTime end_time) {
        this.end_time = end_time;
    }
    public ZoneId getZone() {
        return zone;
    }
    public void setZone(ZoneId zone) {
        this.zone = zone;
    }
    public Boolean getStarted() {
        return started;
    }
    public void setStarted(Boolean started) {
        this.started = started;
    }
    public Organizer getOrganizer() {
        return organizer;
    }
    public void setOrganizer(Organizer organizer) {
        this.organizer = organizer;
    }
    public Set<Participant> getParticipants() {
        return participants;
    }
    public void setParticipants(Set<Participant> participants) {
        this.participants = participants;
    }
    public Venue getVenue() {
        return venue;
    }
    public void setVenue(Venue venue) {
        this.venue = venue;
    }

    public Long getResourceId () {
        return this.id;
    }
    
    @Override
    public boolean equals (Object obj) {
        Event ev = (Event)obj;
        return (id == ev.id);
    }
    
    @Override
    public int hashCode() {
        return Objects.hash(id);
    }
    
}
