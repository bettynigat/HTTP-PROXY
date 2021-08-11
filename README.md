# HTTP-PROXY

The purpose of this project is to implement a ***simple web proxy*** that passes requests, responses, and data between a web client and a web server. 

### HTTP 
HTTP is the protocol used for communication on the web. It is the protocol that defines how
your web browser requests resources from a web server and how the server responds. 

### HTTP-PROXY
Ordinarily, **HTTP** is a client-server protocol. However, in some circumstances, it may be useful
to introduce an intermediate entity called a [**proxy**](https://en.wikipedia.org/wiki/Proxy_server). Conceptually,the proxy sits between the 
client and the server. In the simplest case, instead of sending requests directly 
to the server, the client sends all its requests to the proxy. The proxy then opens a connection
to the server and passes on the client’s request. The proxy receives a reply from the server, 
and then sends that reply back to the client. That is, the proxy essentially acts like both an
HTTP client (to the remote server) and an HTTP server (to the initial client). 

In ths project, we build a basic web proxy capable of: 
1. Accepting HTTP requests from clients,
2. Sending requests to / Receiving responses from remote servers, and
3. Returning data to a client.

Our proxy also does the following things. 
- Serve multiple clients
- Work in real web browsers
- Support HTTP redirection functionality 
