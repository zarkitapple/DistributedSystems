
struct connect_arguments {
    string name<256>;
    string port<7>;
    string ip<16>;
};

struct publish_arguments {
    string name<256>;
    string file_name<256>;
    string file_description<256>;
};

struct delete_arguments {
    string name<256>;
    string file_name<256>;
};

struct UserServer {
  string name<256>;
  string port<7>;
  string ip<16>;
};

typedef struct UserServer UserList<>;

struct Title {
    string file_name<256>;
};

typedef struct Title TitleList<>;

program STORAGE {
    version STORAGEVER {
        int REGISTER_USER_RPC(string<256>) = 1;
        int UNREGISTER__USER_RPC(string<256>) = 2;
        int CONNECT_USER_RPC(connect_arguments) = 3;
        int DISCONNECT_USER_RPC(string<256>) = 4;
        int PUBLISH_FILE_RPC(publish_arguments) = 5;
        int DELETE_FILE_RPC(delete_arguments) = 6;
        UserList LIST_USERS(string<256>) = 7;
        TitleList LIST_CONTENT(string<256>,string<256>) = 8;
        int IS_USER_REGISTERED(string<256>) = 9;
        int IS_USER_CONNECTED(string<256>) = 10;
    } = 1;
} = 1114;