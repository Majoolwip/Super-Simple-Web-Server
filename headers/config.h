/*
   Conforms to the suckless ideology, have config in code.
   Config in code speeds execution and reduces code.
   More code means more potential for bugs and security vulnerabilities.
*/
#ifndef CONFIG_HEADER
#define CONFIG_HEADER
#define PORT 3112          /* Port to listen to for connections, 80 is the standard for http */
#define BUFFER_SIZE 8096
#define CONNECT_BACKLOG_SIZE 64
#define CHILD_LIMIT 256
#define HOST_NAME "majoolwip.com"
#define ROOT_DIR  "." /* Where the program should look for files like index.html */
#define HELP_STR  "name:\n"\
                  "  pinite\n"\
                  "\n"\
                  "synopsis:\n"\
                  "  pinite [flags]\n"\
                  "\n"\
                  "description\n"\
                  "  Pinite is a small, simple, and secure web-server design with Raspberry Pi in mind.\n"\
                  "\n"\
                  "flags:\n"\
                  "  -h --help or anything:\tDisplays the help page\n\n"

#define HTTP10_OK_HEADER   "HTTP/1.0 200 OK\r\n"\
                           "Host: " HOST_NAME "\r\n"\
                           "Content-Type:  %s\r\n"\
                           "Content-Length: %d\r\n"\
                           "\r\n"

#define HTTP10_404         "HTTP/1.0 404 Not Found\r\n"\
                           "Content-Type: text/html\r\n"\
                           "Host: " HOST_NAME "\r\n"\
                           "Content-Length: 221\r\n"\
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

#define HTTP10_NON_GET     "HTTP/1.0 400 Bad Request\r\n"\
                           "Content-Type: text/html\r\n"\
                           "Host: " HOST_NAME "\r\n"\
                           "Content-Length: 215\r\n"\
                           "\r\n"\
                           "<!doctype html>\n"\
                           "<html lang='en'>\n"\
                           "  <head>\n"\
                           "    <meta charset='utf-8'>\n"\
                           "    <title>File Not Found</title>\n"\
                           "  </head>\n"\
                           "  <body>\n"\
                           "    <h1>Error: Non-Get Request!</h1>\n"\
                           "    <p>Only get request are supported!</p>\n"\
                           "  </body>\n"\
                           "</html>\n"

#define HTTP10_USED_PARENT "HTTP/1.0 400 Bad Request\r\n"\
                           "Content-Type: text/html\r\n"\
                           "Host: " HOST_NAME "\r\n"\
                           "Content-Length: 220\r\n"\
                           "\r\n"\
                           "<!doctype html>\n"\
                           "<html lang='en'>\n"\
                           "  <head>\n"\
                           "    <meta charset='utf-8'>\n"\
                           "    <title>File Not Found</title>\n"\
                           "  </head>\n"\
                           "  <body>\n"\
                           "    <h1>Error: Parent Path Request!</h1>\n"\
                           "    <p>Parent path's are not supported!</p>\n"\
                           "  </body>\n"\
                           "</html>\n"

#define HTTP10_INVALID_CONTENT_TYPE  "HTTP/1.0 400 Bad Request\r\n"\
                                     "Content-Type: text/html\r\n"\
                                     "Host: " HOST_NAME "\r\n"\
                                     "Content-Length: 222\r\n"\
                                     "\r\n"\
                                     "<!doctype html>\n"\
                                     "<html lang='en'>\n"\
                                     "  <head>\n"\
                                     "    <meta charset='utf-8'>\n"\
                                     "    <title>File Not Found</title>\n"\
                                     "  </head>\n"\
                                     "  <body>\n"\
                                     "    <h1>Error: Invalid filetype!</h1>\n"\
                                     "    <p>Only certain filetypes are supported!</p>\n"\
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
  {0, 0}
};
#endif
