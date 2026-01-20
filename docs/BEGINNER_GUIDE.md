# Beginner Guide — Webserv

This document explains **how this specific repo works**, end-to-end: sockets → `epoll` loop → reading requests → parsing HTTP → mapping to a `server {}` + `location {}` → executing GET/POST/DELETE → serving static files / directory listing → CGI → uploads → how to test.

Everything below references real files in this project (paths are clickable in VS Code).

---

## 0) Mental model (the big picture)

At runtime the process has **two jobs**:

1. **Wait for network events** (new connections, data ready to read) using `epoll`.
2. **For each client**: read the HTTP request, parse it, then generate an HTTP response and `send()` it back.

High-level flow:

```
main()
  -> run_server()                      (SRC/runServer.cpp)
      -> parse config (default.conf)   (SRC/ConfigFile/*.cpp)
      -> Socket::run()                 (SRC/Socket.cpp)
          -> CreateSocket()            (socket/bind/listen)
          -> CreateEpoll()             (epoll_create + epoll_ctl)
          -> Monitor()                 (epoll_wait loop)
              -> accept() new clients
              -> HandleClient()        (SRC/client.cpp)
                  -> read header/body
                  -> Request::parse_request()
                  -> Methodes::GetMethod() OR upload/session logic
                  -> Response::* helpers
                  -> send response
```

---

## 1) Project structure (what lives where)

- `main.cpp` — entry point.
- `SRC/runServer.cpp` — loads config, builds the socket layer, starts the server.
- `SRC/Socket.cpp` + `INCLUDES/Sockets.hpp` — server sockets + `epoll` loop.
- `SRC/client.cpp` — per-client read/parse/route/send logic.
- `SRC/req_res/Request.cpp` + `INCLUDES/Request.hpp` — HTTP request validation + extracting method/path/host/content-length.
- `SRC/req_res/Response.cpp` + `INCLUDES/Response.hpp` — build HTTP responses (static file, directory listing, delete, CGI wrapping).
- `SRC/Methodes/*.cpp` + `INCLUDES/Methodes.hpp` — method routing layer (`GetMethod` is used; `PostMethod` exists but is currently not wired in `HandleClient`).
- `SRC/CGI.cpp` + `INCLUDES/CGI.hpp` — CGI execution via `fork()` + `execve()`.
- `SRC/uploadFile.cpp` — nonblocking upload helper for `/uploads`.
- `SRC/default.conf` — default server configuration.
- `STATIC/` — website content served by the server (HTML/CSS/images + CGI scripts under `STATIC/www/www/dir/`).
- `stress_test.py` — simple concurrency stress script (GET `/`).

---

## 2) Build and run

### Build

```sh
make
```

### Run

```sh
./Webserv
```

The server loads `SRC/default.conf` by default (hard-coded in `SRC/runServer.cpp`).

---

## 3) Your config format (and how it’s used)

Default config file: `SRC/default.conf`

```conf
server {
    listen localhost:1033;
    client_max_body_size 100000;

    location / {
        root /STATIC/www/www;
        methods GET POST DELETE;
        index index.html;
        autoindex on;
    }
}
```

### How config is parsed

- `Config::store_file()` reads lines into `file_lines` and calls `stores_config()`.
- `Config::get_firstlast()` finds `server { ... }` blocks by counting braces.
- Each block becomes a `ServerConfig` via `ServerConfig::parse_config()`.
- Each `location ... { ... }` becomes a `LocationConfig` via `LocationConfig::parse_locationConfig()`.

Key code paths:
- `SRC/ConfigFile/Config.cpp` (`store_file`, `stores_config`, `get_firstlast`)
- `SRC/ConfigFile/ServerConfig.cpp` (`store_server_info`)
- `SRC/ConfigFile/LocationConfig.cpp` (`parse_locationConfig`)

### How routing chooses a `location`

`ServerConfig::get_Location_Config(path)` tries to return the **best match** (longest prefix-like match) for the request path.

---

## 4) Sockets for beginners (what a socket is)

A **socket** is a file descriptor (an `int`) that represents a network endpoint.

For a TCP server:

1. `socket(AF_INET, SOCK_STREAM, 0)` creates the endpoint.
2. `bind()` attaches it to an IP:port.
3. `listen()` turns it into a “server socket” waiting for connections.
4. `accept()` creates a **client socket** (new fd) for each incoming connection.

Where it happens in this repo: `SRC/Socket.cpp::CreateSocket()`

```cpp
fd_socket = socket(AF_INET, SOCK_STREAM, 0);
setsockopt(fd_socket, SOL_SOCKET, SO_REUSEADDR, ...);
bind(fd_socket, ...);
listen(fd_socket, 10);
```

### Nonblocking sockets

This server sets sockets to nonblocking:

- In nonblocking mode, `read()` may return `-1` with `errno == EWOULDBLOCK` meaning: “no data available right now”.
- That’s normal when you use an event loop.

Where: `Socket::set_nonblocking()` in `SRC/Socket.cpp`

---

## 5) `epoll` for beginners (why it exists)

When you have many clients, you don’t want:
- one thread per client (heavy), or
- blocking `read()` calls (stalling the whole server).

Linux `epoll` lets you ask the kernel:
- “Wake me up when one of these fds becomes readable/writable.”

### How `epoll` is used here

1. Create an epoll instance: `epoll_create()`
2. Register server sockets (listening fds) with `EPOLLIN` (readable means: incoming connection ready).
3. Loop on `epoll_wait()`.
4. If the event is on a server socket → `accept()`.
5. Else it’s a client fd → read request and respond.

Where: `Socket::CreateEpoll()` + `Socket::Monitor()` in `SRC/Socket.cpp`

```cpp
fd_epoll = epoll_create(1);

// Register listening sockets
for (...) {
  event.events = EPOLLIN;
  event.data.fd = sockconf[i].fd_socket;
  epoll_ctl(fd_epoll, EPOLL_CTL_ADD, sockconf[i].fd_socket, &event);
}

// Main loop
max_fds = epoll_wait(fd_epoll, events, MAX_EVENTS, -1);
```

---

## 6) How requests are read (and why there’s a `ClientState`)

HTTP requests arrive as bytes on the client socket.

Because sockets are nonblocking, a full request (especially uploads) may arrive in **multiple reads**.

This repo stores per-client progress in `ClientState` (in `INCLUDES/Sockets.hpp`) inside:

```cpp
std::map<int, ClientState> status; // key = fd_client
```

Important fields:
- `complete_header` — whether we finished reading the HTTP header.
- `metadata`, `complete_metadata` — extra part used by upload parsing.
- `fd_upload`, `tail` — used to continue writing upload content across reads.

### Reading the header

In `SRC/client.cpp`:

- `_getHeader()` reads lines using `ft_getline()` until an empty line (`\r` / blank) which marks end-of-headers.

Key behavior:
- If a `read()` hits `EWOULDBLOCK`, `ft_getline()` returns a marker string so the server can try again later.

---

## 7) HTTP parsing in this repo (`Request`)

Main parser: `Request::parse_request()` → `check_request()` in `SRC/req_res/Request.cpp`.

What it validates:

### (1) Request line
Example:

```
GET /index.html HTTP/1.1

```

`check_requestline()` verifies:
- Exactly 3 tokens (method, path, version)
- Method is one of: `GET`, `POST`, `DELETE`
- Path starts with `/`
- Version is `HTTP/1.1` or `HTTP/1.0`

### (2) Host header
Example:

```
Host: localhost:1033

```

`check_headerline()` validates:
- Header name is exactly `Host:`
- Value has `host:port`
- Host is `localhost` or a valid IPv4
- Port is 0..65535

It also stores:
- `hostname` (converted from `localhost` to `127.0.0.1`)
- `port`

### (3) Content-Length (POST only)
For POST, `check_request()` scans remaining header lines and looks for:

- `Content-Length:`

Then it:
- stores it into `Request::content_length`
- rejects if it’s not a number
- rejects with `413 Payload Too Large` if it exceeds `client_max_body_size`

`Request::get_content_length()` returns an `int` or `-1` if missing.

---

## 8) How the server decides what to do (routing)

Routing is split across:

- `Socket::HandleClient()` (`SRC/client.cpp`) — reads, calls parser, then chooses behavior.
- `Methodes::GetMethod()` (`SRC/Methodes/GetMethode.cpp`) — handles GET and DELETE using location config and response helpers.

### Server selection (Host + port)

In `HandleClient()`, after parsing, the code loops over `Config::get_allserver_config()` and matches:

- `request.get_Hostname()`
- `request.get_port()`

Then calls `m.GetMethod(...)`.

### Location selection and allowed methods

`GetMethod()`:

1. Selects a `LocationConfig` for the request path
2. Checks if the method is allowed using `LocationConfig::get_method("GET")` / etc
3. Builds the filesystem path:

```cpp
std::string path = gcwd() + info_location.get_root() + test_request.get_path();
```

4. Delegates:
- GET → `Response::Get_response(...)`
- DELETE → `Response::Get_delete(...)`

If method is not allowed → `ErrorResponse::Error_MethodeNotAllowed(a)`

---

## 9) Responses: static file, directory listing, delete

Main file: `SRC/req_res/Response.cpp`

### Serving a file

`Response::Display_file()`:
- `ifstream` reads the file into a string
- Builds HTTP headers with:
  - `Content-Type` from `Config::get_mine(path)`
  - `Content-Length` from body size
- Returns a single string: `headers + "\r\n" + body`

### Autoindex (directory listing)

`Response::Display_dir()` uses `opendir/readdir` and builds an HTML list.

### DELETE

`Response::Get_delete()` uses `remove(path.c_str())` and maps errno to HTTP status:
- `403` for `EACCES` / `ENOTEMPTY`
- `404` for `ENOENT`
- `204 No Content` on success

---

## 10) GET / POST / DELETE in HTTP (and how you implement them)

### GET
Meaning: “fetch a resource”.

In this repo:
- GET is allowed per `location` via `methods ...;`
- GET usually returns either:
  - static file (`.html`, `.css`, ...)
  - directory listing (autoindex)
  - CGI output for `.py`/`.php` files

### DELETE
Meaning: “delete a resource”.

In this repo:
- Implemented inside `Methodes::GetMethod()` → `Response::Get_delete()`.
- Only works if the matched `location` allows DELETE.

### POST
Meaning: “send data to the server” (forms, uploads, API calls).

In this repo there are two different POST-related implementations:

1) **Active**: custom logic inside `Socket::HandleClient()` (`SRC/client.cpp`)
- `/login` uses cookies/session helpers
- `/uploads` uses `_uploadFile()` (streaming multipart upload)

2) **Present but not wired**: `Methodes::PostMethod()` in `SRC/Methodes/PostMethode.cpp`
- Contains upload parsing and register/login logic, but `HandleClient()` currently does not call `m.PostMethod()`.

If your goal is “all POST goes through Methodes::PostMethod”, you’d need a small refactor; this guide documents the current behavior.

---

## 11) CGI (what it is + how it’s implemented)

### What is CGI?

CGI (Common Gateway Interface) is a way for a web server to run a program/script (Python, PHP, …) and treat its stdout as the HTTP response body (or full headers+body).

So instead of reading `file.html` from disk, the server does:

- execute script
- capture script output
- wrap it into an HTTP response

### How CGI is triggered in this repo

In `Response::Get_response()` (`SRC/req_res/Response.cpp`):

- If the requested path points to a file ending in `.py` or `.php`, it runs CGI.

```cpp
const std::string ext = last_path.substr(dot);
if (ext == ".py" || ext == ".php") {
  const char *binary = (ext == ".py") ? "/usr/bin/python3" : "/usr/bin/php";
  CGI cgi("GET", last_path.c_str(), ext.c_str(), binary, default_env);
  if (cgi.CGIProccess())
    return cgi.response;
}
```

### How CGI runs under the hood

`SRC/CGI.cpp`:

- `pipe()` creates a pipe so the parent can read child stdout.
- `fork()` creates a child process.
- Child:
  - `dup2(piped[1], 1)` redirects stdout to the pipe
  - `execve(cgiBinary, execArg, env)` runs the interpreter with the script path
- Parent:
  - reads bytes from `piped[0]`
  - writes them into `temp.txt`
  - wraps result into an HTTP response in `responseWrapper()`

### CGI environment variables

This repo currently uses a fixed env array for GET CGI:

```cpp
static const char *default_env[] = {
  "REQUEST_METHOD=GET",
  "QUERY_STRING=",
  "CONTENT_LENGTH=0",
  "CONTENT_TYPE=",
  "GATEWAY_INTERFACE=CGI/1.1",
  "SERVER_PROTOCOL=HTTP/1.1",
  NULL
};
```

That means:
- Query strings are not forwarded yet (`QUERY_STRING` is empty).
- POST CGI is not wired yet (always `REQUEST_METHOD=GET`).

### How to test CGI (GET)

There are CGI scripts in `STATIC/www/www/dir/` (for example `test.py`, `test.php`).

Assuming your config root is `/STATIC/www/www` and the request path is `/dir/test.py`:

```sh
curl -i http://localhost:1033/dir/test.py
curl -i http://localhost:1033/dir/test.php
```

### How to test CGI (POST)

In the current code path, CGI is only executed from `Response::Get_response()` and is hard-coded to GET env vars.

So a real CGI POST test is not supported yet (you can document it as a TODO / next feature).

### Unit-style CGI test in this repo

There is a small executable test file `SRC/test_cgi.cpp` that calls `Response::Get_response()` directly.

You can compile and run it manually (example):

```sh
c++ -std=c++98 -Wall -Wextra -Werror -g \
  SRC/test_cgi.cpp SRC/req_res/Response.cpp SRC/req_res/ErrorResponse.cpp SRC/CGI.cpp \
  SRC/ConfigFile/Config.cpp SRC/ConfigFile/ServerConfig.cpp SRC/ConfigFile/LocationConfig.cpp \
  SRC/ConfigFile/Syntax_server.cpp SRC/ConfigFile/Syntax_location.cpp SRC/req_res/Request.cpp \
  -IINCLUDES -o test_cgi

./test_cgi
```

(That build line may need adjusting depending on your include dependencies; it’s here mainly to show intent.)

---

## 12) Query strings (what they are + how you handle them)

A query string is the part after `?` in a URL:

```
GET /session?name=ayoub HTTP/1.1
```

In this repo, `SRC/client.cpp` contains a simple extractor:

- `extract_name(request_string)` searches the **start line** for `?name=` and returns its value.

This is used together with cookies:

- `extract_cookie_username(request_string)` reads `Cookie:` header and extracts `username=...`

Then the server compares them and answers with either `FOOUND!` or `NOT FOUND!` using `CheckSession()`.

This is a minimal “query + cookie” demo, not a general query parser.

---

## 13) Uploads (multipart/form-data) in this repo

This server implements uploads under the path:

- `POST /uploads`

### What the browser sends

A typical HTML form upload uses `multipart/form-data` with a boundary.

The request looks like:

- Headers contain:
  - `Content-Type: multipart/form-data; boundary=...`
- Body contains parts:
  - metadata headers for a file (`Content-Disposition: form-data; name=...; filename="..."`)
  - then raw file bytes
  - ends with `--boundary--`

### How uploads are implemented here (nonblocking-safe)

Main implementation: `SRC/uploadFile.cpp` via `_uploadFile(fd_client, state)`.

Key ideas:

1) Extract boundary from the request header:

```cpp
size_t pos = header.find("boundary=");
...
boundary = header.substr(pos, end - pos);
```

2) Extract filename from the multipart metadata (read earlier into `state.metadata`):

```cpp
size_t filename_pos = metadata.find("filename=\"");
...
filename = metadata.substr(filename_pos, end_quote - filename_pos);
```

3) Stream-write bytes to `STATIC/upload/<filename>`.

4) Detect the end boundary even if it splits across reads.

To do that, the code keeps a `state.tail` buffer that remembers the last `end_boundary.size()-1` bytes from the previous chunk.

On each read:
- concatenate `tail + new_bytes`
- search for end boundary
- write everything except the portion that might be part of a future boundary

Also important for `epoll` nonblocking:
- If `read()` returns `-1` with `EWOULDBLOCK`, `_uploadFile()` returns `false` and the caller (`HandleClient`) will retry when epoll says the fd is readable again.

### How to test upload

If your HTML upload page posts to `/uploads`, you can test via browser.

You can also test via curl (multipart):

```sh
curl -i -F "file=@STATIC/www/www/index.html" http://localhost:1033/uploads
```

Uploaded files end up in `STATIC/upload/`.

---

## 14) How to test the server (practical checklist)

### Basic GET

```sh
curl -i http://localhost:1033/
curl -i http://localhost:1033/index.html
```

### Directory listing (autoindex)

```sh
curl -i http://localhost:1033/dir/
```

### DELETE

Be careful: this deletes from your filesystem path mapping.

Example (if the file exists under your root mapping):

```sh
curl -i -X DELETE http://localhost:1033/dir/somefile.txt
```

### CGI

```sh
curl -i http://localhost:1033/dir/test.py
curl -i http://localhost:1033/dir/test.php
```

### Upload

```sh
curl -i -F "file=@STATIC/www/www/index.html" http://localhost:1033/uploads
```

### Stress test

`stress_test.py` fires many GET requests:

```sh
python3 stress_test.py
```

---

## 15) Gotchas / limitations (useful when debugging)

- `Socket::set_nonblocking()` uses `fcntl(fd, F_SETFL, O_NONBLOCK)` without preserving existing flags (common improvement is `F_GETFL` then OR `O_NONBLOCK`).
- Header reading is line-by-line and char-by-char (`ft_getline`), which is simple but can be slow under load.
- `HandleClient()` has special-case logic for POST (`/uploads`, `/login`) and does not currently route POST through `Methodes::PostMethod()`.
- CGI is currently GET-only in practice: `REQUEST_METHOD=GET` and `QUERY_STRING=` are fixed.
- `CGI.cpp` writes output to a file named `temp.txt` in the working directory; it also opens `./SRC/temp/temp.txt` in the constructor but doesn’t write to that stream.

---

## 16) If you want the “next steps” (optional)

If you want, I can help you turn this into a cleaner beginner architecture by:

- Routing POST through `Methodes::PostMethod()` (single place for all methods)
- Proper query parsing (decode `%xx`, handle `&k=v`)
- CGI GET with real `QUERY_STRING` and CGI POST with stdin body
- More robust HTTP parsing (multiple headers, keep-alive, chunked, etc)

Just tell me what scope you want.
