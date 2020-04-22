
package src.clientjava.touppercase;

import javax.xml.bind.JAXBElement;
import javax.xml.bind.annotation.XmlElementDecl;
import javax.xml.bind.annotation.XmlRegistry;
import javax.xml.namespace.QName;


/**
 * This object contains factory methods for each 
 * Java content interface and Java element interface 
 * generated in the touppercase package. 
 * <p>An ObjectFactory allows you to programatically 
 * construct new instances of the Java representation 
 * for XML content. The Java representation of XML 
 * content can consist of schema derived interfaces 
 * and classes representing the binding of schema 
 * type definitions, element declarations and model 
 * groups.  Factory methods for each of these are 
 * provided in this class.
 * 
 */
@XmlRegistry
public class ObjectFactory {

    private final static QName _ToUppercaseResponse_QNAME = new QName("http://webserviceserver.src/", "toUppercaseResponse");
    private final static QName _ToUppercase_QNAME = new QName("http://webserviceserver.src/", "toUppercase");

    /**
     * Create a new ObjectFactory that can be used to create new instances of schema derived classes for package: touppercase
     * 
     */
    public ObjectFactory() {
    }

    /**
     * Create an instance of {@link ToUppercaseResponse }
     * 
     */
    public ToUppercaseResponse createToUppercaseResponse() {
        return new ToUppercaseResponse();
    }

    /**
     * Create an instance of {@link ToUppercase }
     * 
     */
    public ToUppercase createToUppercase() {
        return new ToUppercase();
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link ToUppercaseResponse }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://webserviceserver.src/", name = "toUppercaseResponse")
    public JAXBElement<ToUppercaseResponse> createToUppercaseResponse(ToUppercaseResponse value) {
        return new JAXBElement<ToUppercaseResponse>(_ToUppercaseResponse_QNAME, ToUppercaseResponse.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link ToUppercase }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://webserviceserver.src/", name = "toUppercase")
    public JAXBElement<ToUppercase> createToUppercase(ToUppercase value) {
        return new JAXBElement<ToUppercase>(_ToUppercase_QNAME, ToUppercase.class, null, value);
    }

}
