//package com.glenn.eventmgt.entities;
package evmgt;

import java.util.Objects;

import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;

@JsonPropertyOrder({"resourceId"})
@Entity
public class Participant extends BaseEntity {
    @Column
    private String  name;
    @Column
    private String  email;
    private Boolean checkedin;
    @ManyToOne (fetch = FetchType.EAGER, cascade = CascadeType.ALL)
    @JoinColumn (referencedColumnName="ID", nullable=false, updatable=false)
    private Event   event;
    
    public Long getId() {
        return id;
    }
    public void setId(Long id) {
        this.id = id;
    }
    public String getName() {
        return name;
    }
    public void setName(String name) {
        this.name = name;
    }
    public String getEmail() {
        return email;
    }
    public void setEmail(String email) {
        this.email = email;
    }
    public Boolean getCheckedin() {
        return checkedin;
    }
    public void setCheckedin(Boolean checkedin) {
        this.checkedin = checkedin;
    }
    public Event getEvent() {
        return event;
    }
    public void setEvent(Event event) {
        this.event = event;
    }
    
    public Long getResourceId () {
        return this.id;
    }

    @Override
    public boolean equals (Object obj) {
        Participant ev = (Participant)obj;
        return (id == ev.id);
    }
    
    @Override
    public int hashCode() {
        return Objects.hash(id);
    }
    
}
