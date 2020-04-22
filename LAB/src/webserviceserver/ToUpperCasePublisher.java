package src.webserviceserver;

import javax.xml.ws.Endpoint;

public class ToUpperCasePublisher {
    public static void main(String[] args) {
        final String url = "http://localhost:8888/rs";
        System.err.println("Publishing ToLowerCase service at endpoint "+url);
        Endpoint.publish(url, new ToUpperCaseService());
    }   
}