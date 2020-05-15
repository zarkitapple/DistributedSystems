#ifndef _PEERTOPEERRPC_
#define _PEERTOPEERRPC_

#include "../serverrpc/storage.h"
#include "../../lib/lines.h"
#include <string.h>
#include <arpa/inet.h>

#define MAX_SIZE 256
#define PORT_SIZE 7
#define IP_SIZE 16

/*****************************************************************************
 *                                                                           *
 *  Server functions to implement the peer to peer functionalities           *
 *  specified. All the set of functions defined here serve as implementation *
 *  for the system using RPCs.Client connection is made throught a manager   *
 *  function, which also                                                     *
 *  processes the return values of the following functions, expect for       *
 *  list_contents and list_users.                                            *
 *                                                                           *
 *  Authors:     Pablo Alejandre Blanco                                      *
 *               Mario Pérez Enríquez                                        *
 *                                                                           *
 *  Last Revision Date:        17/05/2020                                    *
 *                                                                           *
 ****************************************************************************/

/* 
 * FUNCTION init_p2p_service initializes the peer to peer service by
 *              creating the data control mechanisms and data structures required
 *              by the service, using remote procedure calls (RPC)    
 */
void init_p2p_service_rpc();


/* 
 * FUNCTION register_user_rpc registers a new user provided it does not exist yet.
 * 
 * ARGS     socket is the socket stablishing connection with the client
 *              requesting the operation to perform. Data streams are read 
 *              from this socket. The number of streams varies according
 *              to the expected inputs of the request.
 * 
 * RETURN   0 If successful registration.
 *          1 If user already registered.
 *          2 In any other case.
 */
int register_user_rpc (int socket);

/* 
 * FUNCTION unregister_user_rpc unregisters a new user provided it exists.
 * 
 * ARGS     socket is the socket stablishing connection with the client
 *              requesting the operation to perform. Data streams are read 
 *              from this socket. The number of streams varies according
 *              to the expected inputs of the request.
 * 
 * RETURN   0 If successful unregistration.
 *          1 If user is not registered (not exists).
 *          2 In any other case.
 */
int unregister_user_rpc (int socket);

/* 
 * FUNCTION connect_user_rpc lets a registered user stablish a connection.
 * 
 * ARGS     socket is the socket stablishing connection with the client 
 *              requesting the operation to perform. Data streams are read 
 *              from this socket. The number of streams varies according
 *              to the expected inputs of the request.
 * 
 * RETURN   0 If successful connection.
 *          1 If user is not registered (not exists).
 *          2 If user already connected.
 *          3 In any other case.
 */
int connect_user_rpc (int socket);

/* 
 * FUNCTION disconnect_user_rpc lets a registered user end a connection.
 * 
 * ARGS     socket is the socket stablishing connection with the client 
 *              requesting the operation to perform. Data streams are read 
 *              from this socket. The number of streams varies according
 *              to the expected inputs of the request.
 * 
 * RETURN   0 If successful disconnection.
 *          1 If user is not registered (not exists).
 *          2 If user not connected.
 *          3 In any other case.
 */
int disconnect_user_rpc(int socket);

/* 
 * FUNCTION publish_file_rpc lets a registered user publish a file in the server.
 * 
 * ARGS     socket is the socket stablishing connection with the client 
 *              requesting the operation to perform. Data streams are read 
 *              from this socket. The number of streams varies according
 *              to the expected inputs of the request.
 * 
 * RETURN   0 If successful publishment.
 *          1 If user is not registered (not exists).
 *          2 If user not connected.
 *          3 If file already published.
 *          4 In any other case.
 */
int publish_file_rpc (int socket);

/* 
 * FUNCTION delete_file_rpc lets a registered user delete a file from the server.
 * 
 * ARGS     socket is the socket stablishing connection with the client 
 *              requesting the operation to perform. Data streams are read 
 *              from this socket. The number of streams varies according
 *              to the expected inputs of the request.
 * 
 * RETURN   0 If successful deletion.
 *          1 If user is not registered (not exists).
 *          2 If user not connected.
 *          3 If file not published.
 *          4 In any other case.
 */
int delete_file_rpc(int socket);

/* 
 * FUNCTION list_users_rpc lists the users that are currently connected.
 * 
 * ARGS     socket is the socket stablishing connection with the client 
 *              requesting the operation to perform. Data streams are read 
 *              from this socket. The number of streams varies according
 *              to the expected inputs of the request.
 * 
 * CLIENT RETURN    
 *                  Exec. Code
 *                      0 If successful listing of users.
 *                      1 If user is not registered (not exists).
 *                      2 If user not connected.
 *                      3 In any other case.
 *                  Data
 *                      List of user datas (name, IP, port).
 */
void list_users_rpc (int socket);

/* 
 * FUNCTION list_content_rpc lists content uploaded by a user, on request by
 *              another user.
 * 
 * ARGS     socket is the socket stablishing connection with the client 
 *              requesting the operation to perform. Data streams are read 
 *              from this socket. The number of streams varies according
 *              to the expected inputs of the request.
 * 
 * CLIENT RETURN    
 *                  Exec. Code
 *                      0 If successful listing of content.
 *                      1 If user is not registered (not exists).
 *                      2 If not connected.
 *                      3 In any other case.
 *                  Data
 *                      Files in user directory.
 */
void list_content_rpc (int socket);

#endif