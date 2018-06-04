package evmgt;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.autoconfigure.domain.EntityScan;
import org.springframework.data.convert.Jsr310Converters;

@SpringBootApplication
@EntityScan (basePackageClasses = {EvmgtApiApplication.class, Jsr310Converters.class})
public class EvmgtApiApplication {

    public static void main(String[] args) {
        SpringApplication.run(EvmgtApiApplication.class, args);
    }
}
