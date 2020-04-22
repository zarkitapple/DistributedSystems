package src.clientjava;

public class UsersData {
    private String ip;
    private String port;

    UsersData(String ip, String port) {
        this.ip = ip;
        this.port = port;
    }

    public String getPort (){
        return port;
    }

    public String getIp () {
        return ip;
    }
}