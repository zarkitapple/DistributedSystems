
package src.clientjava.touppercase;

import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebResult;
import javax.jws.WebService;
import javax.xml.bind.annotation.XmlSeeAlso;
import javax.xml.ws.Action;
import javax.xml.ws.RequestWrapper;
import javax.xml.ws.ResponseWrapper;


/**
 * This class was generated by the JAX-WS RI.
 * JAX-WS RI 2.2.9-b130926.1035
 * Generated source version: 2.2
 * 
 */
@WebService(name = "ToUpperCaseService", targetNamespace = "http://webserviceserver.src/")
@XmlSeeAlso({
    ObjectFactory.class
})
public interface ToUpperCaseService {


    /**
     * 
     * @param arg0
     * @return
     *     returns java.lang.String
     */
    @WebMethod
    @WebResult(targetNamespace = "")
    @RequestWrapper(localName = "toUppercase", targetNamespace = "http://webserviceserver.src/", className = "touppercase.ToUppercase")
    @ResponseWrapper(localName = "toUppercaseResponse", targetNamespace = "http://webserviceserver.src/", className = "touppercase.ToUppercaseResponse")
    @Action(input = "http://webserviceserver.src/ToUpperCaseService/toUppercaseRequest", output = "http://webserviceserver.src/ToUpperCaseService/toUppercaseResponse")
    public String toUppercase(
        @WebParam(name = "arg0", targetNamespace = "")
        String arg0);

}
