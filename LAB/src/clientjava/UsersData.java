package src.clientjava;

/**
 * Class data structure used to store the ip and port 
 * of connnected users received on call to list_users()
 */
public class UsersData {
    private String ip;
    private String port;

    /* Co
    UsersData(String ip, String port) {
        this.ip = ip;
        this.port = port;
    }

    /* Getter methods */
    public String getPort (){
        return port;
    }

    public String getIp () {
        return ip;
    }
}