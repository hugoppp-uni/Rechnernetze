# Directories
PROJECT_DIR="/media/maik/shared/dev/CLionProjects/Studium/RN"
DOCUMENT_ROOT_FOLDER=$PROJECT_DIR"/DOCUMENT_ROOT_FOLDER"
DOWNLOAD_DIR=$PROJECT_DIR"/test/download"

# HTTP Server
HTTP_SERVER_BIN=$PROJECT_DIR"/cmake-build-debug/http_server/http_server"
HTTP_SERVER_PORT=8000
HTTP_SERVER_URL="localhost:$HTTP_SERVER_PORT"


# Function for executing a test case:
# 1. Download file (passed as first parameter)
# 2. Compare SHA256 Checksum of downloaded file to original file
# 3. Print result to stdout
test () {
   RESOURCE="$1"
   echo
   echo "TESTING $RESOURCE"
   wget -x -O "$DOWNLOAD_DIR/$RESOURCE" "$HTTP_SERVER_URL/$RESOURCE"
   sha256sum "$DOCUMENT_ROOT_FOLDER/$RESOURCE" "$DOWNLOAD_DIR/$RESOURCE"
   SUM_1=$(echo $(sha256sum "$DOCUMENT_ROOT_FOLDER/$RESOURCE") | awk '{print $1}')
   SUM_2=$(echo $(sha256sum "$DOWNLOAD_DIR/$RESOURCE") | awk '{print $1}')
   if [ "$SUM_1" == "$SUM_2" ]; then
       echo "$RESOURCE OK"
   else
     echo "$RESOURCE FAIL: Checksum is wrong: $SUM_2"
   fi
   diff "$DOCUMENT_ROOT_FOLDER/$RESOURCE" "$DOWNLOAD_DIR/$RESOURCE"
}


# Execute test cases
test "graphics/pedestrian.svg"
test "image/haw-hamburg.jpeg"
test "image/muscari-1024x1024.html"
test "pdf/haw-calendar.pdf"
test "pdf/saltzer.pdf"
test "text/gpl-3.0.txt"
test "text/rfc793.txt"
test "text/Test.txt"
test "video/nasa-epic.mp4"
test "video/nasa-earth-observatory.mp4"