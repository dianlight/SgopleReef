from os.path import join, isfile, isdir, basename
from os import listdir, system, environ
import json
from pprint import pprint
import re
import requests
import subprocess
import sys

Import("env")

# Install pre-requisites
npm_installed = (0 == system("npm --version"))

#
# Dump build environment (for debug)
# print env.Dump()
#print("Current build targets", map(str, BUILD_TARGETS))
#

def get_c_name(source_file):
    return basename(source_file).upper().replace('.', '_').replace('-', '_')

def text_to_header(source_file):
    with open(source_file) as source_fh:
        original = source_fh.read()
    filename = get_c_name(source_file)
    output = "static const char CONTENT_{}[] PROGMEM = ".format(filename)
    for line in original.splitlines():
        output += u"\n  \"{}\\n\"".format(line.replace('\\', '\\\\').replace('"', '\\"'))
    output += ";\n"
    return output

def binary_to_header(source_file):
    filename = get_c_name(source_file)
    output = "const uint8_t CONTENT_"+filename+"[] PROGMEM = {\n  "
    count = 0
    tcount = 0

#    print("Reading: {}".format(source_file))
    with open(source_file, "rb") as source_fh:
        byte = source_fh.read(1)
        while byte != b"":
            output += "0x{:02x}, ".format(ord(byte))
            count += 1
            tcount += 1
            if 16 == count:
                output += "\n  "
                count = 0

            byte = source_fh.read(1)

    output += "0x00 };\n"
    return "#define CONTENT_{}_LEN {}\n{}".format(filename,tcount,output)

def data_to_header(env, target, source):
    output = ""
    for source_file in source:
#        print("Reading {}".format(source_file))
        file = source_file.get_abspath()
        if file.endswith(".css") or file.endswith(".js") or file.endswith(".htm") or file.endswith(".html"):
            output += text_to_header(file)
        else:
            output += binary_to_header(file)
    target_file = target[0].get_abspath()
#    print("Generating {}".format(target_file))
    with open(target_file, "w") as output_file:
        output_file.write(output)

def make_static(env, target, source):
    output = ""

    out_files = []
    for file in listdir(dist_dir):
        if isfile(join(dist_dir, file)):
          out_files.append(file)

    # Sort files to make sure the order is constant
    out_files = sorted(out_files)

    # include the files
    for out_file in out_files:
        filename = "web_server."+out_file+".h"
        output += "#include \"{}\"\n".format(filename)

    output +=    """
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
{\n
    String filename;
    const uint8_t *data;
    size_t length;
    String mimetype;
};\n"""
    output += "StaticFile staticFiles[] = {\n"
    count = 0;

    for out_file in out_files:
        filetype = "TEXT"
        if out_file.endswith(".css") or out_file.endswith(".css.gz") :
            filetype = "CSS"
        elif out_file.endswith(".js") or out_file.endswith(".js.gz"):
            filetype = "JS"
        elif out_file.endswith(".htm") or out_file.endswith(".html") or out_file.endswith(".htm.gz") or out_file.endswith(".html.gz"):
            filetype = "HTML"
        elif out_file.endswith(".jpg") or out_file.endswith(".jpg.gz"):
            filetype = "JPEG"
        elif out_file.endswith(".png") or out_file.endswith(".png.gz"):
            filetype = "PNG"
        elif out_file.endswith(".ico") or out_file.endswith(".ico.gz"):
            filetype = "ICO"
        elif out_file.endswith(".xml") or out_file.endswith(".xml.gz"):
            filetype = "XML"
        elif out_file.endswith(".pdf") or out_file.endswith(".pdf.gz"):
            filetype = "PDF"
        elif out_file.endswith(".zip"):
            filetype = "ZIP"
        elif out_file.endswith(".gz"):
            filetype = "GZ"

        c_name = get_c_name(out_file)
        output += "  { \"/"+out_file+"\", CONTENT_"+c_name+", CONTENT_"+c_name+"_LEN, _CONTENT_TYPE_"+filetype+" },\n"
        count += 1

    output += "};\n"

    output += "#define STATIC_FILES_SIZE {}\n\n".format(count)
    target_file = target[0].get_abspath()
#    print("Generating {}".format(target_file))
    with open(target_file, "w") as output_file:
        output_file.write(output)

def process_html_app(source, dest, env):

    web_server_static_files = join(dest, "web_server_static_files.h")
    web_server_static = join("$BUILDSRC_DIR", "web_server_static.cpp.o")

    for file in sorted(listdir(source)):
        if isfile(join(source, file)):
#            print("Process file: ",file);
            data_file = join(source, file)
            header_file = join(dest, "web_server."+file+".h")
#            print("Data: {} Header: {}".format(data_file,header_file))
            env.Command(header_file, data_file, data_to_header)
            env.Depends(web_server_static_files, header_file)

    env.Depends(web_server_static, env.Command(web_server_static_files, source, make_static))

#
# Generate Web app resources
#
#print("PYTHON ENV:{}\n".format(env.Dump()))
if npm_installed:
    headers_src = join(env.subst("$PROJECTSRC_DIR"), "web_static")

    gui_dir = join(env.subst("$PROJECTSRC_DIR"), "node")
    dist_dir = join(env.subst("$PROJECT_DIR"), "data")
    node_modules = join(env.subst("$PROJECT_DIR"), "node_modules")

    my_env = environ.copy()
    my_flags = env.ParseFlags(env['BUILD_FLAGS'])
    for item in my_flags.get("CPPDEFINES"):
        if isinstance(item,list):
#            print(item[0],item[1])
            my_env["DEFINE_"+item[0]] = item[1]
        elif isinstance(item,str):
#            print(item,'True')
            my_env["DEFINE_"+item] = 'True' 
        else:
            print(item)       
        
    my_env["PROGNAME"] = env["PROGNAME"]
    if "UPLOAD_PORT" in env :
        my_env["UPLOAD_PORT"] = env["UPLOAD_PORT"]


    subprocess.check_call("npm run compile",shell=True,env=my_env)
    # Check to see if the Node modules have been downloaded
    if(isdir(node_modules)):
        if(isdir(dist_dir)):
            process_html_app(dist_dir, headers_src, env)
        else:
            print("Warning: GUI not built, run 'cd %s; npm run build'" % gui_dir)
    else:
        print("Warning: GUI dependencies not found, run 'cd %s; npm install'" % gui_dir)
else:
  print("Warning: Node.JS and NPM required to update the UI")