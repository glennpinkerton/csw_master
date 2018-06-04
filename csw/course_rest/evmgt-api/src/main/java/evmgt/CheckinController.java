package evmgt;

import java.util.Optional;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.rest.webmvc.RepositoryRestController;
import org.springframework.data.rest.webmvc.ResourceNotFoundException;

//import org.springframework.stereotype.Controller;

import org.springframework.data.rest.webmvc.PersistentEntityResource;
import org.springframework.data.rest.webmvc.PersistentEntityResourceAssembler;

//import org.springframework.http.HttpHeaders;
import org.springframework.http.ResponseEntity;

import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.PostMapping;
//import org.springframework.web.bind.annotation.GetMapping;
//import org.springframework.web.bind.annotation.RequestParam;
//import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.bind.annotation.PathVariable;

import evmgt.Participant;
import evmgt.ParticipantRepository;


@RepositoryRestController 
@RequestMapping(path="/participants") // This means URL's start with /participants
                                // (after Application path)
public class CheckinController {

    @Autowired // This means to get the bean called userRepository
               // Which is auto-generated by Spring, we will use it to handle the data
    private ParticipantRepository participantRepository;
    

// The tutorial is not correct in how it uses ResponseEntity.  The respnse entity
// is a generic class and needs a type in the <>.  Also, the ResponceEntity
// contains a status code, a body and headers.  The string from the tutorial
// is properly in the body of the response entity.

    @PostMapping("/checkin/{id}")
    public ResponseEntity<String> checkin (@PathVariable Long id)
        throws Exception
    {
        Optional<Participant>  op_par = participantRepository.findById(id);

        Participant  par = null;
        if (op_par.isPresent()) {
          par = op_par.get();
        }

        if (par == null) {
            throw (new ResourceNotFoundException());
        }

  // Could write a custom exception but I really don't care

        if (par.getCheckedin ()) {
            String cistr = par.getName() + " is already checked in.";
            throw new Exception (cistr);
        }

        par.setCheckedin (true);
        participantRepository.save (par);

        String  str = par.getName() + " checked in using CheckinController";

  // Looks like the ok static method is a shortcut for returning an
  // object of the <> type.

        return ResponseEntity.ok(str);
        
    }


// method to return a HAL linked response, if it works I will be surprised.

    @PostMapping("/hcheckin/{id}")
    public ResponseEntity<PersistentEntityResource>
           hcheckin (@PathVariable Long id,
                    PersistentEntityResourceAssembler assembler)
        throws Exception
    {
        Optional<Participant>  op_par = participantRepository.findById(id);

        Participant  par = null;
        if (op_par.isPresent()) {
          par = op_par.get();
        }

        if (par == null) {
            throw (new ResourceNotFoundException());
        }

  // Could write a custom exception but I really don't care

        if (par.getCheckedin ()) {
            String cistr = par.getName() + " is already checked in.";
            throw new Exception (cistr);
        }

        par.setCheckedin (true);
        participantRepository.save (par);

      // Use the passed assembler to return the participant resource

        return ResponseEntity.ok(assembler.toResource(par));
        
    }


// If the participant is checked in, check out
// method to return a HAL linked response, if it works I will be surprised.

    @PostMapping("/hcheckout/{id}")
    public ResponseEntity<PersistentEntityResource>
           hcheckout (@PathVariable Long id,
                    PersistentEntityResourceAssembler assembler)
        throws Exception
    {
        Optional<Participant>  op_par = participantRepository.findById(id);

        Participant  par = null;
        if (op_par.isPresent()) {
          par = op_par.get();
        }

        if (par == null) {
            throw (new ResourceNotFoundException());
        }

  // Could write a custom exception but I really don't care

        if (par.getCheckedin () == false) {
            String cistr = par.getName() + " is not currently checked in.";
            throw new Exception (cistr);
        }

        par.setCheckedin (false);
        participantRepository.save (par);

      // Use the passed assembler to return the participant resource

        return ResponseEntity.ok(assembler.toResource(par));
        
    }

}
