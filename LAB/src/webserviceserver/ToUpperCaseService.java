package src.webserviceserver;

import javax.jws.WebService;
import javax.jws.WebMethod;

/**
 * ToUpperCase
 */
@WebService
public class ToUpperCaseService {

    @WebMethod
    public String toUppercase (String string){

        return string.toUpperCase();
    }
    
}
