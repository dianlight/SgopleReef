#include "web_server.favicon.ico.gz.h"
#include "web_server.flaticon.woff2.h"
#include "web_server.index.html.gz.h"
#include "web_server.main.css.gz.h"
#include "web_server.main.js.gz.h"

#define  _CONTENT_TYPE_HTML "text/html"
#define  _CONTENT_TYPE_CSS  "text/css"
#define  _CONTENT_TYPE_JS   "application/javascript"
#define  _CONTENT_TYPE_PNG  "image/png"
#define  _CONTENT_TYPE_GIF  "image/gif"
#define  _CONTENT_TYPE_JPEG "image/jpeg"
#define  _CONTENT_TYPE_ICO  "image/x-icon"
#define  _CONTENT_TYPE_XML  "text/xml"
#define  _CONTENT_TYPE_PDF  "application/x-pdf"
#define  _CONTENT_TYPE_ZIP  "application/x-zip"
#define  _CONTENT_TYPE_GZ   "application/x-gzip"
#define  _CONTENT_TYPE_TEXT "text/plain"
struct StaticFile
{

    String filename;
    const uint8_t *data;
    size_t length;
    String mimetype;
};
StaticFile staticFiles[] = {
  { "/favicon.ico.gz", CONTENT_FAVICON_ICO_GZ, CONTENT_FAVICON_ICO_GZ_LEN, _CONTENT_TYPE_ICO },
  { "/flaticon.woff2", CONTENT_FLATICON_WOFF2, CONTENT_FLATICON_WOFF2_LEN, _CONTENT_TYPE_TEXT },
  { "/index.html.gz", CONTENT_INDEX_HTML_GZ, CONTENT_INDEX_HTML_GZ_LEN, _CONTENT_TYPE_HTML },
  { "/main.css.gz", CONTENT_MAIN_CSS_GZ, CONTENT_MAIN_CSS_GZ_LEN, _CONTENT_TYPE_CSS },
  { "/main.js.gz", CONTENT_MAIN_JS_GZ, CONTENT_MAIN_JS_GZ_LEN, _CONTENT_TYPE_JS },
};
#define STATIC_FILES_SIZE 5

