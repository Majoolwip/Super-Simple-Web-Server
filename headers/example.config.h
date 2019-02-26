/*
   Conforms to the suckless ideology, have config in code.
   Config in code speeds execution and reduces code.
   More code means more potential for bugs and security vulnerabilities.
*/
#ifndef CONFIG_HEADER
#define CONFIG_HEADER
#define PORT 80          /* Port to listen to for connections, 80 is the standard for http */
#define BUFFER_SIZE 8096
#define CONNECT_BACKLOG_SIZE 64
#define ROOT_DIR  "."      /* Where the program should look for files like index.html */
#define HELP_STR\
  "name:\n"\
  "  SSWS\n"\
  "\n"\
  "synopsis:\n"\
  "  SSWS [flags]\n"\
  "\n"\
  "description\n"\
  "  Super-Simple-Web-Server is a small, simple, and secure web-server.\n"\
  "\n"\
  "flags:\n"\
  "  -h --help or anything:\tDisplays the help page\n\n"

#define HTTP10_OK_HEADER\
  "HTTP/1.0 200 OK\r\n"\
  "Content-Type:  %s\r\n"\
  "Content-Length: %d\r\n"\
  "\r\n"

#define HTTP10_404\
  "HTTP/1.0 404 Not Found\r\n"\
  "Content-Type: text/html\r\n"\
  "Content-Length: 226\r\n"\
  "\r\n"\
  "<!doctype html>\n"\
  "<html lang='en'>\n"\
  "  <head>\n"\
  "    <meta charset='utf-8'>\n"\
  "    <title>File Not Found</title>\n"\
  "  </head>\n"\
  "  <body>\n"\
  "    <h1>Error: File not found!</h1>\n"\
  "    <p>Could not find the file you requested!</p>\n"\
  "  </body>\n"\
  "</html>\n"

static const struct {
  const char* extension;
  const char* content_type;
} extensions[] = {
  {"htm",   "text/html"},
  {"html",  "text/html"},
  {"js",    "text/javascript"},
  {"css",   "text/css"},
  {"png",   "image/png"},
  {"jpg",   "image/jpeg"},
  {"jpeg",  "image/jpeg"},
  {"ico",   "image/x-icon"},
  {0, 0}
};
#endif
