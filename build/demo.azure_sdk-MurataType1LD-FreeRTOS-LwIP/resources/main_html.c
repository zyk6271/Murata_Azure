#include "wiced_resource.h"

const char resources_apps_DIR_http_server_sent_events_DIR_main_html_data[1865] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n" \
"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\r\n" \
"\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\r\n" \
"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\r\n" \
"  <head>\r\n" \
"    <title>Cypress WICED HTTP Server-Sent Event (SSE) Example Application</title>\r\n" \
"    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\r\n" \
"  </head>\r\n" \
"  <body onload=\"reloadData( )\" style=\"font-family:verdana;\">\r\n" \
"    <table border=\'0\' cellpadding=\'0\' cellspacing=\'0\' width=\"98%\">\r\n" \
"      <tr style=\"height:20px\"><td>&nbsp;</td></tr>\r\n" \
"      <tr style=\"border-collapse: collapse; padding: 0;\">\r\n" \
"        <td style=\"width:20px\"></td>\r\n" \
"        <td style=\"width:152px\"><img src=\"../../images/cypresslogo.png\" alt=\"Cypress Logo\" /></td>\r\n" \
"        <td style=\"vertical-align:middle; text-align:center; font: bold 25px/100% Verdana, Arial, Helvetica, sans-serif;background-image:url(\'../../images/cypresslogo_line.png\');\">\r\n" \
"        WICED&trade; HTTP Server-Sent Event (SSE) Example Application\r\n" \
"        </td>\r\n" \
"        <td style=\"width:137px;background-image:url(\'../../images/cypresslogo_line.png\');\"></td>\r\n" \
"      </tr>\r\n" \
"      <tr><td>&nbsp;</td></tr>\r\n" \
"    </table>\r\n" \
"    <div style=\"margin-left:20px\" id=\"main_text\" >\r\n" \
"      <p>This webpage demonstrates how a webserver notifies its clients using <b>Server-Sent Event (SSE)</b>.</p>\r\n" \
"    </div>\r\n" \
"  </div>\r\n" \
"  <div id=\"result\"></div>\r\n" \
"  <script>\r\n" \
"    if(typeof(EventSource) !== \"undefined\") {\r\n" \
"      var source = new EventSource(\"events\");\r\n" \
"        source.onmessage = function(event) \r\n" \
"        {\r\n" \
"          document.getElementById(\"result\").innerHTML += event.data + \"<br>\";\r\n" \
"        };\r\n" \
"      } \r\n" \
"      else \r\n" \
"      {\r\n" \
"        document.getElementById(\"result\").innerHTML = \"Sorry, your browser does not support server-sent events...\";\r\n" \
"      }\r\n" \
"    </script>\r\n" \
"  </body>\r\n" \
"</html>";
const resource_hnd_t resources_apps_DIR_http_server_sent_events_DIR_main_html = { RESOURCE_IN_MEMORY, 1864, { .mem = { resources_apps_DIR_http_server_sent_events_DIR_main_html_data }}};
