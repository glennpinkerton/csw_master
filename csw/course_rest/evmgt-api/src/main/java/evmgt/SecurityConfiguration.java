
// Clone of course security config class.  As expected, the course
// code does not work.  No surprise at this point.

package evmgt;


/*
import org.springframework.context.annotation.Configuration;
import org.springframework.http.HttpMethod;
import org.springframework.security.config.annotation.authentication.builders.AuthenticationManagerBuilder;
import org.springframework.security.config.annotation.method.configuration.EnableGlobalMethodSecurity;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.WebSecurityConfigurerAdapter;




@Configuration
@EnableGlobalMethodSecurity(prePostEnabled = true)
public class SecurityConfiguration extends WebSecurityConfigurerAdapter {

    @Override
    protected void configure(AuthenticationManagerBuilder auth) throws Exception {
        auth.inMemoryAuthentication().
          withUser("glenn").password("jessie-dog").roles("USER").and().
          withUser("admin").password("jessie-dog").roles("ADMIN");
    }

    @Override
    protected void configure(HttpSecurity http) throws Exception {
        http.httpBasic().and().
        authorizeRequests().
          antMatchers(HttpMethod.POST, "/organizers").hasRole("ADMIN").
          antMatchers(HttpMethod.PUT, "/organizers/**").hasRole("ADMIN").
          antMatchers(HttpMethod.PATCH, "/organizers/**").hasRole("ADMIN").and().
        authorizeRequests().
          antMatchers(HttpMethod.POST, "/events").hasRole("ADMIN").
          antMatchers(HttpMethod.PUT, "/events/**").hasRole("ADMIN").
          antMatchers(HttpMethod.PATCH, "/events/**").hasRole("ADMIN").and().
        csrf().disable();
    }

}

*/




// Supposedly security code from another web tutorial.  This does not work either.

/*
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
import org.springframework.security.config.annotation.web.configuration.WebSecurityConfigurerAdapter;
import org.springframework.security.core.userdetails.User;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.core.userdetails.UserDetailsService;
import org.springframework.security.provisioning.InMemoryUserDetailsManager;


@Configuration
@EnableWebSecurity
public class WebSecurityConfig extends WebSecurityConfigurerAdapter {
    @Override
    protected void configure(HttpSecurity http) throws Exception {
        http
            .authorizeRequests()
//                .antMatchers("/", "/home").permitAll()
//                .anyRequest().authenticated()
                .anyRequest().permitAll();
//                .and()
//            .formLogin()
//                .loginPage("/login")
//                .permitAll()
//                .and()
//            .logout()
//                .permitAll();
    }

    @Bean
    @Override
    public UserDetailsService userDetailsService() {
        UserDetails user =
             User.withDefaultPasswordEncoder()
                .username("glenn")
                .password("jessie-dog")
                .roles("ADMIN")
                .build();

        return new InMemoryUserDetailsManager(user);
    }

}

*/
