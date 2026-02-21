# Webserv (C++98)

A small HTTP server project (42-style) using Linux `epoll`.

- Start here: `docs/BEGINNER_GUIDE.md`
- Default config: `SRC/default.conf`

## Build

```sh
make
```

## Run

```sh
./Webserv
```

By default it listens on `localhost:1033` (see `SRC/default.conf`).

## CGI

Configure CGI interpreters per `location` with `cgi_pass` (extension → interpreter path), for example:

```conf
location /upload/ {
	root /www;
	methods GET POST DELETE;
	autoindex on;
	cgi_pass .py /usr/bin/python3;
	cgi_pass .php /usr/bin/php;
}
```

## Uploads UI

- Upload form: `http://localhost:1033/upload.html`
- Uploads manager (open/delete): `http://localhost:1033/uploads_manager.html`

