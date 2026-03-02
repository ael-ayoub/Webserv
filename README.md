*This project has been created as part of the 42 curriculum by asoudani.*

# Webserv - HTTP Server Implementation

## Description

Webserv is a lightweight HTTP server written in C++98 that implements core web server functionality for handling client requests over TCP/IP. The server uses Linux `epoll` for efficient event-based I/O multiplexing, allowing it to handle multiple concurrent connections.

**Key Features:**
- HTTP/1.1 request parsing and response generation
- GET, POST, and DELETE method support
- CGI (Common Gateway Interface) execution for dynamic content
- Directory listing with autoindex capability
- File uploads and management
- Custom error pages
- Location-based routing with flexible configuration
- Multi-server and multi-location support via configuration files

## Instructions

### Compilation

```sh
make
```

Compile flags can be modified in the `Makefile`. The project requires C++98 compliance and standard Unix/Linux libraries.

### Execution

```sh
./Webserv [config_file]
```

- If no config file is specified, the server uses `SRC/default.conf`
- By default, the server listens on `localhost:1033` and `localhost:1037`

### Configuration

Edit `SRC/default.conf` to customize:
- **Listen addresses and ports** - Change `listen` directives
- **Server name** - Set `server_name`
- **Root directory** - Specify document root with `root`
- **Allowed methods** - Define `methods` (GET, POST, DELETE)
- **CGI interpreters** - Map file extensions to interpreters with `cgi_pass`
- **Error pages** - Customize error responses
- **Upload limits** - Set `client_max_body_size`

**Example Configuration:**
```conf
server {
    listen localhost:8080;
    server_name myserver;
    client_max_body_size 5000000;
    
    location /upload/ {
        root /www;
        methods GET POST DELETE;
        autoindex on;
        cgi_pass .py /usr/bin/python3;
        cgi_pass .php /usr/bin/php;
    }
    
    location / {
        root /www;
        methods GET POST DELETE;
        index index.html;
        autoindex on;
    }
}
```

### Web Interface

- **Home page:** `http://localhost:1033/`
- **Upload form:** `http://localhost:1033/upload.html`
- **Upload manager:** `http://localhost:1033/uploads_manager.html`
- **Static files:** Serve from `/www/` directory
- **CGI scripts:** Execute scripts from `/www/cgi/`

## Resources

### HTTP & Web Server Documentation
- [HTTP/1.1 Specification (RFC 7230-7235)](https://tools.ietf.org/html/rfc7230)
- [CGI Specification (RFC 3875)](https://tools.ietf.org/html/rfc3875)
- [MDN HTTP Guide](https://developer.mozilla.org/en-US/docs/Web/HTTP)

### Linux I/O Multiplexing
- [epoll man page](https://man7.org/linux/man-pages/man7/epoll.7.html)
- [Linux Programming Interface (Chapter 63)](https://man7.org/tlpi/)

### Web Server Architecture
- [How Web Servers Work](https://www.nginx.com/blog/inside-nginx-how-we-designed-for-performance-scale/)
- [Socket Programming Guide](https://beej.us/guide/bgnet/)
