//package com.glenn.eventmgt.entities;
package evmgt;

import java.util.Objects;

import javax.persistence.Entity;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;

@JsonPropertyOrder({"resourceId"})
@Entity
public class Venue extends BaseEntity {
    private String  name;
    private String  street_address;
    private String  street_address2;
    private String  city;
    private String  state;
    private String  country;
    private String  postalcode;
    
    public String getName() {
        return name;
    }
    public void setName(String name) {
        this.name = name;
    }
    public String getStreet_address() {
        return street_address;
    }
    public void setStreet_address(String street_address) {
        this.street_address = street_address;
    }
    public String getStreet_address2() {
        return street_address2;
    }
    public void setStreet_address2(String street_address2) {
        this.street_address2 = street_address2;
    }
    public String getCity() {
        return city;
    }
    public void setCity(String city) {
        this.city = city;
    }
    public String getState() {
        return state;
    }
    public void setState(String state) {
        this.state = state;
    }
    public String getCountry() {
        return country;
    }
    public void setCountry(String country) {
        this.country = country;
    }
    public String getPostalcode() {
        return postalcode;
    }
    public void setPostalcode(String postalcode) {
        this.postalcode = postalcode;
    }
    
    public Long getResourceId () {
        return this.id;
    }

    @Override
    public boolean equals (Object obj) {
        Venue ev = (Venue)obj;
        return (id == ev.id);
    }
    
    @Override
    public int hashCode() {
        return Objects.hash(id);
    }
    
}
