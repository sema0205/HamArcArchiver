
#include "lib/headers/HamArc.h"

// example of creation of archive with some files
// --create --file=C:/testsforlabs/newarchive C:/testsforlabs/download.jpeg C:/testsforlabs/habr1.txt -b 8

// example of extraction all files from archive C:/testsforlabs/archive
// -f C:/testsforlabs/archive --extract -p C:/testsforlabs/test -b 8

// example of extraction certain files from archive C:/testsforlabs/archive (path is C:/testsforlabs/archive.haf but u should input without .haf)
// -f C:/testsforlabs/archive --extract -p C:/testsforlabs/test -b 8 C:/testsforlabs/Labwork4.pdf

// example concatenation of two archives newarchive.haf and newarchiveonemore.haf to result archive which is C:/testsforlabs/resultarchive.haf
// --concantenate C:/testsforlabs/newarchiveonemore C:/testsforlabs/newarchive -f C:/testsforlabs/resultarchive -b 8

// example of adding file to archive
// --append --file=C:/testsforlabs/newarchive C:/testsforlabs/Labwork4.pdf


int main(int argc, char* argv[]) {

    SetVariables(argc, argv);

    return 0;
}


